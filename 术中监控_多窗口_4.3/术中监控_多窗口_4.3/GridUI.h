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

	// 双击后的视图切换
	void  SwitchView();

	// 设置术中读卡器的连接状态
	void  SetSurReaderStatus(DWORD dwSubIndex, BOOL bConnected);

	// 术中读卡器温度
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

	// 三种模式按钮
	CModeButton *                               m_cstModeBtn;

	// 当前选中读卡器的连接状态
	CControlUI *                                m_CurReaderState;

	// 显示温度数据和曲线的
	CImageLabelUI *                             m_cstImgLabel;

	// 读卡器合集
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];             // 连续测温读卡器
	CReaderUI *                                 m_hand_reader;                               // 非连续测温读卡器

private:
	// 最后一次的温度数据
	TempItem              m_aLastTemp[MAX_READERS_PER_GRID];
	// 当前选中的术中读卡器index(从1开始)
	DWORD                 m_dwSelSurReaderIndex;
};
