#include "LuaConfig.h"
#include <assert.h>
#include "lua.hpp"
#include "LmnString.h"

static lua_State* s_L = 0;

CLuaValue::CLuaValue() {
	m_eType = String;
	m_pValue = 0;
}

CLuaValue::~CLuaValue() {
	Clear();
}

void CLuaValue::Clear() {
	if (m_pValue) {
		if (m_eType == String) {
			delete (std::string *)m_pValue;
		}
		else if (m_eType == Table) {
			delete (CLuaTable *)m_pValue;
		}
		else if (m_eType == Integer) {
			// do nothing
		}
		else if (m_eType == Boolean) {
			// do nothing
		}
		else {
			assert(0);
		}
		m_pValue = 0;
	}
	m_eType = String;
}

CLuaValue::ValueType CLuaValue::GetType() const {
	return m_eType;
}

const char * CLuaValue::GetString() const {
	if (m_eType == String) {
		std::string * pStr = (std::string *)m_pValue;
		return pStr->c_str();
	}
	else {
		return 0;
	}
}

int  CLuaValue::GetInt() const {
	if ( m_eType == Integer ) {
		return (int)m_pValue;
	}
	else {
		return 0;
	}
}

BOOL  CLuaValue::GetBoolean() const {
	if (m_eType == Boolean) {
		return (BOOL)m_pValue;
	}
	else {
		return 0;
	}
}

CLuaTable * CLuaValue::GetTable() {
	if ( m_eType == Table ) {
		return (CLuaTable *)m_pValue;
	}
	else {
		return 0;
	}
}

void  CLuaValue::SetString(const char * szValue) {
	Clear();
	m_eType = String;
	m_pValue = new std::string(szValue);
}

void CLuaValue::SetInt(int nValue) {
	Clear();
	m_eType = Integer;
	m_pValue = (void *)nValue;
}

void  CLuaValue::SetBoolean(BOOL bValue) {
	Clear();
	m_eType = Boolean;
	m_pValue = (void *)bValue;
}

void CLuaValue::SetTable(const CLuaTable * pTable) {
	Clear();
	m_eType = Table;
	m_pValue = (void *)pTable;
}


CLuaTable::CLuaTable() {

}

CLuaTable::~CLuaTable() {
	Clear();
}

void CLuaTable::Clear() {
	std::map<std::string, CLuaValue *>::const_iterator it;
	for (it = m_table.begin(); it != m_table.end(); ++it) {
		CLuaValue * pValue = it->second;
		delete pValue;
	}
	m_table.clear();
}

const char * CLuaTable::GetString(const char * szKey) const {
	if (0 == szKey)
		return 0;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end())
		return 0;

	CLuaValue * pValue = it->second;
	if ( pValue && pValue->GetType() == CLuaValue::String ) {
		return pValue->GetString();
	}
	else {
		return 0;
	}
}

int CLuaTable::GetInt(const char * szKey) const {
	if (0 == szKey)
		return 0;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end())
		return 0;

	CLuaValue * pValue = it->second;
	if (pValue && pValue->GetType() == CLuaValue::Integer) {
		return pValue->GetInt();
	}
	else {
		return 0;
	}
}

CLuaTable *  CLuaTable::GetTable(const char * szKey) {
	if (0 == szKey)
		return 0;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end())
		return 0;

	CLuaValue * pValue = it->second;
	if (pValue && pValue->GetType() == CLuaValue::Table) {
		return pValue->GetTable();
	}
	else {
		return 0;
	}
}

void CLuaTable::SetString(const char * szKey, const char * szValue) {
	if (0 == szKey)
		return;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end()) {
		CLuaValue * pValue = new CLuaValue;
		pValue->SetString(szValue);
		m_table.insert( std::pair<std::string, CLuaValue *>(szKey, pValue) );
		return;
	}

	CLuaValue * pValue = it->second;
	pValue->SetString(szValue);
}

void CLuaTable::SetInt(const char * szKey, int nValue) {
	if (0 == szKey)
		return;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end()) {
		CLuaValue * pValue = new CLuaValue;
		pValue->SetInt(nValue);
		m_table.insert(std::pair<std::string, CLuaValue *>(szKey, pValue));
		return;
	}

	CLuaValue * pValue = it->second;
	pValue->SetInt(nValue);
}

void CLuaTable::SetBoolean(const char * szKey, BOOL bValue) {
	if (0 == szKey)
		return;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end()) {
		CLuaValue * pValue = new CLuaValue;
		pValue->SetBoolean(bValue);
		m_table.insert(std::pair<std::string, CLuaValue *>(szKey, pValue));
		return;
	}

	CLuaValue * pValue = it->second;
	pValue->SetBoolean(bValue);
}

