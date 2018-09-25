#include <string>
#include <vector>
#include <time.h>
#include <assert.h>
#include "TelemedReader.h"
#include "LmnString.h"

#define SERIAL_PORT_SLEEP_TIME  5000
#define SERIAL_PORT_READ_MASSIVE_DATA_TIME         600000

#define READER_TAIL             "\x0d\x0a"

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

static int TransferReaderCmd(CTelemedReader::ReaderCmd & cmd, const char * szCmd) {
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




// CTelemedReader类
CTelemedReader::CTelemedReader() {
	TransferReaderCmd(PREPARE_COMMAND, "55 01 01 DD AA");
	TransferReaderCmd(HEARTBEAT_COMMAND, "55 01 01 DD AA");
	
}

CTelemedReader::TELEMED_READER_STATUS CTelemedReader::GetStatus() const {
	return m_eStatus;
}

void  CTelemedReader::Clear() {
	CloseUartPort();
	m_received_data.Clear();
	m_eStatus = STATUS_CLOSE;
}

int   CTelemedReader::RequestPrepare() {
	DWORD dwWrited = PREPARE_COMMAND.dwCommandLength;
	BOOL bRet = Write(PREPARE_COMMAND.abyCommand, dwWrited);
	if (!bRet) {
		return ERROR_FAIL;
	}
	return 0;
}

int   CTelemedReader::ReadPrepareRet() {
	BYTE pData[8192];

	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 20);

	// 如果数据到达20字节
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}

	return ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
}

int  CTelemedReader::RequestHeartbeat() {
	DWORD dwWrited = HEARTBEAT_COMMAND.dwCommandLength;
	BOOL bRet = Write(HEARTBEAT_COMMAND.abyCommand, dwWrited);
	if (!bRet) {		
		return ERROR_FAIL;
	}
	return 0;
}

int  CTelemedReader::ReadHeartbeatRet() {
	BYTE pData[8192];
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 20);

	// 如果数据到达20字节
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}

	return ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
}

void  CTelemedReader::ReceiveAsPossible(DWORD  dwWaitTime, DWORD dwMaxDataLength /*= 0*/) {
	BYTE   buf[8192];
	DWORD  dwLeft = dwMaxDataLength;           // 剩余的字节数
	DWORD  dwWaitedTime = 0;                   // 已经等待的时间
	DWORD  dwReadLen = 0;

	do
	{
		Sleep( 200 );
		dwWaitedTime += 200;

		// 如果对数据的字节数有限制
		if (dwMaxDataLength > 0) {
			if (dwLeft > sizeof(buf)) {
				dwReadLen = sizeof(buf);
			}
			else {
				dwReadLen = dwLeft;
			}
		}
		else {
			dwReadLen = sizeof(buf);
		}
		

		Read(buf, dwReadLen);
		m_received_data.Append(buf, dwReadLen);
		dwLeft -= dwReadLen;

		// 如果等待时间到
		if (dwWaitedTime >= dwWaitTime) {
			break;
		}

		// 如果对字节数目有限制
		if (dwMaxDataLength > 0) {
			// 如果已经读到最大数据(防止死循环)
			if (0 == dwLeft) {
				break;
			}
		}

	} while (TRUE);
}


int CTelemedReader::Reconnect() {

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
		m_received_data.Reform();
		m_eStatus = STATUS_OPEN;
		m_sigStatusChange.emit(m_eStatus);
		return 0;
	}
	else {
		return ERROR_FAIL;
	}

	return ERROR_FAIL;
}

int  CTelemedReader::Heartbeat() {
	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	int ret = RequestHeartbeat();
	if (0 != ret) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ret;
	}

	ret = ReadHeartbeatRet();
	if (0 != ret) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ret;
	}

	m_received_data.Reform();
	return 0;
}

// 设置Id
int  CTelemedReader::SetReaderId(const CReaderIdParam * pParam) {
	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	int nId = pParam->m_nId;
	int nVersion = pParam->m_nVersion;

	// 55 03 01    10 01 00 02    DD AA
	// 55 03 01    03 00 01 40    DD AA

	BYTE buf[8192];
	buf[0] = 0x55;
	buf[1] = 0x03;
	buf[2] = 0x01;

	//buf[3] = 0x10;
	//buf[4] = 0x01;
	buf[3] = (BYTE)nVersion;
	buf[4] = 0x00;
	buf[5] = (nId >> 8) & 0xFF;
	buf[6] = nId & 0xFF;

	buf[7] = 0xDD;
	buf[8] = 0xAA;

	DWORD dwWrited = 9;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAIL;
	}

	return 0;
}

