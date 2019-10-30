#ifdef _DEBUG
#include "vld.h"
#endif

#include <stdio.h>
#include <assert.h>
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
	CLuaTable * pTable = LoadLuaCfgFile("Lua\\a.lua");

	DWORD  len = pTable->Size();
	for (DWORD i = 0; i < len; i++) {
		const char * szKey = 0;
		CLuaValue * pValue = pTable->GetItemAt(i, &szKey);
		if ( pValue->GetType() == CLuaValue::Integer ) {
			printf("%s:%d\n", szKey, pValue->GetInt());
		}
		else if (pValue->GetType() == CLuaValue::Boolean) {
			printf("%s:%s\n", szKey, pValue->GetBoolean() ? "true" : "false" );
		}
		else if (pValue->GetType() == CLuaValue::String) {
			printf("%s:%s\n", szKey, pValue->GetString());
		}
		else if (pValue->GetType() == CLuaValue::Table) {
			printf("%s:%p\n", szKey, pValue->GetTable());
		}
	}

	int ret = SaveLuaCfgFile("Lua\\x.lua", pTable);

	if (pTable)
		delete pTable;
	LuaCfgDeinit();
}

void  test_3() {
	CLuaCfg cfg;
	int ret = cfg.Init("Lua\\c.lua");
	assert(0 == ret);
	printf("config\n");
	printf("------------------\n");
	printf("1=%d \n", cfg.GetInt("1"));
	printf("b=%s \n", cfg.GetString("b"));
	printf("d=%s \n", cfg.GetBoolean("d") ? "true" : "false" );
	printf("hello\n\"world\"=%s \n", cfg.GetString("hello\n\"world\""));
	printf("c.1=%d \n", cfg.GetInt("c.1"));
	printf("c.2=%s \n", cfg.GetString("c.2"));
	printf("c.1=%d \n", cfg.GetInt("c.1"));
	printf("c.m.p=%s \n", cfg.GetString("c.m.p"));
	printf("c.m.q=%s \n", cfg.GetString("c.m.q"));
	cfg.SetBoolean("c.m.a", TRUE);
	cfg.SetString("c.m.b", "a");

	cfg.ResetChangeFlag();
	cfg.DeInit();
}

void  test_4() {
	CLuaCfg cfg;
	int ret = cfg.Init("Lua\\d.lua");
	assert(0 == ret);
	
	cfg.SetInt("c.1", 10, 10);
	cfg.SetString("c.2", "xyz", "xyz");
	cfg.SetBoolean("c.m.p", TRUE, TRUE);
	cfg.SetString("c.m.q", "quit", "quit", TRUE);

	cfg.SetInt("1", 100, 100);
	cfg.SetString("b", "123", "123");
	cfg.SetBoolean("d", FALSE, FALSE, TRUE);

	//cfg.ResetChangeFlag();
	cfg.DeInit();
}

int main() {
	//test_1();
	//test_2();
	//test_3();
	test_4();
	getchar();
	return 0;
}