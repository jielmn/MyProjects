#pragma once
#include "common.h"

class CEdtComboUI : public  CContainerUI, INotifyUI {
public:
	CEdtComboUI();
	~CEdtComboUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	bool OnMySize(void * pParam);
	void Notify(TNotifyUI& msg);

public:
	CComboUI *             m_cmb;
	CEditUI *              m_edt;
};

class CGridUI : public CContainerUI, INotifyUI {
public:
	CGridUI();
	~CGridUI();

	void SetIndex(int nIndex);
	int  GetIndex();

	void SetUserName(CDuiString strName);
	CDuiString  GetUserName();

	void SetHeartBeat(int nHeartBeat);
	int  GetHeartBeat();

	void SetOxy(int nOxy);
	int  GetOxy();

	void SetTemp(int nTemp);
	int  GetTemp();

	void SetDeviceId(CDuiString strId);
	CDuiString  GetDeviceId();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void Notify(TNotifyUI& msg);

	void SetIndex();
	void SetUserName();
	void SetHeartBeat();
	void SetOxy();
	void SetTemp();
	void SetDeviceId();

private:
	CDialogBuilderCallbackEx                    m_callback;
	CLabelUI *                                  m_lblIndex;
	CEdtComboUI *                               m_cstName;
	CLabelUI *                                  m_lblHeartBeat;
	CLabelUI *                                  m_lblOxy;
	CLabelUI *                                  m_lblTemp;
	CLabelUI *                                  m_lblDeviceId;

private:
	int           m_nIndex;
	CDuiString    m_strName;
	int           m_nHeartBeat;
	int           m_nOxy;
	int           m_nTemp;
	CDuiString    m_strDeviceId;
};