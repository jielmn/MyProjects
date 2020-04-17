#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	BOOL AddClass(const CAddClassParam * pParam);
	void GetAllClasses(std::vector<DWORD> & vRet, std::map<std::string, DWORD> & BindingTags);
	void GetRoomData(const CGetRoomParam * pParam, std::vector<DeskItem*> & vRet);
	void ModifyDesk(const CModifyDeskParam * pParam);
	void EmptyDesk(const CEmptyDeskParam * pParam);
	void DeleteClass(const CDeleteClassParam * pParam);
	void ExchangeDesk(const CExchangeDeskParam * pParam, DeskItem & desk1, DeskItem & desk2, std::map<std::string, DWORD> & BindingTags);
	void BindingTag2Desk(const CBindingTagParam * pParam, std::map<std::string, DWORD> & BindingTags);
	void SaveTemp(DWORD dwGreat, const TempItem * pItem);
	void DisableBinding(const CDisableBindingTagParam * pParam);
	void Upgrade();

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};