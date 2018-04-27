
#pragma once
#include "UIlib.h"
#include "sigslot.h"
#include "TelemedReader.h"
#include "MyProgress.h"

class CDuiFrameWnd : public DuiLib::WindowImplBase, public sigslot::has_slots<>
{
public:
	CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void      InitWindow();
	virtual void      Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	DuiLib::CButtonUI *              m_btnSetId;
	DuiLib::CButtonUI *              m_btnSetTime;
	DuiLib::CButtonUI *              m_btnSetMode;
	DuiLib::CButtonUI *              m_btnClear;
	DuiLib::CButtonUI *              m_btnGetData;

	DuiLib::CEditUI *                m_edId;
	DuiLib::COptionUI *              m_opNormalMode;
	DuiLib::CListUI *                m_lstData;
	DuiLib::CLabelUI *               m_lblReaderStatus;

	CMyProgress *                    m_progress;

	void   SetBusy( BOOL bBusy = TRUE );

private:

	void  OnReaderStatusChange(CTelemedReader::TELEMED_READER_STATUS eNewStatus);
	void  OnReaderStatusChangeMsg(CTelemedReader::TELEMED_READER_STATUS eNewStatus);

	void  OnSetReaderIdRet(int ret);
	void  OnSetReaderIdRetMsg(int ret);

	void  OnSetReaderTimeRet(int ret);
	void  OnSetReaderTimeRetMsg(int ret);

	void  OnSetReaderModeRet(int ret);
	void  OnSetReaderModeRetMsg(int ret);

	void  OnClearReaderRet(int ret);
	void  OnClearReaderRetMsg(int ret);

	void  OnGetReaderDataRet(int, const std::vector<TempItem* > &);
	void  OnGetReaderDataRetMsg(int, const std::vector<TempItem* > *);

private:
	void  OnSetReaderId();
	void  OnSetReaderTime();
	void  OnSetReaderMode();
	void  OnClearReader();
	void  OnGetData();
};
