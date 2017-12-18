#pragma once

#include <vector>
#include "sigslot.h"
#include "sqlite3.h"

typedef struct tagPatientInfo {
	char     szId[32];
	char     szInNo[32];
	char     szName[32];
	BOOL     bMale;
	int      nAge;
	char     szOffice[32];
	char     szBedNo[32];
	time_t   tInDate;
	char     szCardNo[32];
}PatientInfo;

enum OperationType {
	OPERATION_ADD = 0,
	OPERATION_MODIFY,
	OPERATION_DELETE,
	OPERATION_END,
};

enum  GlobalErrorNo {
	ERROR_OK = 0,
	ERROR_FAILED_TO_OPEN_DB,
	ERROR_FAILED_TO_CLOSE_DB,
	ERROR_FAILED_TO_EXECUTE_SQL,
	ERROR_PATIENT_ID_EMPTY,
	ERROR_PATIENT_INNO_EMPTY,
	ERROR_PATIENT_NAME_EMPTY,
	ERROR_PATIENT_AGE_WRONG_FORMAT,
	ERROR_EXCEL_DRIVER_NOT_FOUND,
	ERROR_PARTIALLY_FAILED_TO_IMPORT_EXCEL,
	ERROR_FAILED_TO_EXECUTE_EXCEL,
};

const char * GetErrDescription(GlobalErrorNo e);
const char * GetSex(BOOL bMale);
char * GetInt(char * buf, DWORD dwSize, int n);
char * ConvertDate(char * szDest, DWORD dwDestSize, time_t * t);
char * ConvertDateTime(char * szDest, DWORD dwDestSize, time_t * t);
time_t  ConvertDateTime(const char * szDatetime);
CString GetExcelDriver();

class CBusiness : public sigslot::has_slots<> {
public:
	CBusiness();
	void OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret);
	void OnInit( int * ret );
	void OnDeinit(int * ret );
	
	int  GetAllPatients( std::vector<PatientInfo *> & vPatients );
	int  GetPatient(const char * szId, PatientInfo * pPatient);
	int  DeletePatient(const char * szId);
	int  ImportExcel(const char * szFilePath);

	sigslot::signal3<PatientInfo *, OperationType, int *>  sigAMPatient;           // add or modify a patient
	sigslot::signal2<PatientInfo *, OperationType >        sigAMPatientRet;        // success to add, modify or delete a patient

	sigslot::signal1< int *>                               sigInit;                // 初始化数据库（检查表是否存在）
	sigslot::signal1< int *>                               sigDeinit;              // 去初始化数据库
	static CBusiness *  GetInstance();

private:
	static CBusiness *  pInstance;
};


template <class T>
void ClearVector(std::vector<T> & v) {
	typedef std::vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}