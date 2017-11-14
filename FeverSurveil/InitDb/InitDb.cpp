// InitDb.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mysql.h"
#include "LmnContainer.h"
#include "LmnString.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"

void  LogError( MYSQL * pMysql) {
	DWORD dwErrno = mysql_errno(pMysql);
	const char * pErrorDescription = mysql_error(pMysql);
	fprintf( stderr, "error no = %lu, error description = %s \n", dwErrno, pErrorDescription);
}

int main()
{
	MYSQL mysql;
	mysql_init(&mysql);

	FileConfig cfg;
	cfg.Init("FeverSurveil.cfg");

	char szHost[256];
	char szUser[256];
	char szPwd[256];
	char szDatabase[256];
	DWORD dwPort;

	cfg.GetConfig("mysql host", szHost, sizeof(szHost));
	cfg.GetConfig("mysql user", szUser, sizeof(szUser));
	cfg.GetConfig("mysql pwd", szPwd, sizeof(szPwd));
	cfg.GetConfig("mysql database", szDatabase, sizeof(szDatabase));
	cfg.GetConfig("mysql port", dwPort);

	if (!mysql_real_connect(&mysql, szHost, szUser, szPwd, szDatabase, dwPort, 0, 0)) {
		LogError(&mysql);
		printf("failed to connect mysql(host=%s,usr=%s,pwd=%s,database=%s,port=%u) \n",szHost, szUser, szPwd, szDatabase, dwPort);
		getchar();
		return -1;
	}

	if (0 != mysql_set_character_set(&mysql, "GBK")) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	char buf[8192];
	strncpy_s(buf, "call proc_update_date();", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	strncpy_s(buf, "update gzqx set y = 70 where DATEDIFF(x, now()) = 0 and s = '1' ", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	

	strncpy_s(buf, "update areainfo set value=null,info=null,ivalue=0;", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	strncpy_s(buf, "UPDATE scattertest set value = 10, type = 2, info = name, ivalue = 0;", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	strncpy_s(buf, "UPDATE scattertest set value = 12, type = 3 where name like '毕节%';", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	strncpy_s(buf, "UPDATE scattertest set value = 12, type = 3 where name like '遵义%';", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	strncpy_s(buf, "UPDATE scattertest set value = 12, type = 3 where name like '贵阳%';", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}

	strncpy_s(buf, "delete from tableinfo where id >= 6;", sizeof(buf) - 1);
	if (0 != mysql_query(&mysql, buf)) {
		LogError(&mysql);
		getchar();
		return -1;
	}
	

	cfg.Deinit();
	mysql_close(&mysql);
    return 0;
}

