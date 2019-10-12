#pragma once

#include "common.h"

class CGridUI : public CContainerUI
{
public:
	CGridUI();
	~CGridUI();
	void SetIndex(int nIndex);
	int  GetIndex() const;
	void SetText(const char * szText);
	CDuiString GetText() const;

private:
	LPCTSTR GetClass() const {
		return "MyGrid";
	}

	void DoInit();
	void SetIndex();
	void SetText();

private:
	int          m_nIndex;
	CDuiString   m_strText;
	CLabelUI *   m_lblIndex;
	CTextUI *    m_textValue;
};
