#pragma once

#include <vector>
#include "LmnOdbc.h"
#include "InvOutCommon.h"

class  CInvoutDatabase : public CLmnOdbc {
public:
	CInvoutDatabase();
	~CInvoutDatabase();

	enum InvOutDbErr
	{
		InvOutDbErr_Integrity_constraint_violation = 100,
	};

	// 登录
	int Login(const CLoginParam * pParam);

	// 添加经销商
	int AddAgency(const CAgencyParam * pParam, DWORD & dwNewId );

	// 获取所有的经销商
	int  GetAllAgency( std::vector<AgencyItem *> & vRet );

private:
	

private:

};
