#include "InvOutDatabase.h"

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "crypt32.lib")

CInvoutDatabase::CInvoutDatabase() {
	
}

CInvoutDatabase::~CInvoutDatabase() {
	
}

// µÇÂ¼
int CInvoutDatabase::Login(const CLoginParam * pParam) {
	if ( GetStatus() == STATUS_CLOSE ) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char buf[8192];
	char szUserName[256];
	char szUserPassword[256];

	String2SqlString( szUserName,     sizeof(szUserName),     pParam->m_szUserName );
	String2SqlString( szUserPassword, sizeof(szUserPassword), pParam->m_szUserPassword );

	SNPRINTF(buf, sizeof(buf), "SELECT * FROM staff WHERE ext='%s' AND ext='%s'", szUserName, szUserPassword);
	int ret = OpenRecordSet(buf);
	if (0 != ret) {
		return ret;
	}

	ret = MoveNext();
	if (0 != ret) {
		CloseRecordSet();
		return ret;
	}

	CloseRecordSet();
	return 0;
}