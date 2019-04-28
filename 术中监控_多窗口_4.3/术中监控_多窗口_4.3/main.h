#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

#include "GridUI.h"
#include "DragDropUI.h"
#include "Launch.h"

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
	virtual LRESULT  OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

private:
	void   RemoveAllGrids();
	// ����gridsһҳ(��������"��һҳ", "��һҳ"����ʼ���ȵ�)
	void   RefreshGridsPage();
	// ����grids size
	void   RefreshGridsSize(int width, int height);
	void   RefreshGridsSize();
	// ��һҳ
	void   NextPage();
	// ��һҳ
	void   PrevPage();
	// ˫���¼�
	void   OnDbClick();
	// �ƶ����ӣ�����˳��
	void   MoveGrid(const POINT & pt);
	// ֹͣ�ƶ����ӣ�ȷ������λ��
	void   StopMoveGrid();
	// �����ƶ������У������ĸ���Ҫ����
	void   OnMoveGrid(const POINT & pt);
	// �϶����ӹ����У��ϵ���������ң����·�ҳ
	void   OnFlipPage();
	// ����ˡ��˵�����ť
	void   OnBtnMenu(TNotifyUI& msg);
	// ����ˡ����á�
	void   OnSetting();
	// ����ˡ����ڡ�
	void   OnAbout();
	// �����������
	void   SaveGrid(DWORD  dwIndex);
	// ɾ����������
	void   RemoveGridCfg(DWORD  dwIndex);
	// Ӳ���豸�䶯�������д��ڱ䶯
	void   CheckDevice();
	// ������״̬
	void   OnLaunchStatus(WPARAM wParam, LPARAM  lParam);

public:
	// ����������״̬֪ͨ
	void   OnLauchStatusNotify(CLmnSerialPort::PortStatus e);

private:	
	CDialogBuilderCallbackEx                    m_callback;
	DWORD                                       m_dwCurSelGridIndex;      // ��ǰѡ��ĸ������
	ENUM_GRID_STATUS                            m_eGridStatus;            // �����̬��������󻯵ĵ�������

	DuiLib::CTabLayoutUI *                      m_tabs;
	DuiLib::CTileLayoutUI *                     m_layMain;
	CGridUI *                                   m_pGrids[MAX_GRID_COUNT];
	CHorizontalLayoutUI *                       m_layPages;
	CButtonUI *                                 m_btnPrevPage;
	CButtonUI *                                 m_btnNextPage;

	/**** drag drop����  ****/
	int                                         m_nDgSourceIndex;         // drag drop ��ʼ�϶���grid index
	int                                         m_nDgDestIndex;           // drag drop �϶���Ŀ��grid index
	DWORD                                       m_dwDgStartTick;          // drag drop ��ʼ��ʱ��
	CControlUI *                                m_dragdropGrid;
	// ��Ӷ�ʱ������ɿ�ҳ��ĸ����϶�
	BOOL                                        m_bDragTimer;
	BOOL                                        m_bFlipPrevPage;

	// �˵���ť
	DuiLib::CButtonUI *                         m_btnMenu;

	/******* ״̬�� ******/
	CLabelUI *                                  m_lblBarTips;
	CLabelUI *                                  m_lblLaunchStatus;
};
