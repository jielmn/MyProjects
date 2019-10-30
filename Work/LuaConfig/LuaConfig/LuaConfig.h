#pragma once

#include <string>
#include <map>
#include "LmnCommon.h"

class CLuaTable;

class CLuaValue {
public:
	enum  ValueType {
		String = 0,
		Integer,
		Boolean,
		Table,
		End,
	};

	CLuaValue();
	~CLuaValue();

	ValueType GetType() const;
	const char * GetString() const;
	int  GetInt() const;
	BOOL GetBoolean() const;
	CLuaTable * GetTable();

	void  SetString(const char * szValue);
	void  SetInt(int nValue);
	void  SetBoolean(BOOL bValue);
	void  SetTable(const CLuaTable * pTable);

private:
	ValueType    m_eType;
	void *       m_pValue;

	void Clear();
};

class CLuaTable {
public:
	CLuaTable();
	~CLuaTable();

	const char * GetString(const char * szKey) const;
	int GetInt(const char * szKey) const;
	CLuaTable *  GetTable(const char * szKey);
	
	void SetString(const char * szKey, const char * szValue);
	void SetInt(const char * szKey, int nValue);
	void SetBoolean(const char * szKey, BOOL bValue);
	void SetTable(const char * szKey, const CLuaTable * pTable);

	void SetString(int nKey, const char * szValue);
	void SetInt(int nKey, int nValue);
	void SetBoolean(int nKey, BOOL bValue);
	void SetTable(int nKey, const CLuaTable * pTable);

	BOOL  HasKey(const char * szKey);
	void  RemoveKey(const char * szKey);

	// ”√”⁄±È¿˙
	DWORD  Size() const;
	CLuaValue * GetItemAt ( DWORD dwIndex, const char ** ppKey = 0 );

private:
	std::map<std::string, CLuaValue *>  m_table;

	void Clear();
};

int   LuaCfgInit();
void  LuaCfgDeinit();
CLuaTable * LoadLuaCfgFile(const char * szFilePath);
CLuaTable * LoadLuaCfgString(const char * szLua);
int SaveLuaCfgFile(const char * szFilePath, CLuaTable * pTable);

class CLuaCfg {
public:
	CLuaTable * Init(const char * szCfgFileName);
	void DeInit();

private:
	CLuaTable *   m_pRootTable;
};