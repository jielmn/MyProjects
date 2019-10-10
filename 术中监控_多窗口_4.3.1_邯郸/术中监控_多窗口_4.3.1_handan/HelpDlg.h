#pragma once

#include "common.h"

#include "vlc/vlc.h"
#include "vlc/libvlc.h"

#include "MyWndUI.h"
#include "MyProgressUI.h"

class CHelpDlg : public DuiLib::WindowImplBase
{
public:
	CHelpDlg();
	~CHelpDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIHelpFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("Help.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("wrong_folder"); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow(); 

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam); 
	CControlUI * CreateControl(LPCTSTR pstrClass);

private:
protected:
	libvlc_instance_t *         libvlc_inst;
	libvlc_media_player_t *     libvlc_mp;
	libvlc_media_t *            libvlc_m;

	CMyWndUI *                  m_video;
	CControlUI *                m_header;
	//CControlUI *                m_bottom;
	BOOL                        m_bFullScreen;

	CLabelUI *                  m_lblCurTime;
	CLabelUI *                  m_lblDuration;
	CMyProgressUI *             m_progress;
	CButtonUI *                 m_btnStart;
	CButtonUI *                 m_btnStop;

private:
	BOOL    IsPlaying();
};