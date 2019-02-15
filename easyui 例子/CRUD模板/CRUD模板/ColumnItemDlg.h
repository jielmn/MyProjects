#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CColumnItemWnd : public WindowImplBase
{
public:
	enum  ColumnType {
		STRING = 0,
		INT
	};

	CColumnItemWnd(BOOL  bAdd = TRUE);

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIColumnItemFrame"); }
	virtual CDuiString GetSkinFile() { return _T("ColumnItem.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void  Notify(TNotifyUI& msg);
	virtual void   InitWindow();

	static const char * ColumnType2String(ColumnType e);
	static ColumnType String2ColumnType(const char * szType);

private:
	BOOL    m_bAdd;                    // 是添加还是修改
	CEditUI *    m_name;
	CComboUI *   m_type;

public:
	char          m_szColumnName[64];
	ColumnType    m_eColumnType;

private:
	void    OnMyOk();
};