#include <time.h>
#include "exhDatabase.h"
#include "exhCommon.h"

#define  TABLE_NAME "TEMP_DATA"

CMyDatabase *  CMyDatabase::pInstance = 0;

CMyDatabase *  CMyDatabase::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CMyDatabase;
	}
	return pInstance;
}
 

CMyDatabase::CMyDatabase() {
	m_db = 0;
}

void CMyDatabase::OnInitDb( ) {

	int ret = sqlite3_open("exh.db", &m_db);
	if (ret != 0) {
		ret = -1;
		return;
	}

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	const char * sql = 0;

	// 查看Patients表是否存在，如果不存在，则创建
	sql = "select name from sqlite_master where name = '" TABLE_NAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
	if (0 == nrow) {
		sql = "CREATE TABLE " TABLE_NAME "("  \
			"Temperature    int         NOT NULL," \
			"GenDate        DATETIME    NOT NULL" \
			");";

		ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (ret != 0) {
			ret = -1;
			sqlite3_free_table(azResult);
			return;
		}
	}
	sqlite3_free_table(azResult);
}

void CMyDatabase::OnDeinitDb( ) {

}

int  CMyDatabase::AddTemp(DWORD dwTemp, time_t tDate) {
	char sql[8192];
	char *zErrMsg = 0;

	char  date_buf[256];
	Date2String(date_buf, sizeof(date_buf), &tDate);

	_snprintf_s(sql, sizeof(sql), "INSERT INTO %s values( %lu,'%s' )", TABLE_NAME, dwTemp, date_buf);


	/* Execute SQL statement */
	int ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
	if (ret != 0) {
		ret = -1;
	}

	return ret;
}

int CMyDatabase::GetAllTemp(std::vector<TempData *> & v) {
#if TEST_FLAG
	InitRand( TRUE );

	time_t now = time(0);

	for (int i = 0; i < 30; i++) {
		TempData * p = new TempData;
		p->dwTemperature = GetRand(2000, 5000);
		p->tTime = now + i*20;
		v.push_back(p);
	}
#else
	ClearVector( v );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	const char * sql = 0;

	char buf[8192];

	sql = "select * from " TABLE_NAME " order by GenDate;";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {
		TempData * pItem = new TempData;
		memset(pItem, 0, sizeof(TempData));

		sscanf_s(azResult[(i + 1)*ncolumn + 0], "%lu", &pItem->dwTemperature);
		strncpy_s(buf, azResult[(i + 1)*ncolumn + 1], sizeof(buf));
		pItem->tTime = String2Date(buf);

		v.push_back(pItem);
	}
	sqlite3_free_table(azResult);

#endif
	return 0;
}