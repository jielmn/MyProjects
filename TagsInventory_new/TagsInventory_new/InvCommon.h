#pragma once

#include <vector>
#include "sigslot.h"
#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"

#include "inc/rfidlib.h"
#include "inc/rfidlib_reader.h"
#include "inc/rfidlib_AIP_ISO15693.h"
#include "inc/rfidlib_aip_iso14443A.h"
#include "inc/rfidlib_aip_iso18000p3m3.h"

extern ILog    * g_log;
extern IConfig * g_cfg;
extern HWND    g_hWnd;
extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_reader;
extern LmnToolkits::Thread *  g_thrd_timer;
extern DWORD SERIAL_PORT_SLEEP_TIME;

// thread message
#define MSG_RECONNECT_DB           1
#define MSG_RECONNECT_READER       2
#define MSG_READER_INVENTORY       3
#define MSG_USER_LOGIN             4
#define MSG_INV_SMALL_SAVE         5
#define MSG_CHECK_TAG              6
#define MSG_INV_BIG_SAVE           7
#define MSG_QUERY                  8
#define MSG_QUERY_SMALL            9
#define MSG_QUERY_BIG              10

// db reconnect ʱ��
#define  RECONNECT_DB_TIME         10000
#define  RECONNECT_READER_TIME     10000

// ������
#define INV_ERR_NO_MEMORY               10001
#define INV_ERR_DB_CLOSE                10002
#define INV_ERR_DB_ERROR                10003
#define INV_ERR_DB_NOT_UNIQUE           10004
#define INV_ERR_DB_SMALL_PKG_IN_USE     10005
#define INV_ERR_DB_SMALL_PKG_NOT_FOUND  10006
#define INV_ERR_DB_TOO_LARGE_FLOW_NUM   10007

// windows �Զ�����Ϣ
#define UM_SHOW_DB_STATUS          (WM_USER+1)
#define UM_SHOW_READER_STATUS      (WM_USER+2)
#define UM_INVENTORY_RESULT        (WM_USER+3)
#define UM_LOGIN_RESULT            (WM_USER+4)
#define UM_INV_SMALL_SAVE_RESULT   (WM_USER+5)
#define UM_CHECK_TAG_RESULT        (WM_USER+6)
#define UM_TIMER                   (WM_USER+7)
#define UM_INV_BIG_SAVE_RESULT     (WM_USER+8)
#define UM_QUERY_RESULT            (WM_USER+9)
#define UM_QUERY_SMALL_RESULT      (WM_USER+10)
#define UM_QUERY_BIG_RESULT        (WM_USER+11)


// config�����ַ�������
#define ODBC_TYPE                  "odbc type"
#define ODBC_STRING_ORACLE         "odbc string oracle"
#define ODBC_STRING_MYSQL          "odbc string mysql"
#define FACTORY_CODE               "factory code"
#define PRODUCT_CODE               "product code"
#define LAST_BATCH_ID              "last batch id"

#ifdef _DEBUG
#define LOGIN_CARD_ID              "login card"
#endif


char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
int    MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);


#define  DB_STATUS_OK_TEXT             "���ݿ�����OK"
#define  DB_STATUS_CLOSE_TEXT          "���ݿ�����ʧ��"
#define  READER_STATUS_OK_TEXT         "����������OK"
#define  READER_STATUS_CLOSE_TEXT      "����������ʧ��"
#define  MSG_BOX_NEW_INVENTORY_SMALL   "�̵�û�б��棬Ҫ��ʼ�µ��̵���"
#define  CAPTION_NEW_INVENTORY_SMALL   "�̵�"
#define  MSG_BOX_DB_CLOSE              "���ݿ�û��������"
#define  CAPTION_SAVE_INVENTORY_SMALL  "����"
#define  MSG_BOX_BATCH_ID_EMPTY        "���Ų���Ϊ��"
#define  MSG_BOX_WRONG_BATCH_ID_FORMAT "���Ÿ�ʽ����!��������+��+���룬����\"20170906\""
#define  INV_SMALL_SAVE_RET_OK         "�̵㱣��ɹ�"
#define  INV_SMALL_SAVE_RET_ERROR      "�̵㱣��ʧ��"
#define  INV_SMALL_SAVE_RET_SAME_TAG   "�̵㱣��ʧ�ܣ����ݿ����Ѿ�������ͬ��Tag"
#define  CAPTION_LOGIN                 "��¼"
#define  CHECK_TAG_RET_OK              "���ݿ��ﲻ���ڸ�Tag"
#define  CHECK_TAG_RET_ERROR           "���ݿ�����ڸ�Tag"
#define  MSG_BOX_NEW_INVENTORY_BIG     "�̵�û�б��棬Ҫ��ʼ�µ��̵���"
#define  CAPTION_NEW_INVENTORY_BIG     "�̵�"
#define  CAPTION_SAVE_INVENTORY_BIG    "����"
#define  INV_BIG_SAVE_RET_OK           "�̵㱣��ɹ�"
#define  INV_BIG_SAVE_RET_ERROR        "�̵㱣��ʧ��"
#define  MSG_BOX_BATCH_NOT_MATCH       "С��װ�����źʹ��װ��һ��"
#define  MSG_BOX_FACTORY_NOT_MATCH     "С��װ�Ĳ��ر���ʹ��װ��һ��"
#define  MSG_BOX_PRODUCT_NOT_MATCH     "С��װ�Ĳ�Ʒ����ʹ��װ��һ��"

