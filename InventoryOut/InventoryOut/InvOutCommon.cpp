#include "InvOutCommon.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;


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

char * String2SqlString (char * szDest, DWORD dwDestSize, const char * szSrc ) {
	int ret = StrReplaceAll(szDest, dwDestSize - 1, szSrc, "'", "''");
	if (0 == ret) {
		return szDest;
	}
	else {
		return 0;
	}
}

const char *  GetProvinceCode(int nComboIndex) {

	const char * szCode = 0;
	switch (nComboIndex)
	{
	case 0:  //全国
		szCode = "000";
		break;

	case 1: // 北京
		szCode = "010";
		break;

	case 2: // 上海
		szCode = "021";
		break;

	case 3:  // 江苏
		szCode = "025";
		break;

	case 4:  // 浙江
		szCode = "571";
		break;

	case 5:  // 安徽
		szCode = "551";
		break;

	case 6:  // 江西
		szCode = "791";
		break;

	case 7:  // 福建
		szCode = "591";
		break;

	case 8:  // 山东
		szCode = "531";
		break;

	case 9:  // 河南
		szCode = "371";
		break;

	case 10:  // 湖北
		szCode = "027";
		break;

	case 11:  // 湖南
		szCode = "731";
		break;

	case 12:   // 天津市
		szCode = "022";
		break;

	case 13:   // 河北
		szCode = "311";
		break;

	case 14:  // 山西
		szCode = "351";
		break;

	case 15:  // 内蒙古自治区
		szCode = "471";
		break;

	case 16:  // 广东
		szCode = "020";
		break;

	case 17:  // 广西壮族自治区
		szCode = "771";
		break;

	case 18:  // 海南
		szCode = "898";
		break;

	case 19:  // 香港
		szCode = "HK";
		break;

	case 20:  // 澳门
		szCode = "MA";
		break;

	case 21:  // 陕西
		szCode = "029";
		break;

	case 22:  // 甘肃
		szCode = "931";
		break;

	case 23: // 青海
		szCode = "971";
		break;

	case 24: // 宁夏回族
		szCode = "951";
		break;

	case 25:  // 新疆
		szCode = "991";
		break;

	case 26:  // 辽宁
		szCode = "024";
		break;

	case 27:  // 内蒙古
		szCode = "471";
		break;

	case 28:  // 吉林
		szCode = "431";
		break;

	case 29:  // 黑龙江
		szCode = "451";
		break;

	case 30:   // 四川
		szCode = "028";
		break;

	case 31:  // 贵州
		szCode = "851";
		break;

	case 32:  // 云南
		szCode = "871";
		break;

	case 33:  // 西藏
		szCode = "891";
		break;

	case 34:  // 重庆
		szCode = "023";
		break;

	default:
		szCode = "";
		break;
	}

	return szCode;
}