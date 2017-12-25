//#include <windows.h>
#include <afxdb.h>   
#include <odbcinst.h>
#include <assert.h>
#include <time.h>
#include "Business.h"
#include "MyDatabase.h"
#include "BindingReader.h"
#include "SerialPort.h"
#include "LmnString.h"

#define  PATIENTS_TABLE_NAME              "Patients"
#define  TAGS_TABLE_NAME                  "Tags"

IConfig *  g_cfg = 0;
ILog *     g_log = 0;

CBusiness *  CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;		
	}
	return pInstance;
}

CBusiness::CBusiness() {
	sigAMPatient.connect(this, &CBusiness::OnPatientEvent);
	sigInit.connect(this, &CBusiness::OnInit);
	sigDeinit.connect(this, &CBusiness::OnDeinit);
}

void  CBusiness::OnInit(int * ret) {
	assert(ret);
	CMyDatabase::GetInstance()->sigInitDb.emit(ret);
	if (*ret != 0) {
		return;
	}

	*ret = InitBindingReader();
	if (*ret != 0) {
		return;
	}

	// 初始化一些线程

	LmnToolkits::ThreadManager::GetInstance();
	// 创建绑定温度贴的线程
	g_thrd_binding_reader = new LmnToolkits::Thread();
	g_thrd_binding_reader->Start();

	// 创建message handler
	g_handler_binding_reader = new MessageHandlerBindingReader();

	g_thrd_binding_reader->PostDelayMessage(DELAY_INVENTORY_TIME, g_handler_binding_reader, MSG_INVENTORY);


	// 创建串口线程
	g_thrd_serial_port = new LmnToolkits::Thread();
	g_thrd_serial_port->Start();

	// 创建message handler
	g_handler_serial_port = new MessageHandlerSerialPort();

	g_thrd_serial_port->PostMessage(g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);

}

int  CBusiness::InitBindingReader() {	
	CBindingReader::GetInstance()->Init();
	return 0;
}

void  CBusiness::OnDeinit(int * ret) {
	assert(ret);
	CMyDatabase::GetInstance()->sigDeinitDb.emit(ret);

	// 销毁线程
	g_thrd_binding_reader->Stop();
	delete g_thrd_binding_reader;

	// 销毁message handler
	delete g_handler_binding_reader;

	g_thrd_serial_port->Stop();
	delete g_thrd_serial_port;

	delete g_handler_serial_port;
}

void  CBusiness::OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret) {
	assert(ret);

	if (pPatientInfo->szId[0] == '\0') {
		*ret = (GlobalErrorNo)ERROR_PATIENT_ID_EMPTY;
		return;
	}

	if (pPatientInfo->szInNo[0] == '\0') {
		*ret = (GlobalErrorNo)ERROR_PATIENT_INNO_EMPTY;
		return;
	}

	if (pPatientInfo->szName[0] == '\0') {
		*ret = (GlobalErrorNo)ERROR_PATIENT_NAME_EMPTY;
		return;
	}

	if (pPatientInfo->nAge <= 0 ) {
		*ret = (GlobalErrorNo)ERROR_PATIENT_AGE_WRONG_FORMAT;
		return;
	}

	CMyDatabase::GetInstance()->sigAMPatient.emit(pPatientInfo, eType, ret);	

	// 如果成功
	if ( 0 == *ret) {
		sigAMPatientRet.emit(pPatientInfo, eType);
	}
}

int  CBusiness::GetAllPatients(std::vector<PatientInfo *> & vPatients) {

	CMyDatabase::GetInstance()->GetAllPatients(vPatients);
	return 0;
}

int   CBusiness::GetPatient(const char * szId, PatientInfo * pPatient) {
	return CMyDatabase::GetInstance()->GetPatient(szId, pPatient);
}

int   CBusiness::DeletePatient(const char * szId) {
	return CMyDatabase::GetInstance()->DeletePatient(szId);
}

int  CBusiness::GetPatientTags(const char * szId, std::vector<TagInfo *> & vTags) {
	return CMyDatabase::GetInstance()->GetPatientTags(szId, vTags);
}

int   CBusiness::GetPatientByTag(const TagId * pTagId, PatientInfo * pPatient) {
	return CMyDatabase::GetInstance()->GetPatientByTag(pTagId, pPatient);
}

int   CBusiness::BindingTag(const TagInfo * pTagInfo) {
	return CMyDatabase::GetInstance()->BindingTag(pTagInfo);
}

