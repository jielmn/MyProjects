#pragma once

#include "common.h"
#include "MyImage.h"

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_FRAME_NAME); }
	virtual CDuiString GetSkinFile() { return _T(MAIN_XML_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual void     OnFinalMessage(HWND hWnd);

	// �����Զ�����Ϣ
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void   OnSize(WPARAM wParam, LPARAM lParam);
	void   OnChangeSkin();
	void   OnChangeState(int nGridIndex);
	void   OnBtnMenu(TNotifyUI& msg);
	void   OnBtnBedName(TNotifyUI& msg);
	void   OnEdtBedNameKillFocus(TNotifyUI& msg);
	void   OnBtnName(TNotifyUI& msg);
	void   OnGridReaderSwitch(TNotifyUI& msg);
	void   OnEdtNameKillFocus(TNotifyUI& msg);
	void   OnSetting();
	void   OnAbout();
	void   OnDbClick();
	void   OnGridMenu(TNotifyUI& msg);
	void   OnGridExpandOrRestore(DWORD nIndex);
	void   OnGridSaveExcel(TNotifyUI& msg);
	// ���²���
	void   ReLayout(DWORD dwWidth, DWORD dwHeight);
	// ���²���(�����ø���֮��)
	void   UpdateLayout();
	void   OnUpdateGridScroll(WPARAM wParam, LPARAM lParam);
	void   OnMyImageClick(const POINT * pPoint);
	
	// ���¶����ݴﵽ
	void   OnNewTempData(int nGridIndex, DWORD dwTemp);
	// ����
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);
	// launch status
	void   OnLaunchStatus(WPARAM wParam, LPARAM lParam);
	// bar tips
	void   OnBarTips(WPARAM wParam, LPARAM lParam);
	// Ӳ���Ķ�
	void   OnComPortsChanged(WPARAM wParam, LPARAM lParam);
	// �¶�����
	void   OnTempData(WPARAM wParam, LPARAM lParam);
	// ���ӵ�Reader״̬
	void   OnGridReaderStatus(WPARAM wParam, LPARAM lParam);
	void   OnGridReaderStatus(DWORD dwGridIndex, int   nStatus, DWORD  dwDelayTime);

	// ���Reader
	void   OnCheckReaderTimer(WPARAM wParam, LPARAM lParam);
	// 
	void   OnHeatBeatTick(WPARAM wParam, LPARAM lParam);
	//
	void   OnTempTick(WPARAM wParam, LPARAM lParam);
	//
	void   ReHandleReader( DWORD dwCount, DWORD * pOldIntervals, DWORD * pOldBedNo, DWORD  dwOldAreaNo, BOOL * pbOldGridReaderSwitch );
public:
	void   OnEdtRemarkKillFocus();

private:
	CDialogBuilderCallbackEx           m_callback;

	DuiLib::CVerticalLayoutUI *        m_layWindow;
	DuiLib::CTileLayoutUI *            m_layMain;
	DuiLib::CHorizontalLayoutUI *      m_layStatus;

	CControlUI *                       m_pGrids[MAX_GRID_COUNT];
	CHorizontalLayoutUI *              m_pLayFlex[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblIndexes_small[MAX_GRID_COUNT];
	//CLabelUI *                         m_pLblBed_small[MAX_GRID_COUNT];
	CButtonUI *                        m_pBtnBedName_small[MAX_GRID_COUNT];
	CEditUI *                          m_pEdtBedName_small[MAX_GRID_COUNT];
	//CLabelUI *                         m_pLblName_small[MAX_GRID_COUNT];
	CButtonUI *                        m_pBtnName_small[MAX_GRID_COUNT];
	CEditUI *                          m_pEdtName_small[MAX_GRID_COUNT];

	CLabelUI *                         m_pLblCurTemp_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblBedTitle_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblNameTitle_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblCurTempTitle_small[MAX_GRID_COUNT];
	CMyImageUI *                       m_pMyImage[MAX_GRID_COUNT];
	COptionUI *                        m_pOptGridReaderSwitch[MAX_GRID_COUNT];

	DuiLib::CButtonUI *                m_btnMenu;    // �˵���ť	
	CAlarmImageUI *                    m_pAlarmUI[MAX_GRID_COUNT];

	CLabelUI *                         m_lblLaunchStatus;
	CLabelUI *                         m_lblBarTips;

	int                                m_nState;     // �����״̬��������״̬
	int                                m_nMaxGridIndex; // ��󻯸��ӵ����	
public:
	DuiLib::CEditUI *                  m_edRemark;   // �༭ע�͵Ŀ�
};
