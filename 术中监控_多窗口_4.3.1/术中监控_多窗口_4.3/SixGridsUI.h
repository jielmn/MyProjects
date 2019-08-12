#pragma once
#include "common.h"

class CSixGridsUI : public CContainerUI
{
public:
	CSixGridsUI();
	~CSixGridsUI();
	void  SetMode(int nMode);
	void  SetValues(int nIndex, const char * szValue);
	CDuiString  GetValues(int nIndex);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CEditUI *              m_edits[6];
	CHorizontalLayoutUI *  m_top;
	CHorizontalLayoutUI *  m_bottom;
	int                    m_nMode;
};

class CSevenGridsUI : public CContainerUI
{
public:
	CSevenGridsUI();
	~CSevenGridsUI();
	void  SetMode(int nMode);
	void  SetNumberOnly(BOOL bOnly);
	void  SetWeekStr(CDuiString * pWeek, DWORD dwSize);
	void  SetFont(int nFont);
	void  SetValues(int nIndex,const char * szValue);	
	CDuiString  GetValues(int nIndex);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CEditUI *              m_edits[7];
	CLabelUI *             m_labels[7];
	CHorizontalLayoutUI *  m_top;
	CHorizontalLayoutUI *  m_bottom;
	int                    m_nMode;
	BOOL                   m_bNumberOnly;
	int                    m_nFont;
	CDuiString             m_week_days[7];
};


class CShiftUI : public CContainerUI
{
public:
	CShiftUI();
	~CShiftUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();

private:
	CEditUI *              m_edits[2];
};

class CMyDateUI : public  CDateTimeUI, INotifyUI {
public:
	CMyDateUI();
	~CMyDateUI();
	void SetMyTime(SYSTEMTIME* pst);

private:
	LPCTSTR GetClass() const;
	void Notify(TNotifyUI& msg);
	void DoInit();
};