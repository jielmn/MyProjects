#include <time.h>
#include "common.h"
#include "MyControls.h"

CGlobalData  g_data;

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if ( 0 == strcmp(pstrClass, "TempRule") ) {
		return new CTempRuleUI;
	}
	return NULL;
}

bool sortInt(int d1, int d2) {
	return d1 > d2;
}

CControlUI* CALLBACK FindControlByName(CControlUI* pSubControl, LPVOID pParam) {
	const char * name = (const char *)pParam;
	if (pSubControl->GetName() == name) {
		return pSubControl;
	}
	return 0;
}

DWORD GetIntFromDb(const char * szValue, int nDefault /*= 0*/) {
	DWORD dwValue = nDefault;
	if (szValue)
		sscanf_s(szValue, "%lu", &dwValue);
	return dwValue;
}

char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue) {
	assert(dwSize > 0);
	if (szValue)
		STRNCPY(buf, szValue, dwSize);
	else if (dwSize > 0)
		buf[0] = '\0';
	return buf;
}

int  GetCh340Count(char * szComPort, DWORD dwComPortLen) {

	std::map<int, std::string> m;
	GetAllSerialPorts(m);

	char buf[8192];
	int nFindCount = 0;
	std::map<int, std::string>::iterator it;
	for (it = m.begin(); it != m.end(); it++) {
		std::string & s = it->second;
		Str2Lower(s.c_str(), buf, sizeof(buf));

		if (0 != strstr(buf, "usb-serial ch340")) {
			// 如果是第一次记录 comport
			if (0 == nFindCount) {
				int nComPort = 0;
				const char * pFind = strstr(buf, "com");
				while (pFind) {
					if (1 == sscanf(pFind + 3, "%d", &nComPort)) {
						break;
					}
					pFind = strstr(pFind + 3, "com");
				}
				assert(nComPort > 0);
				SNPRINTF(szComPort, dwComPortLen, "com%d", nComPort);
			}
			nFindCount++;
		}
	}
	return nFindCount;
}

void InitComPort(CLmnSerialPort * pPort) {

}