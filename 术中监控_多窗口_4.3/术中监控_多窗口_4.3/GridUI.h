#pragma once

#include "common.h"
#include "ReaderUI.h"
#include "ModeButtonUI.h"

class CGridUI : public CContainerUI, INotifyUI
{
public:
	CGridUI();
	~CGridUI();

	void  SetBedNo(DWORD dwIndex);
	void  SetMode(CModeButton::Mode e);

	// Ë«»÷ºóµÄÊÓÍ¼ÇÐ»»
	void  SwitchView();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnModeChanged();

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	DuiLib::CTabLayoutUI *                      m_tabs;

	CButtonUI *                                 m_btnBedNo;
	CButtonUI *                                 m_btnBedNoM;
	DWORD                                       m_dwBedNo;

	CLabelUI *                                  m_lblReaderNo;         // A ~ F
	DWORD                                       m_dwReaderNo;

	CModeButton *                               m_cstModeBtn;

	CControlUI *                                m_CurReaderState;

	// ¶Á¿¨Æ÷ºÏ¼¯
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];             // Á¬Ðø²âÎÂ¶Á¿¨Æ÷
	CReaderUI *                                 m_hand_reader;                               // ·ÇÁ¬Ðø²âÎÂ¶Á¿¨Æ÷
};
