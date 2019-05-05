#pragma once

#include "common.h"
#include "ReaderUI.h"
#include "ModeButtonUI.h"
#include "MyImageUI.h"

class CGridUI : public CContainerUI, INotifyUI
{
public:
	CGridUI();
	~CGridUI();

	void  SetBedNo(DWORD dwIndex);
	void  SetMode(CModeButton::Mode e);

	// ˫�������ͼ�л�
	void  SwitchView();

	// �������ж�����������״̬
	void  SetSurReaderStatus(DWORD dwSubIndex, BOOL bConnected);

	// ���ж������¶�
	void  OnSurReaderTemp(DWORD dwSubIndex, const TempItem & item);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnModeChanged();
	void SetCurReaderStatus(BOOL bConnected);
	void SetCurReaderTemp(DWORD  dwTemp);
	void SetReaderTemp( DWORD dwSubIndex, DWORD  dwTemp);

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	DuiLib::CTabLayoutUI *                      m_tabs;

	CButtonUI *                                 m_btnBedNo;
	CButtonUI *                                 m_btnBedNoM;
	DWORD                                       m_dwBedNo;

	CLabelUI *                                  m_lblReaderNo;         // A ~ F
	DWORD                                       m_dwReaderNo;

	// ����ģʽ��ť
	CModeButton *                               m_cstModeBtn;

	// ��ǰѡ�ж�����������״̬
	CControlUI *                                m_CurReaderState;

	// ��ʾ�¶����ݺ����ߵ�
	CImageLabelUI *                             m_cstImgLabel;

	// �������ϼ�
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];             // �������¶�����
	CReaderUI *                                 m_hand_reader;                               // ���������¶�����

private:
	// ���һ�ε��¶�����
	TempItem              m_aLastTemp[MAX_READERS_PER_GRID];
	// ��ǰѡ�е����ж�����index(��1��ʼ)
	DWORD                 m_dwSelSurReaderIndex;
};
