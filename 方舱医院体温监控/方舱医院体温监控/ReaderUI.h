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
	void  SetReaderStatus(BOOL bConnected);
	BOOL  IsConnected();
	// 显示断线状态下的上一次温度
	//void  SetDisconnectedTemp(DWORD dwTemp);
	// 显示温度(断线状态和非断线状态不一样)
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
	CLabelUI *                                  m_lblIndicator;           // A~F指示器
	DWORD                                       m_dwIndicator;
	CControlUI *                                m_indicator;              // 颜色指示器
	ReaderType                                  m_type; 
	BOOL                                        m_bConnected;             // 连接状态
	DWORD                                       m_dwTemp;

public:
	COptionUI *                                 m_optSelected;            // 选择框
	CEditableButtonUI *                         m_cstBodyPart;            // 身体部位
	CLabelUI *                                  m_lblTemp;                // 温度数据
	CControlUI *                                m_state;                  // 断线状态
	CLabelUI *                                  m_lblReaderId;            // ReaderId
	CLabelUI *                                  m_lblTagId;               // TagId
};
