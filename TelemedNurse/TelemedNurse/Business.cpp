#include <windows.h>
#include "Business.h"

CBusiness *  CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;		
	}
	return pInstance;
}

CBusiness::CBusiness() {
	sigAMPatient.connect(this, &CBusiness::OnPatientEvent);
	sigInitDb.connect(this, &CBusiness::OnInitDb);
	sigDeinitDb.connect(this, &CBusiness::OnDeinitDb);
}

void  CBusiness::OnInitDb( ) {

}

void  CBusiness::OnDeinitDb() {

}

void  CBusiness::OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret) {
	if (0 == ret) {
		return;
	}

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

	*ret = 0;
}


const char * GetErrDescription(GlobalErrorNo e) {
	const char * szDescription = 0;
	switch (e) {
	case ERROR_OK:
		szDescription = "OK";
		break;
	case ERROR_PATIENT_ID_EMPTY:
		szDescription = "���˱��Ϊ��";
		break;
	case ERROR_PATIENT_INNO_EMPTY:
		szDescription = "����סԺ��Ϊ��";
		break;
	case ERROR_PATIENT_NAME_EMPTY:
		szDescription = "��������Ϊ��";
		break;
	case ERROR_PATIENT_AGE_WRONG_FORMAT:
		szDescription = "���������ʽ����";
		break;
	default:
		szDescription = "δ֪����";
	}
	return szDescription;
}