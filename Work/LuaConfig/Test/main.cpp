#ifdef _DEBUG
#include "vld.h"
#endif

#include <stdio.h>
#include "LuaConfig.h"

void test_1() {

	CLuaValue m;
	m.SetString("123");
	m.SetString("456");

	CLuaTable * pTable = new CLuaTable;
	pTable->SetString("1", "abc");
	pTable->SetString("2", "xyz");

	CLuaTable * pSubTable = new CLuaTable;
	pSubTable->SetString("1_1", "pop");
	pSubTable->SetString("2_1", "gop");
	pSubTable->SetInt("3_1", 64);

	pTable->SetTable("3", pSubTable);

	const char * szKey = "1_1";
	printf("  %s:%s\n", szKey, pSubTable->GetString(szKey));
	szKey = "2_1";
	printf("  %s:%s\n", szKey, pSubTable->GetString(szKey));
	szKey = "3_1";
	printf("  %s:%d\n", szKey, pSubTable->GetInt(szKey));

	szKey = "1";
	printf("%s:%s\n", szKey, pTable->GetString(szKey));
	szKey = "2";
	printf("%s:%s\n", szKey, pTable->GetString(szKey));
	szKey = "3";
	printf("%s:%p\n", szKey, pTable->GetTable(szKey));
	printf("pSubTable = %p\n", pSubTable);

	delete pTable;
}

void test_2() {
	LuaCfgInit();
	CLuaTable * pTable = LoadLuaCfgFile("Lua\\b.lua");

	DWORD  len = pTable->Size();
	for (DWORD i = 0; i < len; i++) {
		const char * szKey = 0;
		CLuaValue * pValue = pTable->GetItemAt(i, &szKey);
		if ( pValue->GetType() == CLuaValue::Integer ) {
			printf("%s:%d\n", szKey, pValue->GetInt());
		}
		else if (pValue->GetType() == CLuaValue::String) {
			printf("%s:%s\n", szKey, pValue->GetString());
		}
		else if (pValue->GetType() == CLuaValue::Table) {
			printf("%s:%p\n", szKey, pValue->GetTable());
		}
	}

	int ret = SaveLuaCfgFile("Lua\\c.lua", pTable);

	if (pTable)
		delete pTable;
	LuaCfgDeinit();
}

int main() {
	//test_1();
	test_2();
	getchar();
	return 0;
}