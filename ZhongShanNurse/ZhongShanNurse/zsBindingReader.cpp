#include <assert.h>
#include "zsBindingReader.h"
#include "mi.h"

#define  BINDING_READER_TYPE_NEW

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


CZsBindingReader::CZsBindingReader(CBusiness * pBusiness) : m_pBusiness(pBusiness) {
	m_eStatus = STATUS_CLOSE;
}

CZsBindingReader::~CZsBindingReader() {
	Clear();
}

void CZsBindingReader::Clear() {
#ifdef BINDING_READER_TYPE_NEW
	CloseUartPort();
	m_received_data.Clear();
	m_eStatus = STATUS_CLOSE;
#endif
}

CZsBindingReader::BINDING_STATUS CZsBindingReader::GetStatus() {
	return m_eStatus;
}

int CZsBindingReader::Init() {
#ifndef BINDING_READER_TYPE_NEW
	char buf[1024];
	GetCurrentDirectory(1024, buf);

	CString str;
	str.Format("%s", buf);
	HINSTANCE hInstMaster = LoadLibrary(".\\function.dll");
	if (!hInstMaster)
	{
		return ZS_ERR_FAILED_TO_LOAD_FUNCTION_DLL;
	}

	//	(FARPROC &)API_OpenComm   = GetProcAddress(m_hInstMaster, _T("API_OpenComm"));
	//	(FARPROC &)API_CloseComm  = GetProcAddress(m_hInstMaster, _T("API_CloseComm"));
	(FARPROC &)MF_Read = GetProcAddress(hInstMaster, _T("MF_Read"));
	(FARPROC &)MF_Write = GetProcAddress(hInstMaster, _T("MF_Write"));
	(FARPROC &)MF_InitValue = GetProcAddress(hInstMaster, _T("MF_InitValue"));
	(FARPROC &)MF_Dec = GetProcAddress(hInstMaster, _T("MF_Dec"));
	(FARPROC &)MF_Inc = GetProcAddress(hInstMaster, _T("MF_Inc"));
	//	(FARPROC &)RDM_GetSnr     = GetProcAddress(m_hInstMaster, _T("RDM_GetSnr"));
	//	(FARPROC &)MF_Request     = GetProcAddress(m_hInstMaster, _T("MF_Request"));
	//	(FARPROC &)MF_Anticoll    = GetProcAddress(m_hInstMaster, _T("MF_Anticoll"));
	//	(FARPROC &)MF_Select      = GetProcAddress(m_hInstMaster, _T("MF_Select"));
	(FARPROC &)MF_Halt = GetProcAddress(hInstMaster, _T("MF_Halt"));

	//	(FARPROC &)MF_Restore           = GetProcAddress(m_hInstMaster, _T("MF_Restore"));
	(FARPROC &)ControlLED = GetProcAddress(hInstMaster, _T("ControlLED"));
	(FARPROC &)ControlBuzzer = GetProcAddress(hInstMaster, _T("ControlBuzzer"));
	(FARPROC &)MF_Getsnr = GetProcAddress(hInstMaster, _T("MF_Getsnr"));
	//	(FARPROC &)GetVersionNum        = GetProcAddress(m_hInstMaster, _T("GetVersionNum"));
	//	(FARPROC &)API_SetDeviceAddress = GetProcAddress(m_hInstMaster, _T("API_SetDeviceAddress"));
	(FARPROC &)SetSerNum = GetProcAddress(hInstMaster, _T("SetSerNum"));
	(FARPROC &)GetSerNum = GetProcAddress(hInstMaster, _T("GetSerNum"));

	(FARPROC &)TypeB_Request = GetProcAddress(hInstMaster, _T("TypeB_Request"));
	(FARPROC &)TYPEB_SFZSNR = GetProcAddress(hInstMaster, _T("TYPEB_SFZSNR"));

	(FARPROC &)ISO15693_Inventory = GetProcAddress(hInstMaster, _T("ISO15693_Inventory"));
	(FARPROC &)ISO15693_Read = GetProcAddress(hInstMaster, _T("ISO15693_Read"));
	(FARPROC &)ISO15693_Write = GetProcAddress(hInstMaster, _T("ISO15693_Write"));

	(FARPROC &)UL_Request = GetProcAddress(hInstMaster, _T("UL_Request"));
	(FARPROC &)UL_HLRead = GetProcAddress(hInstMaster, _T("UL_HLRead"));
	(FARPROC &)UL_HLWrite = GetProcAddress(hInstMaster, _T("UL_HLWrite"));

	if (MF_Read == NULL ||
		MF_Write == NULL ||
		MF_InitValue == NULL ||
		MF_Dec == NULL ||
		MF_Inc == NULL ||
		MF_Getsnr == NULL ||
		SetSerNum == NULL ||
		GetSerNum == NULL ||
		ControlLED == NULL ||
		ControlBuzzer == NULL ||
		TypeB_Request == NULL ||
		TYPEB_SFZSNR == NULL ||
		ISO15693_Inventory == NULL ||
		ISO15693_Read == NULL ||
		ISO15693_Write == NULL ||
		UL_Request == NULL ||
		MF_Halt == NULL ||
		UL_HLRead == NULL ||
		UL_HLWrite == NULL)
	{
		return ZS_ERR_FAILED_TO_LOAD_FUNCTION_DLL;
	}
#endif
	return 0;
}