// 设置Time
int  CTelemedReader::SetReaderTime(const CReaderTimeParam * pParam) {
	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	time_t tTime = pParam->m_tTime;

	// 55 04 01    12 04 18 16 1A 28    DD AA
	BYTE buf[8192];
	buf[0] = 0x55;
	buf[1] = 0x04;
	buf[2] = 0x01;
	
	struct tm * ptmTime = localtime(&tTime);
	if (0 == ptmTime) {
		return ERROR_FAIL;
	}

	buf[3] = (BYTE)( (ptmTime->tm_year + 1900) - 2000 );
	buf[4] = (BYTE)( ptmTime->tm_mon + 1 );
	buf[5] = (BYTE)( ptmTime->tm_mday );
	buf[6] = (BYTE)( ptmTime->tm_hour );
	buf[7] = (BYTE)( ptmTime->tm_min );

	buf[8] = 0xDD;
	buf[9] = 0xAA;

	DWORD dwWrited = 10;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAIL;
	}

	BYTE pData[8192];
	int ret = -1;
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 20);

	// 如果数据到达20字节
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			ret = 0;
		}
	}

	if (0 != ret) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}

	m_received_data.Reform();
	return 0;
}

// 设置Mode
int  CTelemedReader::SetReaderMode(const CReaderModeParam * pParam) {
	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	int nMode = pParam->m_nMode;

	// 55 02   01   DD AA
	BYTE buf[8192];
	buf[0] = 0x55;
	buf[1] = 0x02;

	buf[2] = (0 == nMode ? 0x02 : 0x01 );

	buf[3] = 0xDD;
	buf[4] = 0xAA;

	DWORD dwWrited = 5;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAIL;
	}

	return 0;
}

// 清空Reader
int CTelemedReader::ClearReader() {
	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	// 55 01 04 DD AA
	BYTE buf[8192];
	buf[0] = 0x55;
	buf[1] = 0x01;
	buf[2] = 0x04;
	buf[3] = 0xDD;
	buf[4] = 0xAA;

	DWORD dwWrited = 5;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAIL;
	}

	BYTE pData[8192];
	int ret = -1;
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 20);

	// 如果数据到达20字节
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			ret = 0;
		}
	}

	if (0 != ret) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}

	m_received_data.Reform();
	return 0;
}

// 获取Reader数据 
int CTelemedReader::GetReaderData(std::vector<TempItem* > & vRet) {

	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	// 55 01 03 DD AA
	BYTE buf[8192];
	buf[0] = 0x55;
	buf[1] = 0x01;
	buf[2] = 0x03;
	buf[3] = 0xDD;
	buf[4] = 0xAA;

	DWORD dwWrited = 5;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAIL;
	}

	BYTE pData[8192];
	int ret = -1;

	// 先读第一行数据
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 28);

	// 如果数据到达28字节
	if (m_received_data.GetDataLength() >= 28) {
		m_received_data.Read(pData, 28);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 26, READER_TAIL, 2)) {
			ret = 0;
		}
	}

	if (0 != ret) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}

	// 从这一行获取有多少个item
	// 1 + 5 + 8 + 2 + 2 + 8 + 2
	DWORD  dwCnt = pData[16] * 256 + pData[17];
	// 0条记录
	if (0 == dwCnt) {
		return 0;
	}

	TempItem* pItem = new TempItem;
	memset(pItem, 0, sizeof(TempItem));

	FillTempItem(pItem, pData, 28);
	vRet.push_back(pItem);

	// 剩余的数据长度
	DWORD  dwExpectedLength = 28 * (dwCnt - 1);
	if (0 == dwExpectedLength) {
		return 0;
	}

	ReceiveAsPossible(SERIAL_PORT_READ_MASSIVE_DATA_TIME, dwExpectedLength);

	// 如果数据未达到
	if (m_received_data.GetDataLength() < dwExpectedLength) {
		ClearVector(vRet);
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}

	// 一条一条读数据
	while ( m_received_data.Read(pData, 28) ) {
		pItem = new TempItem;
		memset(pItem, 0, sizeof(TempItem));

		FillTempItem(pItem, pData, 28);
		vRet.push_back(pItem);
	}
	
	m_received_data.Reform();
	return 0;
}

void  CTelemedReader::FillTempItem(TempItem * pItem, const BYTE * pData, DWORD dwDataLen) {
	assert(dwDataLen == 28);

	struct tm tmTime;
	memset(&tmTime, 0, sizeof(tm));

	tmTime.tm_year = 2000 + pData[1] - 1900;
	tmTime.tm_mon = pData[2] - 1;
	tmTime.tm_mday = pData[3];
	tmTime.tm_hour = pData[4];
	tmTime.tm_min = pData[5];
	tmTime.tm_sec = 0;

	pItem->tTime = mktime(&tmTime);

	memcpy(pItem->tTagId.abyUid, pData + 6, 8);
	pItem->tTagId.dwLen = 8;

	pItem->dwTemp = pData[14] * 100 + pData[15];

	memcpy( pItem->tCardId.abyUid, pData + 18, 8 );
	pItem->tCardId.dwLen = 8;
}

int CTelemedReader::SetReaderBluetooth(const CReaderBlueToothParam * pParam) {
	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	BOOL bEnable = pParam->m_bEnable;

	//	55 01 07 DD AA   close
	//	55 01 08 DD AA   open

	BYTE buf[8192];
	buf[0] = 0x55;
	buf[1] = 0x01;

	buf[2] = ( bEnable ? 0x08 : 0x07);

	buf[3] = 0xDD;
	buf[4] = 0xAA;

	DWORD dwWrited = 5;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		Clear();
		m_sigStatusChange.emit(m_eStatus);
		return ERROR_FAIL;
	}

	return 0;
}