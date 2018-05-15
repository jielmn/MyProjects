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
		InvOutDbErr_Not_found,
		InvOutDbErr_Cannt_delete_foreign_key,
		InvOutDbErr_Small_pkg_Parent_pkg_Already_out,
	};

	// ��¼
	int Login(const CLoginParam * pParam, DuiLib::CDuiString & strLoginId, DuiLib::CDuiString & strLoginName );

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

	// ����Inv
	int   SaveInvOut(const CSaveInvOutParam * pParam);

	// ����ʱ���ѯ
	int   QueryByTime(const CQueryByTimeParam * pParam, std::vector<QueryByTimeItem*> & vRet);

	// ���ݴ��װ��ѯ
	int   QueryByBigPkg(const CQueryByBigPkgParam * pParam, std::vector<PkgItem*> & vBig, std::vector<PkgItem*> & vSmall );

	// ����С��װ��ѯ
	int   QueryBySmallPkg(const CQueryBySmallPkgParam * pParam, std::vector<PkgItem*> & vSmall);

private:
	

private:

};
