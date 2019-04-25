#include "MyDatabase.h"

#define  DB_NAME                          "temp_monitor.db"

/******* 温度表 *****/
#define  TEMP_TABLE                       "temperature"
/*
	id         INTEGER        PRIMARY KEY     AUTOINCREMENT
	tag_id     CHAR(16)       NOT NULL
	temp       int            NOT NULL
	time       int            NOT NULL
	remark     varchar(28)    NOT NULL
	type       int            NOT NULL        DEFAULT 0            **** 0: 连续测温，1: 手持读卡器测温
*/

// 温度贴
#define  TAGS_TABLE                      "tags"
/*
	tag_id         CHAR(16)           NOT NULL           PRIMARY KEY
	patient_name   VARCHAR(16)        NOT NULL
	time           int                NOT NULL
*/

// 窗格绑定
#define  GRID_BINDING_TABLE              "bridbinding"
/*
	grid_index     int         NOT NULL        PRIMARY KEY
	tag_id         CHAR(16)    NOT NULL
*/

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if ( ret != 0 ) {
		return -1;
	}
	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}
