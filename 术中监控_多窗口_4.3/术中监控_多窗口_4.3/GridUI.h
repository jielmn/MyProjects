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
	void  SetView(int nTabIndex);

	// �������ж�����������״̬
	void  SetSurReaderStatus(DWORD dwSubIndex, BOOL bConnected);

	// ���ж������¶�
	void  OnSurReaderTemp(DWORD dwSubIndex, const TempItem & item);
	void  ShowSurReaderTemp(DWORD j, const TempItem & item);
	// ��ʾ�ֳ�Tag�¶�
	void  ShowHandReaderTemp(const TempItem & item);
	// �ֳֶ������¶�
	void  OnHandReaderTemp(const TempItem & item);

	// ѡ�ж�����
	void  OnSurReaderSelected(DWORD  dwSubIndex);

	// �����¶�����
	void SetFont(int index);

	// ������ʾ��ȥ��ʱ��
	void UpdateElapsed();

	// �õ��¶���ʷ����
	void OnQueryTempRet(DWORD dwSubIndex, const char * szTagId, const std::vector<TempItem*> & vRet);
	// �õ��¶���ʷ����(�ֳ�Tag)
	void OnQueryHandTempRet(const char * szTagId, const std::vector<TempItem*> & vRet);

	// һ��ǰ������ɾ��
	void PruneData();
	void PruneData(std::vector<TempItem*> & v,time_t t);

	DWORD  GetReaderIndex() const;
	const std::vector<TempItem * > & GetTempData(DWORD dwReaderIndex) const;
	CModeButton::Mode  GetMode() const;

	void ResetData();
	void SetPatientName(const char * szName);
	void SetPatientNameInHandMode(const char * szName);

	// ����excel
	void  ExportExcel();

	// ��ӡexcel���
	void  PrintExcel();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void Notify(TNotifyUI& msg);
	void OnModeChanged(int nSource);
	void SetCurReaderTemp(CLabelUI * pReaderUI );
	void SetReaderTemp( DWORD dwSubIndex, DWORD  dwTemp,DWORD dwHighAlarm,DWORD dwLowAlarm, time_t t);
	void SetHandReaderTemp(DWORD  dwTemp, DWORD dwHighAlarm, DWORD dwLowAlarm, time_t t);
	void OnPatientNameChanged(CEditableButtonUI * pSource);
	
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
	CMyImageUI *                                m_cstImg;

	// ��ȥ��ʱ��
	CLabelUI *                                  m_lblElapsed;

	// �������ϼ�
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];             // �������¶�����
	CReaderUI *                                 m_hand_reader;                               // ���������¶�����
	
	CEditableButtonUI *                         m_cstPatientName;
	CEditableButtonUI *                         m_cstPatientNameM;

private:
	// ���һ�ε��¶�����
	TempItem              m_aLastTemp[MAX_READERS_PER_GRID];
	// �ֳֶ����������һ���¶�����
	TempItem              m_HandLastTemp;
	// ��ǰѡ�е����ж�����index(��1��ʼ)
	DWORD                 m_dwSelSurReaderIndex;

	std::vector<TempItem * >           m_vTemp[MAX_READERS_PER_GRID];
	std::vector<TempItem * >           m_vHandTemp;
};
