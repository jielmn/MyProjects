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
	void  SetView(int nTabIndex);

	// 设置术中读卡器的连接状态
	void  SetSurReaderStatus(DWORD dwSubIndex, BOOL bConnected);

	// 术中读卡器温度
	void  OnSurReaderTemp(DWORD dwSubIndex, const TempItem & item);
	void  ShowSurReaderTemp(DWORD j, const TempItem & item);
	// 显示手持Tag温度
	void  ShowHandReaderTemp(const TempItem & item);
	// 手持读卡器温度
	void  OnHandReaderTemp(const TempItem & item);

	// 选中读卡器
	void  OnSurReaderSelected(DWORD  dwSubIndex);

	// 设置温度字体
	void SetFont(int index);

	// 更新显示逝去的时间
	void UpdateElapsed();

	// 得到温度历史数据
	void OnQueryTempRet(DWORD dwSubIndex, const char * szTagId, const std::vector<TempItem*> & vRet);
	// 得到温度历史数据(手持Tag)
	void OnQueryHandTempRet(const char * szTagId, const std::vector<TempItem*> & vRet);

	// 一周前的数据删除
	void PruneData();
	void PruneData(std::vector<TempItem*> & v,time_t t);

	DWORD  GetReaderIndex() const;
	const std::vector<TempItem * > & GetTempData(DWORD dwReaderIndex) const;
	CModeButton::Mode  GetMode() const;

	void ResetData();
	void SetPatientName(const char * szName);
	void SetPatientNameInHandMode(const char * szName);

	// 导出excel
	void  ExportExcel();

	// 打印excel表格
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

	// 三种模式按钮
	CModeButton *                               m_cstModeBtn;

	// 当前选中读卡器的连接状态
	CControlUI *                                m_CurReaderState;

	// 显示温度数据和曲线的
	CImageLabelUI *                             m_cstImgLabel;
	CMyImageUI *                                m_cstImg;

	// 逝去的时间
	CLabelUI *                                  m_lblElapsed;

	// 读卡器合集
	CVerticalLayoutUI *                         m_layReaders;
	CReaderUI *                                 m_readers[MAX_READERS_PER_GRID];             // 连续测温读卡器
	CReaderUI *                                 m_hand_reader;                               // 非连续测温读卡器
	
	CEditableButtonUI *                         m_cstPatientName;
	CEditableButtonUI *                         m_cstPatientNameM;

private:
	// 最后一次的温度数据
	TempItem              m_aLastTemp[MAX_READERS_PER_GRID];
	// 手持读卡器的最后一次温度数据
	TempItem              m_HandLastTemp;
	// 当前选中的术中读卡器index(从1开始)
	DWORD                 m_dwSelSurReaderIndex;

	std::vector<TempItem * >           m_vTemp[MAX_READERS_PER_GRID];
	std::vector<TempItem * >           m_vHandTemp;
};
