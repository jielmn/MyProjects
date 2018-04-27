#pragma once

#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"



#define  MSG_RECONNECT_DB               1
#define  MSG_LOGIN                      2
#define  MSG_ADD_AGENCY                 3

#define  RECONNECT_DB_TIME              10000

#define  UM_DB_STATUS                   (WM_USER+1)
#define  UM_LOGIN_RET                   (WM_USER+2)


class CLoginParam : public LmnToolkits::MessageData {
public:
	CLoginParam(const char * szUserName, const char * szPasswrod ) {
		STRNCPY(m_szUserName,     szUserName, sizeof(m_szUserName));
		STRNCPY(m_szUserPassword, szPasswrod, sizeof(m_szUserPassword));
	}

	char    m_szUserName[256];
	char    m_szUserPassword[256];
};

class CAgencyParam : public LmnToolkits::MessageData {
public:
	CAgencyParam(const char * szId, const char * szName, const char * szProvinceCode) {
		STRNCPY(m_szId,            szId,    sizeof(m_szId));
		STRNCPY(m_szName,          szName,  sizeof(m_szName));
		STRNCPY(m_szProvinceCode,  szName,  sizeof(m_szProvinceCode));
	}

	char    m_szName[256];
	char    m_szId[256];
	char    m_szProvinceCode[16];
};

extern ILog    * g_log;
extern IConfig * g_cfg;

extern char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
extern int MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);
extern char * String2SqlString(char * szDest, DWORD dwDestSize, const char * szSrc);

const char *    GetProvinceCode( int nComboIndex );