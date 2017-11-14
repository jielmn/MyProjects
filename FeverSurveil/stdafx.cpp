
// stdafx.cpp : 只包括标准包含文件的源文件
// FeverSurveil.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#define DELAY_RECONNECT_SERIAL_PORT_TIME         10000
#define DELAY_RECEIVE_SERIAL_PORT_DATA_TIME      1000

#define DELAY_RECONNECT_DB_TIME                  10000

IConfig *  g_cfg = 0;
ILog *     g_log = 0;

LmnToolkits::Thread *  g_thrd_db = 0;
LmnToolkits::Thread *  g_thrd_serial_port = 0;
MyMessageHandler *     g_handler = 0;
CSerialPort *          g_serial_port = 0;
CMyDatabase *          g_db = 0;
HWND                   g_hwndMain = 0;
ReaderCmd              SYNC_COMMAND;
ReaderCmd              PREPARE_COMMAND;
ReaderCmd              CLEAR_COMMAND;

MyMessageHandler::MyMessageHandler() {

}

MyMessageHandler::~MyMessageHandler() {

}

void MyMessageHandler::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId) {
	case MSG_RECONNECT_SERIAL_PORT:
	{
		g_serial_port->Deinit();
		int ret = g_serial_port->Init();
		NotifyUiSerialPortStatus( );		
		if (0 == ret) {
			Prepare();
		}
	}
	break;

	case MSG_SYNC:
	{
		g_serial_port->Sync();
		NotifyUiSerialPortStatus();
		ReceiveSerialPortData();
	}
	break;

	case MSG_PREPARE:
	{
		g_serial_port->Prepare();
		NotifyUiSerialPortStatus();
		ReceiveSerialPortData();
	}
	break;

	case MSG_CLEAR_READER:
	{
		g_serial_port->ClearReader();
		NotifyUiSerialPortStatus();
		ReceiveSerialPortData();
	}
	break;

	// 接收串口数据
	case MSG_RECEIVE_SERIAL_PORT_DATA:
	{
		DWORD dwReceivedCnt = 0;
		int ret = g_serial_port->Receive(dwReceivedCnt);
		// 如果接收到数据，继续接收
		if (dwReceivedCnt > 0) {
			ReceiveSerialPortData();
		}
		// 如果没有接收到数据，可能数据接收完成，或者有问题
		else {
			// 接收过程中有问题
			if (0 != ret) {
				NotifyUiSerialPortStatus();
			}
			// 认为接收完成，或超时，开始整理数据
			else {
				ComStatus eSubStatus = g_serial_port->GetSubStatus();
				int ret_format = 0;
				std::vector<TagData *> * pVec = 0;

				// 同步阶段
				if (COM_STATUS_UPLOADING == eSubStatus) {
					pVec = new std::vector<TagData *>;
					ret_format = g_serial_port->FormatData(*pVec);

					// 解析正确
					if (0 == ret_format) {
						NotifyUiSerialPortStatus();
						NotifyUiSyncRet(pVec);
					}
					// 解析错误
					else {
						ClearVector(*pVec);
						delete pVec;
						g_serial_port->Deinit();
						NotifyUiSerialPortStatus();
					}
				}
				// 准备阶段
				else if (COM_STATUS_PREPARING == eSubStatus) {
					ret_format = g_serial_port->FormatData();

					// 解析正确
					if (0 == ret_format) {
						NotifyUiSerialPortStatus();
					}
					// 解析错误
					else {
						g_serial_port->Deinit();
						NotifyUiSerialPortStatus();
					}
				}		
				else if (COM_STATUS_CLEARING == eSubStatus) {
					ret_format = g_serial_port->FormatData();

					// 解析正确
					if (0 == ret_format) {
						NotifyUiSerialPortStatus();
					}
					// 解析错误
					else {
						g_serial_port->Deinit();
						NotifyUiSerialPortStatus();
					}
				}
			}
		}
	}
	break;


	case MSG_RECONNECT_DB:
	{
		g_db->ReconnectDb();
		NotifyUiDbStatus();
	}
	break;

	case MSG_UPDATE_FEVER_DATA:
	{
		int ret = g_db->UpdateFeverData(pMessageData);
		if (0 != ret) {
			g_db->Close();
			NotifyUiDbStatus();
		}
	}
	break;

	default:
		break;
	}
}

