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

	// 登录
	int Login(const CLoginParam * pParam, DuiLib::CDuiString & strLoginId, DuiLib::CDuiString & strLoginName );

	// 添加经销商
	int AddAgency(const CAgencyParam * pParam, DWORD & dwNewId );

	// 获取所有的经销商
	int  GetAllAgency( std::vector<AgencyItem *> & vRet );

	// 修改经销商
	int  ModifyAgency(const CAgencyParam * pParam);

	// 删除经销商
	int  DeleteAgency(const CAgencyParam * pParam);

	// 获取所有的销售
	int  GetAllSales( std::vector<SaleStaff *> & vRet );

	// 保存Inv
	int   SaveInvOut(const CSaveInvOutParam * pParam);

	// 根据时间查询
	int   QueryByTime(const CQueryByTimeParam * pParam, std::vector<QueryByTimeItem*> & vRet);

	// 根据大包装查询
	int   QueryByBigPkg(const CQueryByBigPkgParam * pParam, std::vector<PkgItem*> & vBig, std::vector<PkgItem*> & vSmall );

	// 根据小包装查询
	int   QueryBySmallPkg(const CQueryBySmallPkgParam * pParam, std::vector<PkgItem*> & vSmall);

private:
	

private:

};
