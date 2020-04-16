#include <time.h>
#include <assert.h>
#include "common.h"

CGlobalData  g_data;

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

bool sortDWord(DWORD d1, DWORD d2) {
	return d1 < d2;
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
	BYTE buf[256] = { 0 };
	//LmnSleep(1000);
	memcpy(buf, "\x00\x01\x0A\x55\x01\x01\xDD\xAA", 8);
	buf[1] = (BYTE)g_data.m_nReaderNoStart;
	buf[2] = (BYTE)g_data.m_nReaderNoEnd;
	DWORD  dwWriteSize = 8;
	pPort->Write(buf, dwWriteSize);
}

char *  GetTagId(char * szTagId, DWORD dwTagIdLen, const BYTE * pData, DWORD dwDataLen) {
	if (0 == szTagId || 0 == dwTagIdLen) {
		return 0;
	}

	if (dwDataLen != 8) {
		szTagId[0] = '\0';
		return szTagId;
	}

	if (dwTagIdLen < 16) {
		szTagId[0] = '\0';
		return szTagId;
	}

	for (DWORD i = 0; i < dwDataLen; i++) {
		SNPRINTF(szTagId + 2 * i, 3, "%02x", pData[dwDataLen - i - 1]);
	}

	return szTagId;
}

char *  GetHandReaderId(char * szReaderId, DWORD dwReaderIdLen, const BYTE * pData) {
	assert(szReaderId && dwReaderIdLen > 6);
	DWORD  dwSn = (pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | pData[3];
	SNPRINTF(szReaderId, dwReaderIdLen, "%06lu", dwSn);
	return szReaderId;
}