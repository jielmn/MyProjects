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
#define   MSG_CONNECT_1               3 
#define   MSG_SEND_DATA_1             4             // 4 - 100

// 制造1A和16A的tag冲突 
#define   TAG_CONFLICT      1

// 第3A Tag读取3次后，改变ID
#define   TAG_ID_CHANGE     1

class MyMessageHandler : public LmnToolkits::MessageHandler {
public:
	void OnMessage(DWORD dwMessageId, const LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};

class MyMessageHandler_1 : public LmnToolkits::MessageHandler {
public:
	void OnMessage(DWORD dwMessageId, const LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};

static  MyMessageHandler  g_handler;
static  MyMessageHandler_1  g_handler_1;
static LmnToolkits::Thread *  g_thrd = 0;
static LmnToolkits::Thread *  g_thrd_1 = 0;
static  CLmnSerialPort    g_com;
static  CDataBuf          g_buf;

static  BYTE  g_ReaderId[180][11] = { 0 };
static  BYTE  g_TagId[180][8] = { 0 };

#define  MAX_HAND_TAG_CNT 10
static  BYTE  g_TagId_1[MAX_HAND_TAG_CNT][8] = { 0 };
static  BYTE  g_CardId_1[MAX_HAND_TAG_CNT][8] = { 0 };
static  int   g_SendCnt[MAX_HAND_TAG_CNT] = { 0 };
static  int   g_HandDelay[MAX_HAND_TAG_CNT] = {10000, 11000, 12000,13000,14000,15000,16000,17000,18000,19000};

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
			g_buf.Clear();
			g_thrd->PostMessage(&g_handler, MSG_READ_COM);
		}
		else {
			g_com.OpenUartPort(COM_PORT);
			if (g_com.GetStatus() == CLmnSerialPort::OPEN) {
				g_buf.Clear();
				g_thrd->PostMessage(&g_handler, MSG_READ_COM);
			}
			else {
				g_thrd->PostDelayMessage(RECONNECT_TIME, &g_handler, MSG_CONNECT);
			}
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

void MyMessageHandler_1::OnMessage(DWORD dwMessageId, const LmnToolkits::MessageData * pMessageData) {
	if (dwMessageId == MSG_CONNECT_1) {
		if (g_com.GetStatus() == CLmnSerialPort::OPEN) {
			for (int i = 0; i < MAX_HAND_TAG_CNT; i++) {
				g_thrd_1->PostMessage(&g_handler_1, MSG_SEND_DATA_1+i);
			}			
		}
		else {
			g_com.OpenUartPort(COM_PORT);
			if (g_com.GetStatus() == CLmnSerialPort::OPEN) {
				for (int i = 0; i < MAX_HAND_TAG_CNT; i++) {
					g_thrd_1->PostMessage(&g_handler_1, MSG_SEND_DATA_1 + i);
				}
			}
			else {
				g_thrd_1->PostDelayMessage(RECONNECT_TIME, &g_handler_1, MSG_CONNECT_1);
			}
		}
	}
	// 定时发送数据
	else if (dwMessageId >= MSG_SEND_DATA_1 && dwMessageId < MSG_SEND_DATA_1 + 100) {
		int nIndex = dwMessageId - MSG_SEND_DATA_1;
		// 最多发送10个数据
		if ( g_SendCnt[nIndex] >= 1000 ) {
			return;
		}
		g_SendCnt[nIndex]++;

		BYTE   byData[128];
		DWORD  dwDataLen = 32;
		memcpy(byData, "\x55\x1E\x0B\x02\x00\x00\x00\x01\x34\x4C\x8C\x7E\xE3\x59\x02\xE0\x10\x5A\x00\x00\x00\x00\x2E\xF1\x79\xA4\x00\x01\x04\xE0\x00\xFF", dwDataLen);
		memcpy(byData + 8,  g_TagId_1[nIndex], 8);
		memcpy(byData + 22, g_CardId_1[nIndex], 8);
		DWORD dwTemp = GetRand( 3000 + nIndex * 100, 3100 + nIndex * 100 );
		byData[16] = (BYTE)(dwTemp / 100);
		byData[17] = (BYTE)(dwTemp % 100);
		g_com.Write(byData, dwDataLen);
		g_thrd_1->PostDelayMessage(g_HandDelay[nIndex], &g_handler_1, MSG_SEND_DATA_1+nIndex);
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
	// 模拟手持读卡器
	else if (dwIndex == 2) {
		if (0 == g_thrd_1) {
			g_thrd_1 = new LmnToolkits::Thread;
			g_thrd_1->Start();
			g_thrd_1->PostMessage(&g_handler_1, MSG_CONNECT_1);

			memset(g_SendCnt, 0, sizeof(g_SendCnt));
		}
		else {
			printf("模拟手持已经启动！\n");
		}
	}
	else if (dwIndex == 3) {
		if (0 != g_thrd_1) {
			g_thrd_1->Stop();
			delete g_thrd_1;
			g_thrd_1 = 0;
		}
		else {
			printf("还没有启动模拟手持读卡器！\n");
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
	else if (0 == strcmp(szChoice, "5")) {
		if (0 != g_thrd) {
			g_thrd->Stop();
			delete g_thrd;
			g_thrd = 0;
		}

		if (0 != g_thrd_1) {
			g_thrd_1->Stop();
			delete g_thrd_1;
			g_thrd_1 = 0;
		}
		return QUIT_CONSOLE_MENU;
	}
	else if (0 == strcmp(szChoice, "3")) {
		return 2;
	}
	else if (0 == strcmp(szChoice, "4")) {
		return 3;
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

	for (int i = 0; i < MAX_HAND_TAG_CNT; i++) {
		memcpy(g_TagId_1[i], "\x00\x00\x00\x00\x00\x00\x30\xe0", 8);
		g_TagId_1[i][0] = (BYTE)(i + 1);

		memcpy(g_CardId_1[i], "\x00\x00\x00\x00\x00\x00\x55\xe0", 8);
		g_CardId_1[i][0] = (BYTE)(i + 1);
	}

	TConsoleMenuItem  item[5];
	STRNCPY(item[0].szName, "1.开始模拟", sizeof(item[0].szName));
	item[0].hMenu = 0;
	STRNCPY(item[1].szName, "2.停止模拟", sizeof(item[1].szName));
	item[1].hMenu = 0;
	STRNCPY(item[2].szName, "3.开始模拟手持读卡器", sizeof(item[2].szName));
	item[2].hMenu = 0;
	STRNCPY(item[3].szName, "4.停止模拟手持读卡器", sizeof(item[3].szName));
	item[3].hMenu = 0;
	STRNCPY(item[4].szName, "5.退出", sizeof(item[4].szName));
	item[4].hMenu = 0;
	
	InitConsole(SelectChoice, HandleChoice);
	ConsoleMenuHandle h = CreateConsoleMenu("模拟读卡器");
	int ret = AddConsoleMenuItem(h, &item[0]);
	ret = AddConsoleMenuItem(h, &item[1]);
	ret = AddConsoleMenuItem(h, &item[2]);
	ret = AddConsoleMenuItem(h, &item[3]);
	ret = AddConsoleMenuItem(h, &item[4]);
	DisplayConsoleMenu(h);
	DeinitConsole();

	LmnToolkits::ThreadManager::ReleaseInstance();
	return 0;
}