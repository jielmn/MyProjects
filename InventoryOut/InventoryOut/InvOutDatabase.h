#pragma once

#include "LmnOdbc.h"
#include "InvOutCommon.h"

class  CInvoutDatabase : public CLmnOdbc {
public:
	CInvoutDatabase();
	~CInvoutDatabase();

	// ��¼
	int Login(const CLoginParam * pParam);

private:
	

private:

};
