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
		Unknown,
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

	const char * GetString(const char * szKey, const char * szDefault = "") const;
	int  GetInt(const char * szKey, int nDefault = 0) const;
	BOOL GetBoolean(const char * szKey, BOOL bDefault = FALSE) const;
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
	CLuaValue::ValueType GetType(const char * szKey) const;

	// 用于遍历
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
	CLuaCfg();
	~CLuaCfg();

	int  Init(const char * szCfgFileName);
	void DeInit();

	// path例如: a.1.b.x
	// 所以要求配置lua的key不要有关键'.'点号
	int  GetInt(const char * szPath, int nDefault = 0);
	BOOL GetBoolean(const char * szPath, BOOL bDefault = 0);
	const char * GetString(const char * szPath, const char * szDefault = "");

	void  SetInt(const char * szPath, int nValue);
	void  SetBoolean(const char * szPath, BOOL bValue);
	void  SetString(const char * szPath, const char * szValue);

	void  Save();
	void  ResetChangeFlag();

private:
	CLuaTable *   m_pRootTable;
	char          m_szFileName[256];
	BOOL          m_bChanged;
};