void CLuaTable::SetTable(const char * szKey, const CLuaTable * pTable) {
	if (0 == szKey)
		return;

	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end()) {
		CLuaValue * pValue = new CLuaValue;
		pValue->SetTable(pTable);
		m_table.insert(std::pair<std::string, CLuaValue *>(szKey, pValue));
		return;
	}

	CLuaValue * pValue = it->second;
	pValue->SetTable(pTable);
}

void CLuaTable::SetString(int nKey, const char * szValue) {
	char szKey[32];
	SNPRINTF(szKey, sizeof(szKey), "%d", nKey);
	SetString(szKey, szValue);
}

void CLuaTable::SetInt(int nKey, int nValue) {
	char szKey[32];
	SNPRINTF(szKey, sizeof(szKey), "%d", nKey);
	SetInt(szKey, nValue);
}

void CLuaTable::SetBoolean(int nKey, BOOL bValue) {
	char szKey[32];
	SNPRINTF(szKey, sizeof(szKey), "%d", nKey);
	SetBoolean(szKey, bValue);
}

void CLuaTable::SetTable(int nKey, const CLuaTable * pTable) {
	char szKey[32];
	SNPRINTF(szKey, sizeof(szKey), "%d", nKey);
	SetTable(szKey, pTable);
}

BOOL  CLuaTable::HasKey(const char * szKey) {
	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end())
		return FALSE;
	else
		return TRUE;
}

void  CLuaTable::RemoveKey(const char * szKey) {
	std::map<std::string, CLuaValue *>::const_iterator it;
	it = m_table.find(szKey);
	if (it == m_table.end())
		return;
	
	delete it->second;
	m_table.erase(it);
}

DWORD  CLuaTable::Size() const {
	return m_table.size();
}

CLuaValue * CLuaTable::GetItemAt(DWORD dwIndex, const char ** ppKey /*= 0*/) {
	if (dwIndex >= Size()) {
		if (ppKey) {
			*ppKey = 0;
		}
		return 0;
	}

	std::map<std::string, CLuaValue *>::const_iterator it;
	DWORD i = 0;
	for (it = m_table.begin(); it != m_table.end(); ++it, i++) {
		if (i == dwIndex) {
			if (ppKey) {
				*ppKey = it->first.c_str();
			}
			return it->second;
		}
	}

	if (ppKey) {
		*ppKey = 0;
	}
	return 0;
}

static lua_State* init_lua() {
	lua_State* L = luaL_newstate();  //创建Lua栈
	//lua_checkstack(L, 60);         //修改Lua栈大小为60，防止在C和Lua之间传递大数据时，崩溃
	luaL_openlibs(L);                //运行Lua虚拟机
	return L;
}

int   LuaCfgInit() {
	s_L = init_lua();
	if (0 == s_L)
		return -1;
	return 0;
}

void  LuaCfgDeinit() {

}

CLuaTable * LoadLuaCfgFile(const char * szFilePath) {
	FILE * fp = fopen(szFilePath, "rb");
	if (0 == fp)
		return 0;
	
	fseek(fp, 0, SEEK_END);
	long ret = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char * buf = new char[ret + 1];
	memset(buf, 0, ret + 1);
	fread(buf, 1, ret, fp);

	fclose(fp);

	CLuaTable * pTable = LoadLuaCfgString(buf);
	delete[] buf;
	return pTable;
}

static void LoadLuaTable(lua_State* L, CLuaTable * pTable) {
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		int nValueType = lua_type(L, -1);
		int nKeyType   = lua_type(L, -2);

		char szKey[256] = { 0 };
		if ( nKeyType == LUA_TNUMBER ) {
			int nKey = (int)lua_tonumber(L, -2);
			SNPRINTF(szKey, sizeof(szKey), "%d", nKey);
		}
		else if (nKeyType == LUA_TSTRING) {
			size_t key_len = 0;
			const char * sKey = lua_tolstring(L, -2, &key_len);
			STRNCPY(szKey, sKey, sizeof(szKey) );
		}
		else {
			assert(0);
			lua_pop(L, 1);
			continue;
		}

		if ( nValueType == LUA_TNIL ) {
			pTable->SetInt(szKey, 0);
		}
		else if (nValueType == LUA_TBOOLEAN) {
			pTable->SetBoolean(szKey, (BOOL)lua_toboolean(L, -1));
		}
		else if (nValueType == LUA_TNUMBER) {
			pTable->SetInt(szKey, (int)lua_tonumber(L, -1));
		}
		else if (nValueType == LUA_TSTRING) {
			size_t value_len = 0;
			const char * sValue = lua_tolstring(L, -1, &value_len);
			pTable->SetString(szKey, sValue);
		}
		else if (nValueType == LUA_TTABLE) {
			CLuaTable * pSubTable = new CLuaTable;
			LoadLuaTable(L, pSubTable);
			pTable->SetTable(szKey, pSubTable);
		}
		lua_pop(L, 1);
	}
}

