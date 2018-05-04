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

	// ��¼
	int Login(const CLoginParam * pParam);

	// ��Ӿ�����
	int AddAgency(const CAgencyParam * pParam, DWORD & dwNewId );

	// ��ȡ���еľ�����
	int  GetAllAgency( std::vector<AgencyItem *> & vRet );

	// �޸ľ�����
	int  ModifyAgency(const CAgencyParam * pParam);

	// ɾ��������
	int  DeleteAgency(const CAgencyParam * pParam);

	// ��ȡ���е�����
	int  GetAllSales( std::vector<SaleStaff *> & vRet );

private:
	

private:

};
