#pragma once

#include "common.h"
#include "EditableButtonUI.h"

class CReaderUI : public CContainerUI, INotifyUI
{
public:
	enum ReaderType
	{
		Hand = 0,            // 手持Reader
		Surgency             // 术中Reader
	};

	CReaderUI(ReaderType e = Hand);
	~CReaderUI();

	void  SetIndicator(DWORD dwIndex);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnReaderSelected();
	void OnReaderNameChanged();
	DWORD GetGridIndex();
	DWORD GetReaderIndex();

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	CLabelUI *                                  m_lblIndicator;           // A~F指示器
	DWORD                                       m_dwIndicator;
	CControlUI *                                m_indicator;              // 颜色指示器
	ReaderType                                  m_type; 

public:
	COptionUI *                                 m_optSelected;            // 选择框
	CEditableButtonUI *                         m_cstBodyPart;            // 身体部位

private:
	CControlUI *                                m_state;                  // 断线状态
};
