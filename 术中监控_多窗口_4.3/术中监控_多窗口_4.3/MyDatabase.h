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
	void TagBindingGrid(const CBindingTagGrid * pParam, std::string & old_tagid);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	void  CreateTable(const char * szTableName, const char * szSql);
	// ɾ����ʱ���¶����ݣ�Tag����
	void  PruneOldData();
	// ����TagID��ѯ�󶨵�grid index(Return: 0 notfound, > 0 found)
	int  QueryBindingGridIndexByTagId(const char * szTagId);
};