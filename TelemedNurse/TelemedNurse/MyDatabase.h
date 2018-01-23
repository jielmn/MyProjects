#pragma once

#include "sigslot.h"
#include "sqlite3.h"


class CMyDatabase : public sigslot::has_slots<> {
public:
	CMyDatabase();
	
	void OnInitDb(int * ret);
	void OnDeinitDb(int * ret);
	void OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret);

	int  GetAllPatients(std::vector<PatientInfo *> & vPatients);
	int  GetPatient(const char * szId, PatientInfo * pPatient);
	int  DeletePatient(const char * szId);
	int  GetPatientTags(const char * szId, std::vector<TagInfo *> & vTags);
	int  GetPatientByTag(const TagId * pTagId, PatientInfo * pPatient);
	int  BindingTag(const TagInfo * pTagInfo);
	int  DeleteTag(const TagId * pTagId);
	int  SaveTempData(std::vector<TagData*> & v);

	// ��ȡ�����ݵ�7���ڵ��¶�����
	// �¶����ݰ���ʱ������
	int  GetLatestTempData(const char * szPatientId, std::vector<TagData*> & vRet);
	// ��ȡ4�ܵ��¶�����
	int  GetTempData(const char * szPatientId, time_t tStartTime, std::vector<TagData*> & vRet);

	sigslot::signal3<PatientInfo *, OperationType, int *>  sigAMPatient;             // add or modify a patient
	sigslot::signal1< int *>                                     sigInitDb;          // ��ʼ�����ݿ⣨�����Ƿ���ڣ�
	sigslot::signal1< int *>                                     sigDeinitDb;        // ȥ��ʼ�����ݿ�
	static CMyDatabase *  GetInstance();

private:
	static CMyDatabase *  pInstance;

	sqlite3 *           m_db;
};