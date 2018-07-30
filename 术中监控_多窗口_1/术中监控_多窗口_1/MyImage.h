#pragma once

#include "common.h"
class CDuiFrameWnd;

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI(DuiLib::CPaintManagerUI *pManager, CDuiFrameWnd * pMainWnd);
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  AddTemp(DWORD dwTemp);
	void  MyInvalidate();
	void  SetState(int nNewState);
	void  SetIndex(int nIndex);
	void  OnChangeSkin();
	void  OnMyClick(const POINT * pPoint);
	void  SetRemark(DuiLib::CDuiString & strRemark);

private:
	DuiLib::CPaintManagerUI *    m_pManager;
	vector<TempData *>           m_vTempData;
	int                          m_nState;     // 0 С����״̬��1 ȫ��״̬
	BOOL                         m_bSetParentScrollPos;
	int                          m_nIndex;     // ���к�
	Pen                          m_temperature_pen;
	SolidBrush                   m_temperature_brush;
	CDuiFrameWnd *               m_pMainWnd;
	DWORD                        m_dwNextTempIndex;
	DWORD                        m_dwCurTempIndex;

private:
	int    CalcMinWidth();   // ����ͼ����Ҫ�Ŀ��
	void   DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS = DEFAULT_POINT_RADIUS);
};