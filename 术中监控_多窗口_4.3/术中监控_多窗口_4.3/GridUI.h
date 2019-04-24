#pragma once

#include "common.h"
#include "ReaderUI.h"

class CGridUI : public CContainerUI
{
public:
	CGridUI();
	~CGridUI();

	void  SetBedNo(DWORD dwIndex);
	// ˫�������ͼ�л�
	void  SwitchView();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	DuiLib::CTabLayoutUI *                      m_tabs;

	CButtonUI *                                 m_btnBedNo;
	CButtonUI *                                 m_btnBedNoM;
	DWORD                                       m_dwBedNo;

	CLabelUI *                                  m_lblReaderNo;
	DWORD                                       m_dwReaderNo;

	// �������ϼ�
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];
};
