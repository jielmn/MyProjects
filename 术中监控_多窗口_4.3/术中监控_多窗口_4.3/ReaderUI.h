#pragma once

#include "common.h"
#include "EditableButtonUI.h"

class CReaderUI : public CContainerUI, INotifyUI
{
public:
	enum ReaderType
	{
		Hand = 0,            // �ֳ�Reader
		Surgency             // ����Reader
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
	CLabelUI *                                  m_lblIndicator;           // A~Fָʾ��
	DWORD                                       m_dwIndicator;
	CControlUI *                                m_indicator;              // ��ɫָʾ��
	ReaderType                                  m_type; 

public:
	COptionUI *                                 m_optSelected;            // ѡ���
	CEditableButtonUI *                         m_cstBodyPart;            // ���岿λ

private:
	CControlUI *                                m_state;                  // ����״̬
};
