//#include <windows.h>
#include <afxdb.h>   
#include <odbcinst.h>
#include <assert.h>
#include <time.h>
#include "Business.h"
#include "MyDatabase.h"

#define  PATIENTS_TABLE_NAME              "Patients"
#define  TAGS_TABLE_NAME                  "Tags"

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
}

void  CBusiness::OnDeinit(int * ret) {
	assert(ret);
	CMyDatabase::GetInstance()->sigDeinitDb.emit(ret);
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
		sSql = "SELECT * from [demo$]";
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

char * ConvertDateTime(char * szDest, DWORD dwDestSize, time_t * t) {
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