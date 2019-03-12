#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	int  SaveTemp(const CSaveTempSqliteParam * pParam);
	int  QueryTempByTag(const char * szTagId, std::vector<TempData*> & vRet);
	int  SaveRemark(const CSetRemarkSqliteParam * pParam);
	int  QueryHandReaderTemp( vector< vector<TempData *> * > & vData, 
		                      vector< string * >  & vTagId, vector< string * > & vTagName );

private:
	sqlite3 *           m_db;
};