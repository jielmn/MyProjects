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

// thread message
#define MSG_RECONNECT_DB           1
#define MSG_RECONNECT_READER       2
#define MSG_READER_INVENTORY       3
#define MSG_USER_LOGIN             4
#define MSG_INV_SMALL_SAVE         5
#define MSG_CHECK_TAG              6

// db reconnect 时间
#define  RECONNECT_DB_TIME         10000
#define  RECONNECT_READER_TIME     10000

// 错误码
#define INV_ERR_NO_MEMORY          10001
#define INV_ERR_DB_CLOSE           10002
#define INV_ERR_DB_ERROR           10003
#define INV_ERR_DB_NOT_UNIQUE      10004

// windows 自定义消息
#define UM_SHOW_DB_STATUS          (WM_USER+1)
#define UM_SHOW_READER_STATUS      (WM_USER+2)
#define UM_INVENTORY_RESULT        (WM_USER+3)
#define UM_LOGIN_RESULT            (WM_USER+4)
#define UM_INV_SMALL_SAVE_RESULT   (WM_USER+5)
#define UM_CHECK_TAG_RESULT        (WM_USER+6)
#define UM_TIMER                   (WM_USER+7)


// config配置字符串常量
#define ODBC_TYPE                  "odbc type"
#define ODBC_STRING_ORACLE         "odbc string oracle"
#define ODBC_STRING_MYSQL          "odbc string mysql"
#define FACTORY_CODE               "factory code"
#define PRODUCT_CODE               "product code"
#define LAST_BATCH_ID              "last batch id"


char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
int    MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);


#define  DB_STATUS_OK_TEXT             "数据库连接OK"
#define  DB_STATUS_CLOSE_TEXT          "数据库连接失败"
#define  READER_STATUS_OK_TEXT         "读卡器连接OK"
#define  READER_STATUS_CLOSE_TEXT      "读卡器连接失败"
#define  MSG_BOX_NEW_INVENTORY_SMALL   "盘点没有保存，要开始新的盘点吗？"
#define  CAPTION_NEW_INVENTORY_SMALL   "盘点"
#define  MSG_BOX_DB_CLOSE              "数据库没有连接上"
#define  CAPTION_SAVE_INVENTORY_SMALL  "保存"
#define  MSG_BOX_BATCH_ID_EMPTY        "批号不能为空"
#define  MSG_BOX_WRONG_BATCH_ID_FORMAT "批号格式不对!必须是年+月+号码，例如\"20170906\""
#define  INV_SMALL_SAVE_RET_OK         "盘点保存成功"
#define  INV_SMALL_SAVE_RET_ERROR      "盘点保存失败"
#define  INV_SMALL_SAVE_RET_SAME_TAG   "盘点保存失败：数据库里已经存在相同的Tag"
#define  CAPTION_LOGIN                 "登录"
#define  CHECK_TAG_RET_OK              "数据库里不存在该Tag"
#define  CHECK_TAG_RET_ERROR           "数据库里存在该Tag"
#define  MSG_BOX_NEW_INVENTORY_BIG     "盘点没有保存，要开始新的盘点吗？"
#define  CAPTION_NEW_INVENTORY_BIG     "盘点"
#define  CAPTION_SAVE_INVENTORY_BIG    "保存"

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

#define  TABS_INDEX_INVENTORY_SMALL    0
#define  TABS_INDEX_INVENTORY_BIG      1
#define  TABS_INDEX_INVENTORY_QUERY    2
#define  TABS_INDEX_INVENTORY_CHECK    3

#define  MIN_TIMER_ID                  10000
#define  MAX_TIMER_ID                  19999

#define  INV_BIG_CHAR_TIMER            MIN_TIMER_ID
#define  INV_BIG_CHAR_TIMER_INTEVAL    100                          // 100毫秒

#define  FLOW_NUM_LEN                         3

#define  NORMAL_COLOR                  0xFF386382
#define  ERROR_COLOR                   0xFFFF0000

#define  SET_CONTROL_TEXT(ctrl,text)    do { if ( ctrl ) { ctrl->SetText(text); } } while( 0 )
#define  GET_CONTROL_TEXT(ctrl)         ( (ctrl == 0) ? "" : ctrl->GetText() )

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

	// 以下是权限
	// 权限
}User;


class CInvSmallSaveParam : public LmnToolkits::MessageData {
public:
	CInvSmallSaveParam();
	~CInvSmallSaveParam();

	CString                  m_strBatchId;
	std::vector<TagItem *>   m_items;
};


typedef void(*CB_TAG_REPORT_HANDLE)(void *cbParam, RFID_READER_HANDLE hreader, DWORD AIPtype, DWORD TagType, DWORD AntId, BYTE uid[], WORD uidlen);
extern err_t rfid_inventory(RFID_READER_HANDLE hreader, BOOLEAN newAI, BYTE Antennas[], BYTE AntCount,
	BOOLEAN enISO15693, BOOLEAN enAFI, BYTE afi, BOOLEAN enISO14443A, BOOLEAN enISO18000p3m3,
	CB_TAG_REPORT_HANDLE tag_report_handler, void *cbParam, DWORD *tagCount);

extern char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit = '\0');
extern int CompTag(const TagItem * p1, const TagItem * p2);


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