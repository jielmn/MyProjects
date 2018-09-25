#include <time.h>
#include "ReaderToolCommon.h"
#include "TelemedReader.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
DWORD     g_dwReaderVersion = 0;

const char * GetErrorDescription( int e )  {
	const char * szMsg = 0;
	switch (e)
	{
	case CTelemedReader::ERROR_OK:
		szMsg = "OK";
		break;

	case CTelemedReader::ERROR_DISCONNECTED:
		szMsg = "Reader没有连接";
		break;

	case CTelemedReader::ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT:
		szMsg = "没有收到数据或数据格式错误";
		break;

	case CTelemedReader::ERROR_FAIL:
		szMsg = "未知失败";
		break;

	default:
		szMsg = "";
		break;
	}

	return szMsg;
}

char * DateTime2Str(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit /*= '\0'*/) {
	if (0 == buf || 0 == dwBufLen) {
		return 0;
	}

	if ((WORD)dwBufLen < uidlen * 3) {
		return 0;
	}

	if (0 == uidlen) {
		buf[0] = '\0';
		return buf;
	}

	if (chSplit != '\0') {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf(buf + i * 3, "%02x", uid[i]);
			if (i < uidlen - 1) {
				buf[i * 3 + 2] = chSplit;
			}
		}
		buf[uidlen * 3] = '\0';
	}
	else {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf(buf + i * 2, "%02x", uid[i]);
		}
		buf[uidlen * 2] = '\0';
	}


	return buf;
}