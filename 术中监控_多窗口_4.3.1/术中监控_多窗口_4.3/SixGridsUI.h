#pragma once
#include "common.h"

class CSixGridsUI : public CContainerUI
{
public:
	CSixGridsUI();
	~CSixGridsUI();
	void  SetMode(int nMode);
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

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CEditUI *              m_edits[7];
	CHorizontalLayoutUI *  m_top;
	CHorizontalLayoutUI *  m_bottom;
	int                    m_nMode;
};