int   CBusiness::DeleteTag(const TagId * pTagId) {
	return CMyDatabase::GetInstance()->DeleteTag(pTagId);
}

int   CBusiness::SaveTempData(std::vector<TagData*> & v) {
	return CMyDatabase::GetInstance()->SaveTempData(v);
}

int   CBusiness::GetLatestTempData(const char * szPatientId, std::vector<TagData*> & vRet) {
	return CMyDatabase::GetInstance()->GetLatestTempData(szPatientId, vRet);
}

int    CBusiness::GetTempData(const char * szPatientId, time_t tStartTime, std::vector<TagData*> & vRet) {
	return CMyDatabase::GetInstance()->GetTempData(szPatientId, tStartTime, vRet);
}

int  CBusiness::ImportExcel(const char * szFilePath) {
	CString sDriver = GetExcelDriver();
	if (sDriver.GetLength() == 0) {
		return ERROR_EXCEL_DRIVER_NOT_FOUND;
	}

	CString sDsn;
	CDatabase database;
	CString sSql;
	CString sItem;

	sDsn.Format("ODBC;DRIVER={%s};DSN='''';DBQ=%s", sDriver, szFilePath);
	TRY
	{
		//open db  
		database.Open(NULL,false,false,sDsn);
		CRecordset recset(&database);
		//read sql   
		sSql = "SELECT * from [病人信息$]";
		recset.Open(CRecordset::forwardOnly,sSql,CRecordset::readOnly);

		//int n = recset.GetODBCFieldCount();
		//get query results  
		BOOL bFailed = FALSE;
		while (!recset.IsEOF())
		{
			PatientInfo tInfo;

			//read inside value  
			recset.GetFieldValue("编号",     sItem);
			strncpy_s(tInfo.szId, sItem, sizeof(tInfo.szId));
			recset.GetFieldValue("住院号",   sItem);
			strncpy_s(tInfo.szInNo, sItem, sizeof(tInfo.szInNo));
			recset.GetFieldValue("姓名",     sItem);
			strncpy_s(tInfo.szName, sItem, sizeof(tInfo.szName));

			recset.GetFieldValue("性别",     sItem);
			if (0 == strcmp(sItem, "女")) {
				tInfo.bMale = FALSE;
			}
			else {
				tInfo.bMale = TRUE;
			}

			tInfo.nAge = 0;
			recset.GetFieldValue("年龄",     sItem);
			sscanf_s(sItem, " %d", &tInfo.nAge);

			recset.GetFieldValue("科室",     sItem);
			strncpy_s(tInfo.szOffice, sItem, sizeof(tInfo.szOffice));
			recset.GetFieldValue("床号",     sItem);
			strncpy_s(tInfo.szBedNo, sItem, sizeof(tInfo.szBedNo));

			struct tm  t;
			memset(&t, 0, sizeof(t));
			recset.GetFieldValue("入院日期", sItem);
			if (3 != sscanf_s(sItem, "%d/%d/%d", &t.tm_year, &t.tm_mon, &t.tm_mday)) {
				sscanf_s(sItem, "%d-%d-%d", &t.tm_year, &t.tm_mon, &t.tm_mday);
			}
			t.tm_year -= 1900;
			t.tm_mon--;
			tInfo.tInDate = mktime(&t);

			recset.GetFieldValue("就诊卡",   sItem);
			strncpy_s(tInfo.szCardNo, sItem, sizeof(tInfo.szCardNo));

			int ret = 0;
			CBusiness::GetInstance()->sigAMPatient.emit(&tInfo, OPERATION_ADD, &ret);
			if (0 != ret) {
				bFailed = TRUE;
			}
			recset.MoveNext();
		}

		//close db  
		database.Close();

		if (bFailed) {
			return ERROR_PARTIALLY_FAILED_TO_IMPORT_EXCEL;
		}
		return 0;
	}
	CATCH(CDBException, e)
	{
		//db exception occur  
		return ERROR_FAILED_TO_EXECUTE_EXCEL;
	}
	END_CATCH;
	return 0;
}