#define  DATABASE_STATUS_LABEL_ID      "lblDatabaseStatus"
#define  READER_STATUS_LABEL_ID        "lblReaderStatus"
#define  USER_LABEL_ID                 "lblUser"
#define  TABS_ID                       "switch"
#define  BATCH_ID_EDIT_ID              "batch_id"
#define  PACKAGE_ID_EDIT_ID            "package_id"
#define  START_SMALL_BUTTON_ID         "btnStartSmall"
#define  STOP_SMALL_BUTTON_ID          "btnStopSmall"
#define  SAVE_SMALL_BUTTON_ID          "btnSaveSmall"
#define  PRINT_SMALL_BUTTON_ID         "btnBarcodeSmall"
#define  COUNT_SMALL_LABEL_ID          "lblCountSmall"
#define  INV_SMALL_SAVE_LABEL_ID       "lblInvSmallSaveRet"
#define  CHECK_TAG_ID_LABEL_ID         "lblCheckTagId"
#define  CHECK_TAG_RET_LABEL_ID        "lblCheckTagResult"
#define  START_BIG_BUTTON_ID           "btnStartBig"
#define  STOP_BIG_BUTTON_ID            "btnStopBig"
#define  SAVE_BIG_BUTTON_ID            "btnSaveBig"
#define  PRINT_BIG_BUTTON_ID           "btnBarcodeBig"
#define  BIG_BATCH_ID_EDIT_ID          "big_batch_id"
#define  BIG_PACKAGE_ID_EDIT_ID        "big_package_id"
#define  INV_BIG_SAVE_LABEL_ID         "lblInvBigSaveRet"
#define  COUNT_BIG_LABEL_ID            "lblCountBig"
#define  INV_BIG_LIST_ID               "LstInvBig"
#define  MANUAL_SMALL_PKG_ID_EDIT_ID   "small_pkg_barcode"
#define  MANUAL_SMALL_PKG_ID_BUTTON_ID "btnAddSmallBarcode"
#define  QUERY_START_TIME_DATETIME_ID  "DateTimeStart"
#define  QUERY_END_TIME_DATETIME_ID    "DateTimeEnd"
#define  QUERY_BATCH_ID_EDIT_ID        "edtQueryBatchId"
#define  QUERY_OPERATOR_EDIT_ID        "edtQueryOperator"
#define  QUERY_TYPE_COMBO_ID           "cmbQueryType"
#define  QUERY_BIG_LIST_ID             "lstQueryBig"
#define  QUERY_SMALL_LIST_ID           "lstQuerySmall"
#define  QUERY_TAGS_LIST_ID            "lstQueryTags"
#define  QUERY_BUTTON_ID               "btnQuery"

#define  TABS_INDEX_INVENTORY_SMALL    0
#define  TABS_INDEX_INVENTORY_BIG      1
#define  TABS_INDEX_INVENTORY_QUERY    2
#define  TABS_INDEX_INVENTORY_CHECK    3

#define  INDEX_INV_BIG_PACKAGE_ID      0

#define  MIN_TIMER_ID                  10000
#define  MAX_TIMER_ID                  19999

#define  INV_BIG_CHAR_TIMER            MIN_TIMER_ID
#define  INV_BIG_CHAR_TIMER_INTEVAL    100                          // 100����

#define  FLOW_NUM_LEN                         4
#define  MAX_FLOW_NUMBER                      10000
#define  MAX_FLOW_NUMBER_BIG                  1000

#define  NORMAL_COLOR                  0xFF386382
#define  ERROR_COLOR                   0xFFFF0000

#define  YES_TEXT                      "��"
#define  NO_TEXT                       "��"

