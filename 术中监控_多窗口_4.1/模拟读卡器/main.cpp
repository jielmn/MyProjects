#ifdef _DEBUG
#include "vld.h"
#endif

#include "LmnCommon.h"
#include "LmnThread.h"
#include "LmnSerialPort.h"

#define   COM_PORT                    "com3"
#define   RECONNECT_TIME              10000

#define   MSG_CONNECT                 1            // 打开串口
#define   MSG_READ_COM                2            // 读串口数据并处理

// 制造1A和16A的tag冲突 
#define   TAG_CONFLICT      1

// 第3A Tag读取3次后，改变ID
#define   TAG_ID_CHANGE     1

class MyMessageHandler : public LmnToolkits::MessageHandler {
public:
	void OnMessage(DWORD dwMessageId, const LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};

static  MyMessageHandler  g_handler;
static LmnToolkits::Thread *  g_thrd = 0;
static  CLmnSerialPort    g_com;
static  CDataBuf          g_buf;

static  BYTE  g_ReaderId[180][11] = { 0 };
static  BYTE  g_TagId[180][8] = { 0 };

// 上一次温度数据
static  DWORD  g_Temp[180] = { 0 };

DWORD  GetRandTemp(BYTE  byBed) {
	int  delta     = GetRand(0, 30);
	BOOL bPositive = (BOOL)GetRand(0, 1);

	if ( g_Temp[byBed] == 0) {
		g_Temp[byBed] = GetRand(3200, 4080);
	}
	
	if (bPositive) {
		g_Temp[byBed] += delta;
	}
	else {
		g_Temp[byBed] -= delta;
	}

	if ((int)g_Temp[byBed] < 3200) {
		g_Temp[byBed] = 3200;
	}
	else if ((int)g_Temp[byBed] > 4080) {
		g_Temp[byBed] = 4080;
	}

	return g_Temp[byBed];
}

void MyMessageHandler::OnMessage(DWORD dwMessageId, const LmnToolkits::MessageData * pMessageData) {
	if ( dwMessageId == MSG_CONNECT ) {
		if ( g_com.GetStatus() == CLmnSerialPort::OPEN ) {
			g_com.CloseUartPort();
		}

		g_com.OpenUartPort(COM_PORT);
		if ( g_com.GetStatus() == CLmnSerialPort::OPEN ) {
			g_buf.Clear();
			g_thrd->PostMessage(&g_handler, MSG_READ_COM);
		}
		else {
			g_thrd->PostDelayMessage( RECONNECT_TIME, &g_handler, MSG_CONNECT );
		}
	}
	else if ( dwMessageId == MSG_READ_COM ) {
		BYTE   byData[128];
		DWORD  dwRcvLen = 0;
		DWORD  dwWriteLen = 0;
		dwRcvLen = sizeof(byData);
		g_com.Read( byData, dwRcvLen );
		while ( dwRcvLen > 0 ) {
			g_buf.Append(byData, dwRcvLen);
			g_com.Read(byData, dwRcvLen);
		}

		if ( g_buf.GetDataLength() >= 8 ) {
			g_buf.Read( byData, 8 );

			BYTE byBed  = byData[1];
			BYTE byArea = byData[2];
			//"\x00\x00\x00\x55\x01\x01\xDD\xAA"
			//"\x00\x00\x00\x55\x01\x02\xDD\xAA"
			// 心跳
			if ( byData[5] == 0x01 ) {
				DWORD r = GetRand(1, 5);
				if ( r > 1 ) {
					//55 0E 00 06 01 45 52 00 00 03 00 00 00 00 00 01 FF
					memcpy(byData, "\x55\x0E\x00\x06\x01\x45\x52\x00\x00\x03\x00\x00\x00\x00\x00\x01\xFF", 17);
					byData[3] = byBed;
					byData[4] = byArea;
					dwWriteLen = 17;
					g_com.Write( byData, dwWriteLen );
				}
			}
			// 温度
			else if ( byData[5] == 0x02 ) {
				DWORD r = GetRand(1, 5);
				if ( r > 1 ) {
					memcpy(byData, "\x55\x1A\x00\x06\x01\x45\x52\x00\x00\x03\x00\x00\x00\x00\x00\x01\x8F\x50\xD9\x93\xCD\x59\x02\xE0\x02\x07\x08\x05\xFF\xDD\xAA", 31);
					byData[3] = byBed;
					byData[4] = byArea;
					dwWriteLen = 31;
					//DWORD dwTemp = GetRand(3200, 4080);
					DWORD dwTemp = GetRandTemp(byBed);
					byData[24] = (BYTE)(dwTemp / 1000);
					byData[25] = (BYTE)(( dwTemp / 100 ) % 10);
					byData[26] = (BYTE)(( dwTemp / 10) % 10);
					byData[27] = (BYTE)(dwTemp  % 10);

					if ( byBed <= 180 && byBed > 0 ) {
						memcpy(byData + 5,  g_ReaderId[byBed-1], 11);
						memcpy(byData + 16, g_TagId[byBed-1], 8);
					}

#if TAG_ID_CHANGE
					if ( byBed == 13 ) {
						static int nCnt = 0;

						if (nCnt >= 3) {
							byData[17] = 0x03;
						}
						nCnt++;
					}
#endif

					g_com.Write(byData, dwWriteLen);
				}
			}

			g_buf.Reform();
		}

		g_thrd->PostMessage(&g_handler, MSG_READ_COM);
	}
}

void HandleChoice(ConsoleMenuHandle hMenu, const void * pArg, DWORD dwIndex) {
	if ( dwIndex == 0 ) {
		if (0 == g_thrd) {
			g_thrd = new LmnToolkits::Thread;
			g_thrd->Start();
			g_thrd->PostMessage(&g_handler, MSG_CONNECT);
		}
		else {
			printf("已经启动模拟！\n");
		}
	}
	else if (dwIndex == 1) {
		if (0 != g_thrd) {
			g_thrd->Stop();
			delete g_thrd;
			g_thrd = 0;
		}
		else {
			printf("还没有启动模拟！\n");
		}
	}
}

DWORD SelectChoice (ConsoleMenuHandle hMenu, const void * pArg, const char * szChoice) {
	if ( 0 == strcmp(szChoice, "1") ) {
		return 0;
	}
	else if (0 == strcmp(szChoice, "2")) {
		return 1;
	}
	else if (0 == strcmp(szChoice, "3")) {
		if (0 != g_thrd) {
			g_thrd->Stop();
			delete g_thrd;
			g_thrd = 0;
		}
		return QUIT_CONSOLE_MENU;
	}
	return 100;
}




int main() {
	LmnToolkits::ThreadManager::GetInstance();
	InitRand(TRUE,1);

	for ( int i = 0; i < 180; i++ ) {
		memcpy(g_ReaderId[i], "\x45\x52\x00\x00\x04\x00\x00\x00\x00\x00\x01", 11);

		g_ReaderId[i][8]  = (i + 1) / 100;
		g_ReaderId[i][9]  = ((i + 1) % 100) / 10;
		g_ReaderId[i][10] = (i + 1) % 10;

		memcpy(g_TagId[i], "\x00\x00\x00\x00\x00\x00\x29\xe0", 8);
#if TAG_CONFLICT
		if (i == 90) {
			g_TagId[i][0] = (BYTE)(0 + 1);
		}
		else if (i == 84) {
			g_TagId[i][0] = (BYTE)(6 + 1);
		}
		else {
			g_TagId[i][0] = (BYTE)(i + 1);
		}
#else
		g_TagId[i][0] = (BYTE)(i + 1);
#endif
	}

	TConsoleMenuItem  item[3];
	STRNCPY(item[0].szName, "1.开始模拟", sizeof(item[0].szName));
	item[0].hMenu = 0;
	STRNCPY(item[1].szName, "2.停止模拟", sizeof(item[1].szName));
	item[1].hMenu = 0;
	STRNCPY(item[2].szName, "3.退出", sizeof(item[2].szName));
	item[2].hMenu = 0;
	
	InitConsole(SelectChoice, HandleChoice);
	ConsoleMenuHandle h = CreateConsoleMenu("模拟读卡器");
	int ret = AddConsoleMenuItem(h, &item[0]);
	ret = AddConsoleMenuItem(h, &item[1]);
	ret = AddConsoleMenuItem(h, &item[2]);
	DisplayConsoleMenu(h);
	DeinitConsole();

	LmnToolkits::ThreadManager::ReleaseInstance();
	return 0;
}