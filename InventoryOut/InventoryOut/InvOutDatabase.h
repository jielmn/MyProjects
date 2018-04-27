#pragma once

#include "LmnOdbc.h"
#include "InvOutCommon.h"

class  CInvoutDatabase : public CLmnOdbc {
public:
	CInvoutDatabase();
	~CInvoutDatabase();

	// 登录
	int Login(const CLoginParam * pParam);

	// 添加经销商
	int AddAgency(const CAgencyParam * pParam);

private:
	

private:

};
