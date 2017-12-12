#pragma once

#include "sigslot.h"

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
	OPERATION_END,
};

enum  GlobalErrorNo {
	ERROR_OK = 0,
	ERROR_PATIENT_ID_EMPTY,
	ERROR_PATIENT_INNO_EMPTY,
	ERROR_PATIENT_NAME_EMPTY,
	ERROR_PATIENT_AGE_WRONG_FORMAT,
};

const char * GetErrDescription(GlobalErrorNo e);

class CBusiness : public sigslot::has_slots<> {
public:
	CBusiness();
	void OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret);
	void OnInitDb();
	void OnDeinitDb();

	sigslot::signal3<PatientInfo *, OperationType, int *>  sigAMPatient;       // add or modify a patient
	sigslot::signal0<>                                     sigInitDb;          // 初始化数据库（检查表是否存在）
	sigslot::signal0<>                                     sigDeinitDb;        // 去初始化数据库
	static CBusiness *  GetInstance();

private:
	static CBusiness *  pInstance;
};