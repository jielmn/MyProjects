
// stdafx.cpp : 只包括标准包含文件的源文件
// TagsInventory.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "LmnString.h"

IConfig *           g_cfg = 0;
IConfig *           g_cfg_change = 0;
ILog *              g_log = 0;
CMyDatabase *       g_mydb = 0;
CMyReader   *       g_myreader = 0;
LmnToolkits::Thread *  g_thrd_db = 0;
LmnToolkits::Thread *  g_thrd_reader = 0;
MyMessageHandlerDb *     g_handler_db = 0;
MyMessageHandlerReader * g_handler_reader = 0;
HWND                     g_hDlg = 0;
LoginUser                g_login_user;
BOOL                     g_bLogVerifyID = FALSE;
HWND					 g_hQueryDlg = 0;

bool operator == (const TagItem & t1, const TagItem & t2) {
	if ( t1.dwUidLen == t2.dwUidLen && 0 == memcmp(t1.abyUid, t2.abyUid, t1.dwUidLen)) {
		return true;
	}

	return false;
}

char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit /*= '\0'*/ ) {
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
			sprintf(buf + i * 3, "%02x", uid[i]);
			if (i < uidlen - 1) {
				buf[i * 3 + 2] = chSplit;
			}
		}
		buf[uidlen * 3] = '\0';
	}
	else {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf(buf + i * 2, "%02x", uid[i]);
		}
		buf[uidlen * 2] = '\0';
	}
	

	return buf;
}

CTagItem::CTagItem(const TagItem * pItem) {
	if (0 != pItem) {
		memcpy(&m_item, pItem, sizeof(TagItem));
	}
	else {
		memset(&m_item, 0, sizeof(TagItem));
	}
}

CTagItem::~CTagItem( ) {

}

CSaveInvParam::CSaveInvParam() {

}

CSaveInvParam::~CSaveInvParam() {
	ClearVector(m_items);
}


MyMessageHandlerDb::MyMessageHandlerDb() {

}

MyMessageHandlerDb::~MyMessageHandlerDb() {
	
}

void MyMessageHandlerDb::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId) {
	case MSG_RECONNECT_DB:
		g_mydb->ReconnectDb();
		break;
	case MSG_VERIFY_USER:
		{
			CTagItem * pItem = (CTagItem *)pMessageData;
			g_mydb->VerifyUser(&pItem->m_item);
		}
		break;
	case MSG_SAVE_INVENTORY:
		{
			CSaveInvParam * pParam = (CSaveInvParam *)pMessageData;
			g_mydb->SaveInventory(pParam);
		}
		break;
	case MSG_QUERY:
		{
			g_mydb->Query(pMessageData);
		}
		break;
	default:
		break;
	}
}



MyMessageHandlerReader::MyMessageHandlerReader() {

}

MyMessageHandlerReader::~MyMessageHandlerReader() {

}

void MyMessageHandlerReader::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId) {
	case MSG_RECONNECT_READER:
		g_myreader->Open();
		break;
	case MSG_READER_INVENTORY:
		g_myreader->Inventory();
		break;
	default:
		break;
	}
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


err_t rfid_inventory(RFID_READER_HANDLE hreader,
					BOOLEAN newAI,
					BYTE Antennas[],
					BYTE AntCount,
					BOOLEAN enISO15693,
					BOOLEAN enAFI,
					BYTE afi,
					BOOLEAN enISO14443A,
					BOOLEAN enISO18000p3m3,
					CB_TAG_REPORT_HANDLE tag_report_handler,
					void *cbParam,
					DWORD *tagCount)
{
	err_t iret = 0;
	DWORD totalCount;
	RFID_DN_HANDLE dnInvenParamList = RFID_HANDLE_NULL;
	/*
	*  Add air interface protocol
	*/
	dnInvenParamList = RDR_CreateInvenParamSpecList();
	if (dnInvenParamList) {
		if (enISO15693) {// enable iso15693 inventory
			ISO15693_CreateInvenParam(dnInvenParamList, 0, enAFI, afi, 0x00);
		}
		if (enISO14443A) { // enable iso14443a inentory
			ISO14443A_CreateInvenParam(dnInvenParamList, 0);
		}
		if (enISO18000p3m3) {// enable iso18000p3m3 inventory
			ISO18000p3m3_CreateInvenParam(dnInvenParamList, 0, 0, 0, ISO18000p6C_Dynamic_Q);
		}
	}
	totalCount = 0;

	//if AntCount =0  ,use default
label_inventory:
	iret = RDR_TagInventory(hreader, newAI, AntCount, Antennas, dnInvenParamList);
	if ((iret == NO_ERR) || (iret == -21)) {
		//ClearVector(g_tag_items);
		totalCount += RDR_GetTagDataReportCount(hreader);
		RFID_DN_HANDLE dnhReport = RDR_GetTagDataReport(hreader, RFID_SEEK_FIRST);
		while (dnhReport != RFID_HANDLE_NULL)
		{
			DWORD AIPtype, TagType, AntId;
			BYTE dsfid;
			BYTE uid[32];
			BYTE uidlen;
			/* Is iso15693 tag ? if parse ok ,process it */
			iret = ISO15693_ParseTagDataReport(dnhReport, &AIPtype, &TagType, &AntId, &dsfid, uid);
			if (iret == NO_ERR)
			{
				tag_report_handler(cbParam, hreader, AIPtype, TagType, AntId, uid, 8);
			}
			/* Is iso14443A tag ? if parse ok ,process it */
			iret = ISO14443A_ParseTagDataReport(dnhReport, &AIPtype, &TagType, &AntId, uid, &uidlen);
			if (iret == NO_ERR) {
				tag_report_handler(cbParam, hreader, AIPtype, TagType, AntId, uid, uidlen);
			}
			/* Is iso18000-3 mode 3 tag ? if parse ok ,process it */
			BYTE tagData[64];
			DWORD tagDataLen = sizeof(tagData);
			DWORD metaFlags = 0;
			iret = ISO18000p3m3_ParseTagDataReport(dnhReport, &AIPtype, &TagType, &AntId, &metaFlags, tagData, &tagDataLen);
			if (iret == NO_ERR) {
				tag_report_handler(cbParam, hreader, AIPtype, TagType, AntId, tagData, (WORD)tagDataLen);
			}

			dnhReport = RDR_GetTagDataReport(hreader, RFID_SEEK_NEXT);
		}
		if (iret == -21)
		{
			/*Stop trigger ocur at the reader .Maybe there are tags left for reading. */
			newAI = AI_TYPE_CONTINUE;
			goto label_inventory;
		}

		iret = 0;
	}
	else {
		/* error occur */
	}
	if (dnInvenParamList) DNODE_Destroy(dnInvenParamList);
	if (tagCount) *tagCount = totalCount;

#ifdef MY_DEBUG
	
#endif	

	return iret;
}

char *  GetNum(char * buf, DWORD dwBufSize, DWORD dwNum) {
	_snprintf(buf, dwBufSize, "%lu", dwNum);
	return buf;
}

char *  GetOperateType(char * buf, DWORD dwBufSize, DWORD dwOperateType) {
	if (dwOperateType == 100) {
		strncpy(buf, "包装盘点", dwBufSize );
		return buf;
	}
	strncpy(buf, "", dwBufSize);
	return buf;
}