#define  SET_CONTROL_TEXT(ctrl,text)      do { if ( ctrl ) { ctrl->SetText(text); } } while( 0 )
#define  GET_CONTROL_TEXT(ctrl)           ( (ctrl == 0) ? "" : ctrl->GetText() )
#define  SET_CONTROL_ENABLED( ctrl, e )   do { if ( ctrl ) { ctrl->SetEnabled(e); } } while( 0 )

#define  SET_CONTROL_TEXT_COLOR(ctrl, color) do { if ( ctrl ) { ctrl->SetTextColor(color); } } while( 0 )

#define  MAX_TAG_ID_LENGTH              32
typedef struct tagTagItem {
	BYTE     abyUid[MAX_TAG_ID_LENGTH];
	DWORD    dwUidLen;
}TagItem;

class CTagItemParam : public LmnToolkits::MessageData {
public:
	CTagItemParam(const TagItem * pItem);
	~CTagItemParam();

	TagItem   m_item;
};

#define   MAX_USER_ID_LENGTH             32
#define   MAX_USER_NAME_LENGTH           32
typedef struct tagUser {
	char     szUserId[MAX_USER_ID_LENGTH];
	char     szUserName[MAX_USER_NAME_LENGTH];
	TagItem  tTagId;

	// ������Ȩ��
	// Ȩ��
}User;


class CInvSmallSaveParam : public LmnToolkits::MessageData {
public:
	CInvSmallSaveParam();
	~CInvSmallSaveParam();

	CString                  m_strBatchId;
	std::vector<TagItem *>   m_items;
};

class CInvBigSaveParam : public LmnToolkits::MessageData {
public:
	CInvBigSaveParam();
	~CInvBigSaveParam();

	CString                  m_strBatchId;
	std::vector<CString *>   m_items;
};


class CQueryParam : public LmnToolkits::MessageData {
public:
	CQueryParam();
	CQueryParam(time_t tStart, time_t tEnd, const char * szBatchId, const char * szOperator, int nQueryType);
	~CQueryParam();

	time_t                   m_tStart;
	time_t                   m_tEnd;
	CString                  m_strBatchId;
	CString                  m_strOperator;
	int                      m_nQueryType;
};

class CQuerySmallParam : public LmnToolkits::MessageData {
public:
	CQuerySmallParam() {};
	CQuerySmallParam(DWORD dwId) : m_dwId(dwId) {};
	~CQuerySmallParam() {};

	DWORD                   m_dwId;
};

class CQueryBigParam : public LmnToolkits::MessageData {
public:
	CQueryBigParam() {};
	CQueryBigParam(DWORD dwId) : m_dwId(dwId) {};
	~CQueryBigParam() {};

	DWORD                   m_dwId;
};


#define MAX_TIME_BUF_LENGTH           32
#define MAX_BATCH_ID_LENGTH           32

// ��ѯ����Ĵ�/С��¼
typedef struct tagQueryResultItem {
	DWORD     dwId;
	char      szOperator[MAX_USER_NAME_LENGTH];
	char      szProcTime[MAX_TIME_BUF_LENGTH];
	char      szBatchId[MAX_BATCH_ID_LENGTH];
	DWORD     dwParentId;                                // �������װID
}QueryResultItem;

// ��ѯ���
typedef struct tagQueryResult {
	int    nQueryType;
	std::vector<QueryResultItem*> * pvRet;
}QueryResult;


// ��ѯС��װ��Tags
typedef struct tagQuerySmallResultItem {
	TagItem    item;
}QuerySmallResultItem;

// ��ѯС��װTags�Ľ��
// std::vector<QuerySmallResultItem*> * pvRet;


typedef void(*CB_TAG_REPORT_HANDLE)(void *cbParam, RFID_READER_HANDLE hreader, DWORD AIPtype, DWORD TagType, DWORD AntId, BYTE uid[], WORD uidlen);
extern err_t rfid_inventory(RFID_READER_HANDLE hreader, BOOLEAN newAI, BYTE Antennas[], BYTE AntCount,
	BOOLEAN enISO15693, BOOLEAN enAFI, BYTE afi, BOOLEAN enISO14443A, BOOLEAN enISO18000p3m3,
	CB_TAG_REPORT_HANDLE tag_report_handler, void *cbParam, DWORD *tagCount);

extern char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit = '\0');
extern TagItem * GetUid(TagItem * pTagItem, const char * szUid, BOOL bWithSplitChar = FALSE );

extern int CompTag(const TagItem * p1, const TagItem * p2);
extern time_t  ConvertDateTime(const SYSTEMTIME & t);


// templates
template <class T>
void ClearVector(std::vector<T> & v) {
	typedef std::vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}