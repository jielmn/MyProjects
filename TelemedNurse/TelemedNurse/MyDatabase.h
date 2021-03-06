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

	// 获取有数据的7天内的温度数据
	// 温度数据按照时间排序
	int  GetLatestTempData(const char * szPatientId, std::vector<TagData*> & vRet);
	// 获取4周的温度数据
	int  GetTempData(const char * szPatientId, time_t tStartTime, std::vector<TagData*> & vRet);

	sigslot::signal3<PatientInfo *, OperationType, int *>  sigAMPatient;             // add or modify a patient
	sigslot::signal1< int *>                                     sigInitDb;          // 初始化数据库（检查表是否存在）
	sigslot::signal1< int *>                                     sigDeinitDb;        // 去初始化数据库
	static CMyDatabase *  GetInstance();

private:
	static CMyDatabase *  pInstance;

	sqlite3 *           m_db;
};