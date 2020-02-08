#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	void  SaveSurTemp(CSaveSurTempParam * pParam);
	// �����ֳ��¶�����
	void  SaveHandTemp(CSaveHandTempParam * pParam);
	// ����TagID��ѯ�¶�����
	void  QueryTempByTag(const char * szTagId, std::vector<TempItem*> & vRet);
	// ����ע��
	void  SaveRemark(const CSaveRemarkParam * pParam);
	// ����TagID��ѯTag�Ĳ�������
	void  QueryTagPNameByTagId(const char * szTagId, char * szPName, DWORD  dwPNameLen);
	// ������������tag id
	void SaveLastSurTagId(const CSaveLastSurTagId * pParam);
	// ��ȡ���һ�ε�����TagId
	void GetAllLastSurTags(std::vector<LastSurTagItem *> & vLastSurTags);
	// ��ȡ���е��ֳ�Tag�¶�����
	void GetAllHandTagTempData(std::vector<HandTagResult *> & vHandTagRet);
	// ����Tag�ʹ����
	void TagBindingGrid(const CBindingTagGrid * pParam, std::string & old_tagid, int & old_grid_index);
	// ɾ�����ӵ��ֳ�Tag��
	void RemoveGridBinding(const CRemoveGridBindingParam * pParam);
	// ����Tag��patient name
	void SaveTagPName(const CSaveTagPNameParam * pParam);
	// ��ѯtag�󶨵�grid index
	int  QueryBindingIndexByTag(const char * szTagId);
	// ��ѯPatientInfo
	void QueryPatientInfo(const CQueryPatientInfoParam * pParam, PatientInfo * pRet, 
		std::vector<PatientEvent * > & vEvents);
	// ����info
	void SavePatientInfo(const CSavePatientInfoParam * pParam);
	// ����patient events
	void SavePatientEvents(const CSavePatientEventsParam * pParam);
	// ��ѯPatientData
	void QueryPatientData( const CQueryPatientDataParam * pParam, PatientData * pData, 
		                   DWORD dwSize, std::vector<TempItem *> & vTemp );
	// ����data
	void SavePatientData(const CSavePatientDataParam * pParam);
	// ɾ��tag
	void DelTag(const CDelTag * pParam);
	// �Ƿ��Ժ
	BOOL IsOutHospital(const char * szTagId);
	// ��ѯסԺ��Ϣ
	void QueryInHospital(const CQueryInHospital * pParam, std::vector<InHospitalItem * > & vRet);
	// ��ѯ��Ժ��Ϣ
	void QueryOutHospital(const CQueryOutHospital * pParam, std::vector<OutHospitalItem * > & vRet);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	BOOL  CreateTable(const char * szTableName, const char * szSql);
	// ɾ����ʱ���¶����ݣ�Tag����
	void  PruneOldData();
	// ƴ��Where�Ӿ�
	void  ConcatWhereClause(CDuiString & strClause, const CDuiString & strItem);
	// ��ѯ�¼���Ϣ
	void  QueryEventsByTag(const char * szTagId, InHospitalItem * pRet);
	void  QueryEventsByTag(const char * szTagId, OutHospitalItem * pRet);
	// ����config����İ汾��
	void  UpdateConfigVersion(BOOL bCreated);
	int   GetVersion();
	// ת��tag����
	void  ConvertTagNames();

public:
	// ����TagID��ѯ�󶨵�grid index(Return: 0 notfound, > 0 found)
	int  QueryBindingGridIndexByTagId(const char * szTagId);

	// ���ջ�ȡ���д�λ��
	void  GetAllCubeBeds(std::vector<CubeItem*> & vRet);
};