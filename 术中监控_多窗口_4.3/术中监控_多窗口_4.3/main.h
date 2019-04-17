#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

class CDuiFrameWnd : public WindowImplBase , public sigslot::has_slots<>
{
public:
	enum ENUM_GRID_STATUS {
		GRID_STATUS_GRIDS = 0,
		GRID_STATUS_MAXIUM
	};

	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
#endif

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void     OnFinalMessage(HWND hWnd);
	virtual LRESULT  OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void   RemoveAllGrids();
	// ����gridsһҳ(��������"��һҳ", "��һҳ"����ʼ���ȵ�)
	void   RefreshGridsPage();
	// ����grids size
	void   RefreshGridsSize(int width, int height);

private:
	CDialogBuilderCallbackEx                    m_callback;
	DWORD                                       m_dwCurGridIndex;         // ��ǰ�ĸ������
	ENUM_GRID_STATUS                            m_eGridStatus;            // �����̬��������󻯵ĵ�������

	DuiLib::CTabLayoutUI *                      m_tabs;
	DuiLib::CTileLayoutUI *                     m_layMain;
	CControlUI *                                m_pGrids[MAX_GRID_COUNT];
	CHorizontalLayoutUI *                       m_layPages;
	CButtonUI *                                 m_btnPrevPage;
	CButtonUI *                                 m_btnNextPage;
};
