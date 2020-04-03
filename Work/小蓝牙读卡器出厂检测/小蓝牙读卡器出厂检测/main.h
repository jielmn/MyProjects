#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"
 
class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
#endif

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnFinalMessage(HWND hWnd);


private:
	void   CheckDevices();
	void   CheckControls();
	void   OnAutoTest();
	void   OnInfoMsg(WPARAM wParam, LPARAM  lParam);
	void   OnBluetoothCnnMsg(WPARAM wParam, LPARAM  lParam);
	void   OnBleDisconnected(WPARAM wParam, LPARAM  lParam);
	void   OnMeasureTemp();
	void   OnAutoTestFin(WPARAM wParam, LPARAM  lParam);
	void   OnStopAutoTestRet();

private:
	CLabelUI *                m_lblStatus;
	CEditUI *                 m_edReaderMac;
	CButtonUI *               m_btnClearMac;
	CButtonUI *               m_btnAutoTest;
	CRichEditUI *             m_rchInfo;
	CButtonUI *               m_btnTemp;
	COptionUI *               m_opPass;
	COptionUI *               m_opNotPass;
	COptionUI *               m_opErrReason[3];
	CButtonUI *               m_btnSaveResult;

private:
	// 先检查串口，再检测测试是否开始
	BOOL                      m_bComConnected;                // 串口是否连接上
	BOOL                      m_bAutoTestStarted;             // 是否点击了自动测试按钮
	BOOL                      m_bAutoTestFinished;            // 自动测试是否完成
	BOOL                      m_bStopAutoTestFinished;        // 停止自动测试是否完成
	BOOL                      m_bBleConnected;                // 蓝牙连接是否还在

};
