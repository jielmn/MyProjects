#include <afx.h>
#include <vector>
#include <string>
#include "LmnCommon.h"
#include "LmnString.h"
#include "TelemedInvReader.h"


#define  RECONNECT_TIME                         10000

#define  MAX_READER_COMMAND_LENGTH              256
// Reader通信协议命令
typedef struct tagReaderCmd {
	BYTE      abyCommand[MAX_READER_COMMAND_LENGTH];
	DWORD     dwCommandLength;
}ReaderCmd;

static ReaderCmd  PREPARE_COMMAND;
static ReaderCmd  INVENTORY_COMMAND;

static BOOL GetAllSerialPortName(std::vector<std::string> & vCom);
static int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd);



CTelemedInvReader::CTelemedInvReader() {
	m_eStatus = CTelemedInvReader::STATUS_CLOSE;
	TransferReaderCmd(PREPARE_COMMAND, "55 01 02 DD AA");
	TransferReaderCmd(INVENTORY_COMMAND, "55 01 02 DD AA");

}

CTelemedInvReader::~CTelemedInvReader() {
	Clear();
}

CTelemedInvReader::READER_STATUS CTelemedInvReader::GetStatus() const {
	return m_eStatus;
}

void  CTelemedInvReader::Clear() {
	CloseUartPort();
	m_received_data.Clear();
	m_eStatus = CTelemedInvReader::STATUS_CLOSE;
}

void  CTelemedInvReader::ReceiveAsPossible(DWORD  dwIntervalTime, DWORD dwMaxDataLength) {
	BYTE   buf[8192];
	DWORD  dwLeft = dwMaxDataLength;
	DWORD  dwReadLen = 0;

	do
	{
		Sleep(dwIntervalTime);
		if (dwLeft > sizeof(buf)) {
			dwReadLen = sizeof(buf);
		}
		else {
			dwReadLen = dwLeft;
		}

		Read(buf, dwReadLen);
		m_received_data.Append(buf, dwReadLen);
		dwLeft -= dwReadLen;

		// 如果再无数据了
		if (0 == dwReadLen) {
			break;
		}

		// 如果已经读到最大数据(防止死循环)
		if (0 == dwLeft) {
			break;
		}

	} while (TRUE);
}

// 重连
int  CTelemedInvReader::Reconnect() {

	// 如果已经连接上
	if (m_eStatus == STATUS_OPEN) {
		return 0;
	}

	std::vector<std::string>  vCom;
	// 获取所有的串口信息
	GetAllSerialPortName(vCom);

	// 先关闭之前可能打开的串口
	Clear();


	BOOL  bPrepared = FALSE;
	// 尝试打开从系统获得的串口列表
	do
	{
		std::vector<std::string>::iterator it;
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string  sItem = *it;

			BOOL bRet = OpenUartPort(sItem.c_str(), 9600);
			if (!bRet) {
				Clear();
				continue;
			}

			RequestPrepare();
			int ret = ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
				break;
			}
			Clear();
		}
	} while (0);

	if (bPrepared) {
		m_eStatus = STATUS_OPEN;
		m_sigStatusChange.emit(m_eStatus);
		m_sigInventoryAsyn.emit();
		return 0;
	}
	else {
		m_sigReconnect.emit(RECONNECT_TIME);
		return ERROR_FAIL;
	}

	return ERROR_FAIL;
}


// 盘点
int  CTelemedInvReader::Inventory() {

	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	int ret = 0;
	do
	{
		ret = RequestInventory();
		if (0 != ret) {
			break;
		}

		ret = ReadInventoryRet();
		if (0 != ret) {
			break;
		}

	} while (0);

	if (m_eStatus == STATUS_CLOSE) {
		m_sigStatusChange.emit(m_eStatus);
		m_sigReconnect.emit(RECONNECT_TIME);
	}

	if (0 == ret) {
		m_sigInventoryAsyn.emit();
	}

	return 0;
}






// 协议部分
int   CTelemedInvReader::ReadToken(TokenTelemed & t) {
	if (m_received_data.GetDataLength() < 8) {
		return -1;
	}

	m_received_data.Read(t.abyUid, 8);

	m_received_data.Reform();

	return 0;
}

// prepare
int   CTelemedInvReader::RequestPrepare() {
	DWORD dwWrited = PREPARE_COMMAND.dwCommandLength;
	BOOL bRet = Write(PREPARE_COMMAND.abyCommand, dwWrited);
	if (!bRet) {
		return -1;
	}
	return 0;
}

int   CTelemedInvReader::ReadPrepareRet() {

	DWORD dwLastTick = LmnGetTickCount();
	DWORD dwLeftTick = SERIAL_PORT_SLEEP_TIME;

	do
	{
		ReceiveAsPossible(200, 8);
		DWORD dwCurTick = LmnGetTickCount();
		DWORD dwElapsed = dwCurTick - dwLastTick;
		dwLastTick = dwCurTick;

		if (dwLeftTick > dwElapsed) {
			dwLeftTick -= dwElapsed;
		}
		else {
			dwLeftTick = 0;
		}

		TokenTelemed t;
		int ret = ReadToken(t);
		if (0 == ret) {
			if (0 == memcmp(t.abyUid, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8)) {
				return 0;
			}
		}

		// 时间耗尽，还没有读到结束符号
		if (0 == dwLeftTick) {
			break;
		}
	} while (TRUE);

	return -1;
}

int   CTelemedInvReader::RequestInventory() {

	DWORD dwWrited = INVENTORY_COMMAND.dwCommandLength;
	BOOL bRet = Write(INVENTORY_COMMAND.abyCommand, dwWrited);
	if (!bRet) {
		m_eStatus = STATUS_CLOSE;
		return -1;
	}
	return 0;
}

int   CTelemedInvReader::ReadInventoryRet() {

	DWORD dwLastTick = LmnGetTickCount();
	DWORD dwLeftTick = SERIAL_PORT_SLEEP_TIME;

	do 
	{
		ReceiveAsPossible( 200, 8 );
		DWORD dwCurTick = LmnGetTickCount();
		DWORD dwElapsed = dwCurTick - dwLastTick;
		dwLastTick = dwCurTick;

		if (dwLeftTick > dwElapsed) {
			dwLeftTick -= dwElapsed;
		}
		else {
			dwLeftTick = 0;
		}

		TokenTelemed t;
		int ret = ReadToken(t);
		if (0 == ret) {
			if (0 == memcmp(t.abyUid, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8)) {
				return 0;
			}

			BYTE  byUid[8];
			for (int i = 0; i < 8; i++) {
				byUid[i] = t.abyUid[7 - i];
			}
			m_sigInvTagIetm(byUid, 8);
		}

		// 时间耗尽，还没有读到结束符号
		if (0 == dwLeftTick) {
			break;
		}
	} while ( TRUE );
	
	// 格式不对
	m_eStatus = STATUS_CLOSE;
	return -1;
}







// 遍历系统里的所有串口
static BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = NULL;
	vCom.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	char valuename[200], databuffer[200];
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename, &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer, &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		std::string com_name = databuffer;
		vCom.push_back(com_name);

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);
	return TRUE;
}

// 把字符串转为字节
static int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd) {
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
		sscanf_s(split[i], "%x", &tmp);
		cmd.abyCommand[i] = (BYTE)tmp;
	}
	cmd.dwCommandLength = dwSize;

	return 0;
}