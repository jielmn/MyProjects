#pragma once

#include "LmnOdbc.h"
#include "InvOutCommon.h"

class  CInvoutDatabase : public CLmnOdbc {
public:
	CInvoutDatabase();
	~CInvoutDatabase();

	// µÇÂ¼
	int Login(const CLoginParam * pParam);

private:
	

private:

};
