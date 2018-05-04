#pragma once

#include <vector>
#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"



#define  MSG_RECONNECT_DB               1
#define  MSG_LOGIN                      2
#define  MSG_ADD_AGENCY                 3
#define  MSG_GET_ALL_AGENCY             4
#define  MSG_MODIFY_AGENCY              5
#define  MSG_DELETE_AGENCY              6
#define  MSG_GET_ALL_SALES              7
#define  MSG_GET_ALL_AGENCY_FOR_TARGET  8

#define  RECONNECT_DB_TIME              10000

#define  UM_DB_STATUS                   (WM_USER+1)
#define  UM_LOGIN_RET                   (WM_USER+2)
#define  UM_ADD_AGENCY_RET              (WM_USER+3)
#define  UM_GET_ALL_AGENCY_RET          (WM_USER+4)
#define  UM_MODIFY_AGENCY_RET           (WM_USER+5)
#define  UM_DELETE_AGENCY_RET           (WM_USER+6)
#define  UM_GET_ALL_SALES               (WM_USER+7)



#define TARGET_TYPE_SALES              0
#define TARGET_TYPE_AGENCIES           1

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

	CAgencyParam(DWORD dwId) {
		memset(&m_tAgency, 0, sizeof(AgencyItem));
		m_tAgency.dwId = dwId;
	}

	AgencyItem   m_tAgency;
};

typedef struct tagSaleStaff {
	char    szId[128];
	char    szName[128];	
}SaleStaff;



extern ILog    * g_log;
extern IConfig * g_cfg;

extern char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
extern int MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);
char * String2SqlValue(char * szDest, DWORD dwDestSize, const char * strValue);

const char * GetErrorDescription(int ret);


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