CLuaTable * LoadLuaCfgString(const char * szLua) {
	if (0 == szLua)
		return 0;

	if ( s_L == 0 )
		return 0;

	std::string s = "return ";
	s += szLua;
	luaL_loadstring(s_L, s.c_str());
	int ret = lua_pcall(s_L, 0, LUA_MULTRET, 0);
	// lua字符串有误
	if (0 != ret) {
		return 0;
	}

	int nTop = lua_gettop(s_L);
	if (nTop != 1) {
		return 0;
	}

	int nType = lua_type(s_L, -1);
	// 如果不是table
	if (nType != LUA_TTABLE) {
		return 0;
	}

	CLuaTable * pTable = new CLuaTable;
	LoadLuaTable(s_L, pTable);

	return pTable;
}

static BOOL IsSimpleKey(const char * szKey) {
	size_t len = strlen(szKey);
	for (size_t i = 0; i < len; i++) {
		if ( (szKey[i] >= 'a' && szKey[i] <= 'z') || (szKey[i] >= 'A' && szKey[i] <= 'Z') ) {
			continue;
		}
		else {
			return FALSE;
		}
	}
	return TRUE;
}

static  char * ConvertString(char * buf, DWORD dwBufSize, const char * source) {
	char buf_1[8192];
	char buf_2[8192];
	StrReplaceAll(buf_1, sizeof(buf_1), source,  "\n", "\\n");
	StrReplaceAll(buf_2, sizeof(buf_2), buf_1,   "\t", "\\t");
	StrReplaceAll(buf,   dwBufSize,     buf_2,   "\"", "\\\"");
	return buf;
}

static void Indent(FILE * fp, int nDepth) {
	for (int j = 0; j < nDepth + 1; j++) {
		fwrite("\t", 1, 1, fp);
	}
}

static int WriteLuaTable(CLuaTable * pTable, FILE * fp, int nDepth) {
	assert(pTable && fp);
	DWORD  dwSize = pTable->Size();
	char buf[8192];
	for (DWORD i = 0; i < dwSize; i++) {
		const char * key = 0;
		CLuaValue * pValue = pTable->GetItemAt(i, &key);
		CLuaValue::ValueType eType =  pValue->GetType();	

		Indent(fp, nDepth);

		int nKey = 0;
		BOOL bKeyIsInt = (0 == Str2Int(key, &nKey) ? TRUE : FALSE);
		BOOL bSimpleKey = IsSimpleKey(key);

		char szKey[256];
		// 数字key
		if (bKeyIsInt) {
			SNPRINTF(szKey, sizeof(szKey), "[%d]", nKey);
		}
		// 简单字符key
		else if (bSimpleKey) {
			STRNCPY(szKey, key, sizeof(szKey));
		}
		// 复杂字符key
		else {
			ConvertString(buf, sizeof(buf), key);
			SNPRINTF(szKey, sizeof(szKey), "[\"%s\"]", buf);
		}

		if (eType == CLuaValue::Integer) {
			SNPRINTF( buf, sizeof(buf), "%s=%d,\r\n", szKey, pValue->GetInt() );
			fwrite(buf, 1, strlen(buf), fp);
		}
		else if (eType == CLuaValue::Boolean) {
			SNPRINTF(buf, sizeof(buf), "%s=%s,\r\n", szKey, pValue->GetBoolean() ? "true" : "false" );
			fwrite(buf, 1, strlen(buf), fp);
		}
		else if (eType == CLuaValue::String) {
			char szValue[8192];
			ConvertString(szValue, sizeof(szValue), pValue->GetString());
			SNPRINTF(buf, sizeof(buf), "%s=\"%s\",\r\n", szKey, szValue);
			fwrite(buf, 1, strlen(buf), fp);
		}
		else if (eType == CLuaValue::Table) {
			SNPRINTF(buf, sizeof(buf), "%s={\r\n", szKey);
			fwrite(buf, 1, strlen(buf), fp);
			WriteLuaTable(pValue->GetTable(), fp, nDepth + 1);
			Indent(fp, nDepth);
			fwrite("},\r\n", 1, 4, fp);
		}
		else {
			assert(0);
		}
	}
	return 0;
}

int SaveLuaCfgFile(const char * szFilePath, CLuaTable * pTable) {
	if ( 0 == szFilePath || 0 == pTable ) {
		return -1;
	}

	FILE * fp = fopen(szFilePath, "wb");
	if (0 == fp) {
		return -1;
	}

	fwrite("{\r\n", 1, 3, fp);

	WriteLuaTable(pTable, fp, 0);

	fwrite("}\r\n", 1, 3, fp);

	fclose(fp);
	return 0;
}