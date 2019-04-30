#pragma once
#include "common.h"

class CEditableButtonUI : public CContainerUI, INotifyUI
{
public:
	CEditableButtonUI();
	~CEditableButtonUI();
	virtual CDuiString GetText() const;
	void  SetText(LPCTSTR pstrText);
	void SetEnabled(bool bEnable = true);

private:
	LPCTSTR GetClass() const {
		return "EditableButton";
	}

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	
private:
	CDuiString    m_strMyText;
	DWORD         m_dwEditColor;
	DWORD         m_dwBtnColor;
	int           m_nBtnFont;
	int           m_nEdtFont;
	CDuiString    m_strHotImg;

	CButtonUI *   m_btn;
public:
	CEditUI *     m_edit;
private:
	BOOL          m_bInited;
};
