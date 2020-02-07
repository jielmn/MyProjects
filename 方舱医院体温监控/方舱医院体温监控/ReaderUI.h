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
	void  SetReaderStatus(BOOL bConnected);
	BOOL  IsConnected();
	// ��ʾ����״̬�µ���һ���¶�
	//void  SetDisconnectedTemp(DWORD dwTemp);
	// ��ʾ�¶�(����״̬�ͷǶ���״̬��һ��)
	void  SetTemp(DWORD dwTemp);
	CDuiString GetTemp() const;
	int   GetIntTemp() const;

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnReaderSwitch();
	void OnReaderNameChanged();
	CControlUI *  GetGridUI();

public:
	DWORD GetGridIndex();
	DWORD GetReaderIndex();

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	CLabelUI *                                  m_lblIndicator;           // A~Fָʾ��
	DWORD                                       m_dwIndicator;
	CControlUI *                                m_indicator;              // ��ɫָʾ��
	ReaderType                                  m_type; 
	BOOL                                        m_bConnected;             // ����״̬
	DWORD                                       m_dwTemp;

public:
	COptionUI *                                 m_optSelected;            // ѡ���
	CEditableButtonUI *                         m_cstBodyPart;            // ���岿λ
	CLabelUI *                                  m_lblTemp;                // �¶�����
	CControlUI *                                m_state;                  // ����״̬
	CLabelUI *                                  m_lblReaderId;            // ReaderId
	CLabelUI *                                  m_lblTagId;               // TagId
};
