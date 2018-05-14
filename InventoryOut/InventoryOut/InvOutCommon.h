#pragma once

#include <vector>
#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "UIlib.h"


#define  MSG_RECONNECT_DB               1
#define  MSG_LOGIN                      2
#define  MSG_ADD_AGENCY                 3
#define  MSG_GET_ALL_AGENCY             4
#define  MSG_MODIFY_AGENCY              5
#define  MSG_DELETE_AGENCY              6
#define  MSG_GET_ALL_SALES              7
#define  MSG_GET_ALL_AGENCY_FOR_TARGET  8
#define  MSG_SAVE_INV_OUT               9
#define  MSG_QUERY_BY_TIME              10
#define  MSG_QUERY_BY_BIG_PKG           11

#define  RECONNECT_DB_TIME              10000

#define  UM_DB_STATUS                   (WM_USER+1)
#define  UM_LOGIN_RET                   (WM_USER+2)
#define  UM_ADD_AGENCY_RET              (WM_USER+3)
#define  UM_GET_ALL_AGENCY_RET          (WM_USER+4)
#define  UM_MODIFY_AGENCY_RET           (WM_USER+5)
#define  UM_DELETE_AGENCY_RET           (WM_USER+6)
#define  UM_GET_ALL_SALES               (WM_USER+7)
#define  UM_GET_ALL_AGENCY_FOR_TARGET_RET  (WM_USER+8)
#define  UM_TIMER_RET                      (WM_USER+9)
#define  UM_SAVE_INV_OUT_RET               (WM_USER+10)
#define  UM_QUERY_BY_TIM_RET               (WM_USER+11)
#define  UM_QUERY_BY_BIG_PKG_RET           (WM_USER+12)



#define TARGET_TYPE_SALES              0
#define TARGET_TYPE_AGENCIES           1


#define  MIN_TIMER_ID                  10000
#define  MAX_TIMER_ID                  19999

#define  INV_OUT_CHAR_TIMER            MIN_TIMER_ID
#define  INV_OUT_CHAR_TIMER_INTEVAL    100                          // 100毫秒

#define  FLOW_NUM_LEN                         4

class CLoginParam : public LmnToolkits::MessageData {
public:
	CLoginParam(const char * szUserName, const char * szPasswrod ) {
		STRNCPY(m_szUserName,     szUserName, sizeof(m_szUserName));
		STRNCPY(m_szUserPassword, szPasswrod, sizeof(m_szUserPassword));
	}

	char    m_szUserName[256];
	char    m_szUserPassword[256];
};

typedef struct tagAgencyItem {
	DWORD   dwId;
	char    szName[64];
	char    szId[64];
	char    szProvince[32];
}AgencyItem;


class CAgencyParam : public LmnToolkits::MessageData {
public:
	CAgencyParam(const AgencyItem * pItem) {
		memcpy(&m_tAgency, pItem, sizeof(AgencyItem));
	}

	CAgencyParam(DWORD dwId, const char * szId) {
		memset(&m_tAgency, 0, sizeof(AgencyItem));
		m_tAgency.dwId = dwId;
		STRNCPY(m_tAgency.szId, szId, sizeof(m_tAgency.szId));
	}

	AgencyItem   m_tAgency;
};

typedef struct tagSaleStaff {
	char    szId[128];
	char    szName[128];	
}SaleStaff;


class CSaveInvOutParam : public LmnToolkits::MessageData {
public:
	CSaveInvOutParam( int nTargetType, const DuiLib::CDuiString & strTargetId, const DuiLib::CDuiString & strOperatorId, 
		             const std::vector<DuiLib::CDuiString *> & vBig, const std::vector<DuiLib::CDuiString *> & vSmall);
	~CSaveInvOutParam();

	int                   m_nTargetType;
	DuiLib::CDuiString    m_strTargetId;
	DuiLib::CDuiString    m_strOperatorId;

	std::vector<DuiLib::CDuiString *> m_vBig;
	std::vector<DuiLib::CDuiString *> m_vSmall;
};

// 大、小包装数据库结构
typedef struct tagPackage {
	DWORD     dwId;
	char      szId[32];
	DWORD     dwParentId;
}Package;


class CQueryByTimeParam : public LmnToolkits::MessageData {
public:
	CQueryByTimeParam(time_t tStart, time_t tEnd, int nTargetType, const char * szTargetId) {
		m_tStart = tStart;
		m_tEnd = tEnd;
		m_nTargetType = nTargetType;

		if (szTargetId) {
			STRNCPY(m_szTargetId, szTargetId, sizeof(m_szTargetId));
		}
		else {
			memset(m_szTargetId, 0, sizeof(m_szTargetId));
		}
	}

	time_t  m_tStart;
	time_t  m_tEnd;
	int     m_nTargetType;
	char    m_szTargetId[128];
};

typedef struct tagQueryByTimeItem {
	int     m_nPackageType;
	char    m_szPackageId[32];
	int     m_nTargetType;
	char    m_szTargetName[64];
	char    m_szOperatorName[32];
	time_t  m_tOperatorTime;
}QueryByTimeItem;


class CQueryByBigPkgParam : public LmnToolkits::MessageData {
public:
	CQueryByBigPkgParam(const char * szBigPkgId) {
		STRNCPY(m_szBigPkgId, szBigPkgId, sizeof(m_szBigPkgId));
	}

	char    m_szBigPkgId[32];
};

#define  PKG_STATUS_OUT                0                  // 已出库
#define  PKG_STATUS_HALF_OUT           1                  // 半出库
#define  PKG_STATUS_NOT_OUT            2                  // 没有出库

typedef struct tagPkgItem {
	DWORD    dwPkgId;
	DWORD    dwStatus;
	char     szId[32];

	char     szInOperator[16];
	time_t   tInTime;

	int      nOutTargetType;
	char     szOutTargetName[64];
	char     szOutOperator[16];
	time_t   tOutTime;
}PkgItem;


#define  PACKAGE_TYPE_BIG         0
#define  PACKAGE_TYPE_SMALL       1


extern ILog    * g_log;
extern IConfig * g_cfg;

extern char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
extern int MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);
char * String2SqlValue(char * szDest, DWORD dwDestSize, const char * strValue);

const char * GetErrorDescription(int ret);
extern char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern time_t  String2DateTime(const char * szDatetime);

time_t SystemTime2Time(const SYSTEMTIME & t);
SYSTEMTIME Time2SystemTime(time_t  t);

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