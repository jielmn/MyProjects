
#include "zsCommon.h"
#include "LmnString.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
HWND    g_hWnd = 0;

LmnToolkits::Thread *  g_thrd_db = 0;                     // ���ݿ��߳�
LmnToolkits::Thread *  g_thrd_reader = 0;                 // Reader�߳�
LmnToolkits::Thread *  g_thrd_timer = 0;                  // TIMER



char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize) {
	if (dest == 0 || dwDestSize == 0) {
		return 0;
	}

	if (pSrc == 0 || dwSrcSize == 0) {
		dest[0] = '\0';
		return dest;
	}

	char * pTmp = new char[dwSrcSize];
	if (0 == pTmp) {
		dest[0] = '\0';
		return dest;
	}

	memcpy(pTmp, pSrc, dwSrcSize);

	for (DWORD i = 0; i < dwSrcSize; i++) {
		pTmp[i] += 5;
	}

	int ret = EncodeBase64(dest, dwDestSize, pTmp, dwSrcSize);
	delete[] pTmp;

	if (LMNX_OK != ret) {
		dest[0] = '\0';
		return dest;
	}

	return dest;
}

int MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize) {
	if (pDest == 0 || dwDestSize == 0) {
		return 1;
	}

	if (szSrc == 0) {
		dwDestSize = 0;
		return 0;
	}

	DWORD dwSrcLen = strlen(szSrc);
	char * pTmp = new char[dwSrcLen];
	if (0 == pTmp) {
		return -1;
	}

	int ret = DecodeBase64(pTmp, &dwSrcLen, szSrc);
	if (LMNX_OK != ret) {
		delete[] pTmp;
		return 2;
	}

	if (dwSrcLen > dwDestSize) {
		delete[] pTmp;
		return 3;
	}

	char * dest = (char *)pDest;
	for (DWORD i = 0; i < dwSrcLen; i++) {
		dest[i] = pTmp[i] - 5;
	}
	dwDestSize = dwSrcLen;

	delete[] pTmp;
	return 0;
}


const char * GetErrDescription(int e) {
	const char * szDescription = 0;
	switch (e) {
	case 0:
		szDescription = "OK";
		break;

	case ZS_ERR_DB_CLOSE:
		szDescription = "�������ݿ�ʧ��";
		break;

	case ZS_ERR_DB_NOT_UNIQUE:
		szDescription = "Υ��ΨһԼ��";
		break;

	case ZS_ERR_DB_ERROR:
		szDescription = "���ݿ����";
		break;

	default:
		szDescription = "δ֪����";
	}
	return szDescription;
}

const char * GetGender(BOOL bFemale) {
	if (bFemale) {
		return "Ů";
	}
	else {
		return "��";
	}
}

const char * GetInHosStatus(BOOL bOutHos) {
	if (bOutHos) {
		return "��Ժ";
	}
	else {
		return "סԺ";
	}
}