int CZsBindingReader::Inventory(TagItem * pId) {
	int   ret = 0;

#ifndef BINDING_READER_TYPE_NEW
	BYTE  cardnumber[100];
	BYTE  pBuffer[100];

	ret = ISO15693_Inventory(cardnumber, pBuffer);
	if (1 == ret) {
		m_eStatus = STATUS_OPEN;
		ret = ZS_ERR_NOT_FOUND_TAG;
	}
	else if (2 == ret) {
		m_eStatus = STATUS_CLOSE;
		ret = ZS_ERR_BINDING_READER_FAILED_TO_INVENTORY;
	}
	else if (0 != ret) {
		m_eStatus = STATUS_CLOSE;
		ret = -1;
	}
	else {
		int card = cardnumber[0];

		if (card >= 2) {
			m_eStatus = STATUS_OPEN;
			return ZS_ERR_BINDING_READER_FAILED_TOO_MANY_CARDS;
		}

		int nSize = 8;
		pId->dwUidLen = nSize;

		for (int i = 0; i < nSize; i++) {
			pId->abyUid[i] = *(pBuffer + 2 + (nSize - 1 - i));
		}

		m_eStatus = STATUS_OPEN;
		ret = 0;
	}	
#else

	// 如果串口关闭，尝试连接串口
	if (m_eStatus == STATUS_CLOSE) {
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
				ret = ReadPrepareRet();
				if (0 != ret) {
					Clear();
					continue;
				}
				
				RequestBeep();
				ret = ReadBeepRet();
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
		}
	}

	if (m_eStatus == STATUS_CLOSE) {
		return -1;
	}

	// 如果串口连接成功，尝试盘点
	ret = RequestInv();
	if (0 != ret) {
		m_eStatus = STATUS_CLOSE;
		return -1;
	}

	ret = ReadInvRet(pId);
	if (0 != ret) {
		m_eStatus = STATUS_CLOSE;
		return -1;
	}

	if (pId->dwUidLen == 0) {
		ret = ZS_ERR_NOT_FOUND_TAG;
	}
	else {
		ret = 0;
	}



#endif
	return ret;
}


int   CZsBindingReader::RequestPrepare() {
	BYTE buf[8192];
	memcpy(buf, "\x55\x55\x00\x00\x00\x03\x01\x03\x01", 9);

	DWORD dwWrited = 9;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		return -1;
	}
	return 0;
}

int   CZsBindingReader::ReadPrepareRet() {
	BYTE pData[8192];

	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 8);

	// 如果数据到达8字节
	if (m_received_data.GetDataLength() >= 8 ) {
		m_received_data.Read(pData, 8);
		if (0 == memcmp(pData, "\x55\x55\x00\x00\x00\x02\x00\x02", 8) ) {
			return 0;
		}
	}

	return -1;
}

int   CZsBindingReader::RequestBeep() {
	BYTE buf[8192];
	memcpy(buf, "\x55\x55\x00\x00\x00\x03\x01\x05\x07", 9);

	DWORD dwWrited = 9;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		return -1;
	}
	return 0;
}

int   CZsBindingReader::ReadBeepRet() {
	BYTE pData[8192];

	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 8);

	// 如果数据到达8字节
	if (m_received_data.GetDataLength() >= 8) {
		m_received_data.Read(pData, 8);
		if (0 == memcmp(pData, "\x55\x55\x00\x00\x00\x02\x00\x02", 8)) {
			return 0;
		}
	}

	return -1;
}

int   CZsBindingReader::RequestInv() {
	BYTE buf[8192];
	memcpy(buf, "\x55\x55\x00\x00\x00\x03\x01\x09\x0B", 9);

	DWORD dwWrited = 9;
	BOOL bRet = Write(buf, dwWrited);
	if (!bRet) {
		return -1;
	}
	return 0;
}

int   CZsBindingReader::ReadInvRet(TagItem * pId) {
	assert(pId);

	BYTE pData[8192];

	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 8);

	if (m_received_data.GetDataLength() < 8) {
		return -1;
	}

	m_received_data.Read(pData, 8);

	// 如果没有卡
	if (0 == memcmp(pData, "\x55\x55\x00\x00\x00\x02\x01\x03", 8)) {
		pId->dwUidLen = 0;
		return 0;
	}

	// 如果长度不对
	if ( pData[5] != 0x0B ) {
		return -1;
	}

	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 9);
	if (m_received_data.GetDataLength() < 9) {
		return -1;
	}

	m_received_data.Read(pData, 9);
	pId->dwUidLen = 8;
	for (int i = 0; i < 8; i++) {
		pId->abyUid[i] = pData[7-i];
	}

	return 0;
}

void  CZsBindingReader::ReceiveAsPossible(DWORD  dwWaitTime, DWORD dwMaxDataLength /*= 0*/) {
	BYTE   buf[8192];
	DWORD  dwLeft = dwMaxDataLength;           // 剩余的字节数
	DWORD  dwWaitedTime = 0;                   // 已经等待的时间
	DWORD  dwReadLen = 0;

	do
	{
		Sleep(200);
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