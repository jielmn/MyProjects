#pragma once

#include "LmnOdbc.h"
#include "InvOutCommon.h"

class  CInvoutDatabase : public CLmnOdbc {
public:
	CInvoutDatabase();
	~CInvoutDatabase();

	// ��¼
	int Login(const CLoginParam * pParam);

	// ��Ӿ�����
	int AddAgency(const CAgencyParam * pParam);

private:
	

private:

};
