#pragma once

#include <vector>
#include "sigslot.h"
#include "sqlite3.h"
#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"

extern IConfig *  g_cfg;
extern ILog *     g_log;

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

typedef struct tagTagId {
	BYTE     Id[8];
}TagId;

typedef struct tagTagInfo {
	TagId    tagId;
	char     szPatientId[32];
	time_t   tBindingDate;
}TagInfo;

// �¶�������
typedef struct tagTagData {
	time_t     tTime;                  // ʱ��
	TagId      tTagId;                 // id
	DWORD      dwTemperature;          // �¶����ݣ���3500��ʾ35.00
}TagData;

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
	ERROR_FAILED_TO_LOAD_FUNCTION_DLL,
	ERROR_NOT_FOUND_TAG,
	ERROR_BINDING_READER_FAILED_TO_INVENTORY,
	ERROR_BINDING_READER_FAILED_TOO_MANY_CARDS,
};

const char * GetErrDescription(GlobalErrorNo e);
const char * GetSex(BOOL bMale);
char * GetInt(char * buf, DWORD dwSize, int n);
char * ConvertDate(char * szDest, DWORD dwDestSize, time_t * t);
char * ConvertDateTime(char * szDest, DWORD dwDestSize, const time_t * t);
time_t  ConvertDateTime(const char * szDatetime);
CString GetExcelDriver();
char *  ConvertTagId( char * buf, DWORD dwBufSize, const TagId * pTagId);
int     ConvertTagId( TagId * pTagId, const char * szTagId );

class CBusiness : public sigslot::has_slots<> {
public:
	CBusiness();
	void OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret);
	void OnInit( int * ret );
	void OnDeinit(int * ret );
	int  InitBindingReader();
	
	int  GetAllPatients( std::vector<PatientInfo *> & vPatients );
	int  GetPatient(const char * szId, PatientInfo * pPatient);
	int  DeletePatient(const char * szId);
	int  ImportExcel(const char * szFilePath);
	int  GetPatientTags(const char * szId, std::vector<TagInfo *> & vTags );
	int  GetPatientByTag(const TagId * pTagId, PatientInfo * pPatient );
	int  BindingTag(const TagInfo * pTagInfo);
	int  DeleteTag(const TagId * pTagId);

	sigslot::signal3<PatientInfo *, OperationType, int *>  sigAMPatient;           // add or modify a patient
	sigslot::signal2<PatientInfo *, OperationType >        sigAMPatientRet;        // success to add, modify or delete a patient

	sigslot::signal1< int *>                               sigInit;                // ��ʼ�����ݿ⣨�����Ƿ���ڣ�
	sigslot::signal1< int *>                               sigDeinit;              // ȥ��ʼ�����ݿ�
	sigslot::signal1<TagInfo *>                            sigBinding;             // ��Tag�ɹ�
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