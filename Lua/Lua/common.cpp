#include <time.h>
#include "common.h"

CGlobalData  g_data;

lua_State* init_lua() {
	lua_State* L = luaL_newstate();  //����Luaջ
	//lua_checkstack(L, 60);//�޸�Luaջ��СΪ60����ֹ��C��Lua֮�䴫�ݴ�����ʱ������
	luaL_openlibs(L);     //����Lua�����
	return L;
}