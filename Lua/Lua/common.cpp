#include <time.h>
#include "common.h"

CGlobalData  g_data;

lua_State* init_lua() {
	lua_State* L = luaL_newstate();  //创建Lua栈
	//lua_checkstack(L, 60);//修改Lua栈大小为60，防止在C和Lua之间传递大数据时，崩溃
	luaL_openlibs(L);     //运行Lua虚拟机
	return L;
}