const char * GetErrDescription(GlobalErrorNo e) {
	const char * szDescription = 0;
	switch (e) {
	case ERROR_OK:
		szDescription = "OK";
		break;
	case ERROR_FAILED_TO_OPEN_DB:
		szDescription = "打开数据库失败";
		break;
	case ERROR_FAILED_TO_CLOSE_DB:
		szDescription = "关闭数据库失败";
		break;
	case ERROR_FAILED_TO_EXECUTE_SQL:
		szDescription = "执行SQL失败";
		break;
	case ERROR_PATIENT_ID_EMPTY:
		szDescription = "病人编号为空";
		break;
	case ERROR_PATIENT_INNO_EMPTY:
		szDescription = "病人住院号为空";
		break;
	case ERROR_PATIENT_NAME_EMPTY:
		szDescription = "病人姓名为空";
		break;
	case ERROR_PATIENT_AGE_WRONG_FORMAT:
		szDescription = "病人年龄格式不对";
		break;
	case ERROR_EXCEL_DRIVER_NOT_FOUND:
		szDescription = "没有找到Excel驱动";
		break;
	case ERROR_PARTIALLY_FAILED_TO_IMPORT_EXCEL:
		szDescription = "导入Excel部分失败";
		break;
	case ERROR_FAILED_TO_EXECUTE_EXCEL:
		szDescription = "执行Excel出现错误";
		break;
	case ERROR_FAILED_TO_LOAD_FUNCTION_DLL:
		szDescription = "加载function.dll失败";
		break;
	default:
		szDescription = "未知错误";
	}
	return szDescription;
}

const char * GetSex(BOOL bMale) {
	if (bMale) {
		return "男";
	}
	else {
		return "女";
	}
}

char * GetInt(char * buf, DWORD dwSize, int n) {
	_snprintf_s(buf, dwSize, dwSize, "%d", n);
	return buf;
}

char * ConvertDate(char * szDest, DWORD dwDestSize, time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d-%02d-%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday );
	return szDest;
}

char * ConvertDateTime(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

time_t  ConvertDateTime(const char * szDatetime) {
	struct tm  tmp;
	if (6 != sscanf_s(szDatetime, "%d-%d-%d %d:%d:%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec)) {
		return 0;
	}

	tmp.tm_year -= 1900;
	tmp.tm_mon--;
	return mktime(&tmp);
}

time_t  ConvertDateTime(const SYSTEMTIME & t) {
	struct tm tmp;
	memset( &tmp, 0, sizeof(tmp) );

	tmp.tm_year = t.wYear - 1900;
	tmp.tm_mon  = t.wMonth - 1;
	tmp.tm_mday = t.wDay;

	return mktime(&tmp);
}

SYSTEMTIME  ConvertDateTime(const time_t & tTime) {
	SYSTEMTIME s;
	struct tm t;
	localtime_s(&t, &tTime);
	memset(&s, 0, sizeof(s));
	s.wYear = t.tm_year + 1900;
	s.wMonth = t.tm_mon + 1;
	s.wDay = t.tm_mday;
	return s;
}

char *  ConvertTagId(char * buf, DWORD dwBufSize, const TagId * pTagId) {	
	int nSize = sizeof(pTagId->Id);
	assert((int)dwBufSize >= 3 * nSize);

	for (int i = 0; i < nSize; i++) {
		_snprintf_s(buf + 3 * i, dwBufSize - 3 * i, _TRUNCATE, "%02X", pTagId->Id[i]);
		if (i < nSize - 1) {
			buf[3*i+2] = '-';
		}
	}
	buf[nSize * 3] = '\0';
	return buf;
}

int ConvertTagId(TagId * pTagId, const char * szTagId) {
	assert(szTagId);
	int nLen = strlen(szTagId);
	if ((nLen + 1) % 3 != 0) {
		return -1;
	}

	int nNum = (nLen + 1) / 3;
	int nSize = sizeof(pTagId->Id);
	if (nSize < nNum ) {
		return -1;
	}

	for (int i = 0; i < nNum; i++) {
		int nTmp;
		sscanf_s(szTagId + 3 * i, "%02X", &nTmp );
		pTagId->Id[i] = nTmp;
	}
	return 0;
}


CString GetExcelDriver()
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	CString sDriver;

	//get driver name function(included in odbcinst.h)  
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return "";
	//check excel included or not  
	do
	{
		if (strstr(pszBuf, "Excel") != 0)
		{
			//found  
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, '\0') + 1;
	} while (pszBuf[1] != '\0');

	return sDriver;
}

BOOL GetBoolean(const char * szStr) {
	if (0 == szStr) {
		return FALSE;
	}
	if (0 == StrICmp(szStr, "true")) {
		return TRUE;
	}
	return FALSE;
}

time_t  TrimDatetime(time_t t) {
	struct tm tmp;
	localtime_s( &tmp, &t);
	tmp.tm_hour = 0;
	tmp.tm_min = 0;
	tmp.tm_sec = 0;

	return mktime(&tmp);
}