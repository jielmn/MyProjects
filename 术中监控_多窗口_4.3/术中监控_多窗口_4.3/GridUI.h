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

	// ѡ�ж�����
	void  OnSurReaderSelected(DWORD  dwSubIndex);

	// �����¶�����
	void SetFont(int index);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnModeChanged();
	void SetCurReaderTemp(DWORD  dwTemp, DWORD dwHighAlarm, DWORD dwLowAlarm);
	void SetReaderTemp( DWORD dwSubIndex, DWORD  dwTemp,DWORD dwHighAlarm,DWORD dwLowAlarm);
	// ������ʾ��ȥ��ʱ��
	void UpdateElapsed();

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	DuiLib::CTabLayoutUI *                      m_tabs;

	CButtonUI *                                 m_btnBedNo;
	CButtonUI *                                 m_btnBedNoM;
	DWORD                                       m_dwBedNo;

	CLabelUI *                                  m_lblReaderNo;         // A ~ F

	// ����ģʽ��ť
	CModeButton *                               m_cstModeBtn;

	// ��ǰѡ�ж�����������״̬
	CControlUI *                                m_CurReaderState;

	// ��ʾ�¶����ݺ����ߵ�
	CImageLabelUI *                             m_cstImgLabel;

	// ��ȥ��ʱ��
	CLabelUI *                                  m_lblElapsed;

	// �������ϼ�
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];             // �������¶�����
	CReaderUI *                                 m_hand_reader;                               // ���������¶�����

private:
	// ���һ�ε��¶�����
	TempItem              m_aLastTemp[MAX_READERS_PER_GRID];
	// �ֳֶ����������һ���¶�����
	TempItem              m_HandLastTemp;
	// ��ǰѡ�е����ж�����index(��1��ʼ)
	DWORD                 m_dwSelSurReaderIndex;
};
