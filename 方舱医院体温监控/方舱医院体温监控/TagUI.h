#pragma once

#include "common.h"
#include "EditableButtonUI.h"

class CTagUI : public CContainerUI, INotifyUI
{
public:
	CTagUI();
	~CTagUI();
	void   OnHandTemp(const TempItem * pItem,const char * szName);
	void   DoEvent(DuiLib::TEventUI& event);
	CDuiString GetTagId();
	void   SetBindingGridIndex(int nIndex);
	int    GetBindingGridIndex();
	CDuiString GetPTagName();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnTagNameChanged();

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;

	CLabelUI *                                  m_lblReaderId;
	CLabelUI *                                  m_lblTagId;
	CLabelUI *                                  m_lblTempTime;	
	CLabelUI *                                  m_lblTemp;
	CLabelUI *                                  m_lblBinding;                // �󶨵��ĸ����� 

public:
	TempItem                                    m_item;
	int                                         m_nBindingGridIndex;

	CEditableButtonUI  *                        m_cstPatientName;
};