int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd) {
	if (0 == szCmd) {
		return -1;
	}

	SplitString split;
	split.SplitByBlankChars(szCmd);
	DWORD dwSize = split.Size();
	if (dwSize >= sizeof(cmd.abyCommand)) {
		return -1;
	}

	for (DWORD i = 0; i < dwSize; i++) {
		int  tmp = 0;
		sscanf_s( split[i], "%x", &tmp );
		cmd.abyCommand[i] = (BYTE)tmp;
	}
	cmd.dwCommandLength = dwSize;

	return 0;
}

void ReconnectDb(BOOL bRightNow /*= FALSE*/) {
	if (bRightNow)
		g_thrd_db->PostMessage(g_handler, MSG_RECONNECT_DB);
	else
		g_thrd_db->PostDelayMessage(DELAY_RECONNECT_DB_TIME, g_handler, MSG_RECONNECT_DB);
}

void ReconnectSerialPort(BOOL bRightNow /*= FALSE*/) {
	if ( bRightNow )
		g_thrd_serial_port->PostMessage(g_handler, MSG_RECONNECT_SERIAL_PORT);
	else 
		g_thrd_serial_port->PostDelayMessage(DELAY_RECONNECT_SERIAL_PORT_TIME, g_handler, MSG_RECONNECT_SERIAL_PORT);
}

void Sync() {
	g_thrd_serial_port->PostMessage(g_handler, MSG_SYNC);
}

void Prepare() {
	g_thrd_serial_port->PostMessage(g_handler, MSG_PREPARE);
}

void ClearReader() {
	g_thrd_serial_port->PostMessage(g_handler, MSG_CLEAR_READER);
}

void NotifyUiSerialPortStatus() {
	ComStatus eMainStatus = g_serial_port->GetMainStatus();
	ComStatus eSubStatus = g_serial_port->GetSubStatus();
	BOOL      bPrepared = g_serial_port->IfPrepared();

	DWORD  dwParam = (eMainStatus << 16) | bPrepared;

	::PostMessage(g_hwndMain, UM_SERIAL_PORT_STATE, (WPARAM)dwParam, (LPARAM)eSubStatus);
}

void NotifyUiDbStatus() {
	DbStatus  eStatus = g_db->GetStatus();
	::PostMessage(g_hwndMain, UM_DB_STATE, (WPARAM)eStatus, 0);
}

void ReceiveSerialPortData() {
	g_thrd_serial_port->PostDelayMessage(DELAY_RECEIVE_SERIAL_PORT_DATA_TIME, g_handler, MSG_RECEIVE_SERIAL_PORT_DATA);
}

BOOL GetBoolean(const char * szStr) {
	if (0 == szStr) {
		return FALSE;
	}
	if (0 == StrICmp(szStr, "true")) {
		return TRUE;
	}
	return FALSE;
}

void NotifyUiSyncRet(std::vector<TagData *> * pVec) {
	::PostMessage(g_hwndMain, UM_SYNC_RESULT, (WPARAM)pVec, 0);
}

void UpdateFeverData(DWORD dwHospitalIndex, DWORD dwFeverCnt) {
	if (dwFeverCnt == 0) {
		return;
	}
	g_thrd_db->PostMessage(g_handler, MSG_UPDATE_FEVER_DATA, new CUpdateFeverData(dwHospitalIndex, dwFeverCnt));
}

char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit /*= '\0'*/) {
	if (0 == buf || 0 == dwBufLen) {
		return 0;
	}

	if ((WORD)dwBufLen < uidlen * 3) {
		return 0;
	}

	if (0 == uidlen) {
		buf[0] = '\0';
		return buf;
	}

	if (chSplit != '\0') {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf_s(buf + i * 3, dwBufLen-i*3, "%02x", uid[i]);
			if (i < uidlen - 1) {
				buf[i * 3 + 2] = chSplit;
			}
		}
		buf[uidlen * 3] = '\0';
	}
	else {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf_s(buf + i * 2, dwBufLen-i*2, "%02x", uid[i]);
		}
		buf[uidlen * 2] = '\0';
	}


	return buf;
}