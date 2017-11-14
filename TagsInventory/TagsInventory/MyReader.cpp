#include "stdafx.h"
#include "MyReader.h"

#define  RECONNECT_READER_TIME               10000

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
	char buf[8192];
	err_t iret = NO_ERR;

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