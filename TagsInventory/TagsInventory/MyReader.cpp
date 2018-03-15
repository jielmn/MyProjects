#include "stdafx.h"
#include "MyReader.h"

#define  RECONNECT_READER_TIME               10000

static BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = NULL;
	vCom.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	CString valuename, databuffer;
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename.GetBuffer(200), &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer.GetBuffer(200), &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		std::string com_name = databuffer;
		vCom.push_back(com_name);

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);
	return TRUE;
}


void MY_TAG_REPORT(void *cbParam, RFID_READER_HANDLE hreader, DWORD AIPtype, DWORD TagType, DWORD AntId, BYTE uid[], WORD uidlen) {
	TagItem * pItem = new TagItem;
	if (0 == pItem) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "no memory!");
		return;
	}

	if (uidlen > sizeof(pItem->abyUid)) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "too long uid!");
		delete pItem;
		return;
	}

	memcpy(pItem->abyUid, uid, uidlen);
	pItem->dwUidLen = uidlen;

	::PostMessage(g_hDlg, UM_TAG_ITEM, (WPARAM)pItem, 0);
}

CMyReader::CMyReader() {
	m_nStatus = -1;

	char buf[8192];
	g_cfg->GetConfig("newAI", buf, sizeof(buf));
	m_bNewAI = GetBoolean(buf);
	g_cfg->GetConfig("enISO15693", buf, sizeof(buf));
	m_enISO15693 = GetBoolean(buf);
	g_cfg->GetConfig("enAFI", buf, sizeof(buf));
	m_enAFI = GetBoolean(buf);
	g_cfg->GetConfig("enISO14443A", buf, sizeof(buf));
	m_enISO14443a = GetBoolean(buf);
	g_cfg->GetConfig("enISO18000p3m3", buf, sizeof(buf));
	m_enISO18000p3m3 = GetBoolean(buf);
}

CMyReader::~CMyReader() {

}

void CMyReader::Open() {
	//char buf[8192];
	err_t iret = NO_ERR;

	std::vector<std::string>  vCom;
	GetAllSerialPortName(vCom);

	const std::string PARAMS = "RDType=M201;CommType=COM;BaudRate=38400;Frame=8E1;BusAddr=255;COMName=";

	m_nStatus = STATUS_CLOSE;
	std::vector<std::string>::iterator it;
	for ( it = vCom.begin(); it != vCom.end(); it++ ) {
		std::string  sItem = *it;
		std::string  sFullParam = PARAMS+sItem;

		iret = RDR_Open(sFullParam.c_str(), &m_hr_reader);
		//iret = RDR_Open("RDType=M201;CommType=COM;COMName=COM3;BaudRate=38400;Frame=8E1;BusAddr=255", &m_hr_reader);
		// 如果OK
		if (NO_ERR == iret) {
			m_nStatus = STATUS_OPEN;
			// 盘点
			g_thrd_reader->PostMessage(g_handler_reader, MSG_READER_INVENTORY);
			break;
		}
	}

	if (m_nStatus == STATUS_CLOSE) {
		// 重连
		g_thrd_reader->PostDelayMessage(RECONNECT_READER_TIME, g_handler_reader, MSG_RECONNECT_READER);
	}

	// 通知界面
	::PostMessage(g_hDlg, UM_SHOW_STATUS, STATUS_TYPE_READER, m_nStatus);


	/*
	g_cfg->GetConfig("reader string", buf, sizeof(buf));
	iret = RDR_Open(buf, &m_hr_reader);
	// 如果OK
	if (NO_ERR == iret) {
		m_nStatus = STATUS_OPEN;
		// 盘点
		g_thrd_reader->PostMessage(g_handler_reader, MSG_READER_INVENTORY);
	}
	// 如果有问题
	else {
		m_nStatus = STATUS_CLOSE;
		// 重连
		g_thrd_reader->PostDelayMessage(RECONNECT_READER_TIME, g_handler_reader, MSG_RECONNECT_READER);
	}
	// 通知界面
	::PostMessage(g_hDlg, UM_SHOW_STATUS, STATUS_TYPE_READER, m_nStatus);
	*/
}

void CMyReader::Inventory() {
	err_t iret;
	DWORD totalCount = 0;
	iret = rfid_inventory(  m_hr_reader, m_bNewAI, 0, 0, m_enISO15693,
							m_enAFI, (BYTE)m_afi,
							m_enISO14443a,
							m_enISO18000p3m3, MY_TAG_REPORT, 0, &totalCount);
	// 如果OK
	if (iret == NO_ERR) {
		// 再来一次盘点
		g_thrd_reader->PostMessage(g_handler_reader, MSG_READER_INVENTORY);
	}
	// 如果有问题
	else {
		// 重连
		g_thrd_reader->PostDelayMessage(RECONNECT_READER_TIME, g_handler_reader, MSG_RECONNECT_READER);
		// 通知界面
		::PostMessage(g_hDlg, UM_SHOW_STATUS, STATUS_TYPE_READER, m_nStatus);
	}
	
}

int  CMyReader::GetStatus() const {
	return m_nStatus;
}