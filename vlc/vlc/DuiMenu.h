#pragma once
#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	DuiLib::CDuiString  m_strXMLPath;
	DuiLib::CControlUI * m_pOwner;
	DWORD      m_dwParam0;
	DWORD      m_dwParam1;
	virtual void    InitWindow();

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, DuiLib::CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner), m_dwParam0(0), m_dwParam1(0) {}
	explicit CDuiMenu(LPCTSTR pszXMLPath, DuiLib::CControlUI * pOwner, DWORD wParam, LPARAM  lParam) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner), m_dwParam0(wParam), m_dwParam1(lParam) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(""); }
	virtual DuiLib::CDuiString GetSkinFile() { return m_strXMLPath; }
	virtual void       OnFinalMessage(HWND hWnd) { delete this; }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void Init(HWND hWndParent, POINT ptPos);
	virtual void  Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};