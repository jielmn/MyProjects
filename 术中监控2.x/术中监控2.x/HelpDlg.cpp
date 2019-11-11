#include "HelpDlg.h"
#include "DuiMenu.h"

#define VLC_TIMER    1

CHelpDlg::CHelpDlg() {
	libvlc_inst = 0;
	libvlc_mp = 0;
	libvlc_m = 0;
	m_bFullScreen = FALSE;
}

CHelpDlg::~CHelpDlg() {
	if (libvlc_mp)
		libvlc_media_player_release(libvlc_mp);

	if (libvlc_inst)
		libvlc_release(libvlc_inst);  
}

void  CHelpDlg::InitWindow() {
	WindowImplBase::InitWindow();

	m_video = (CMyWndUI *)m_PaintManager.FindControl("video");
	m_header = m_PaintManager.FindControl("header");
	//m_bottom = m_PaintManager.FindControl("bottom");
	m_lblCurTime = (CLabelUI *)m_PaintManager.FindControl("lblCurTime");
	m_lblDuration = (CLabelUI *)m_PaintManager.FindControl("lblDuration");
	m_progress = (CMyProgressUI *)m_PaintManager.FindControl("ProgressDemo1");
	m_btnStart = (CButtonUI *)m_PaintManager.FindControl("btnStart");
	m_btnStop = (CButtonUI *)m_PaintManager.FindControl("btnStop");

	libvlc_inst = libvlc_new(0, NULL);
	if (0 == libvlc_inst) {
		return;
	}

	//HWND screen_hwnd = GetHWND();
	HWND screen_hwnd = m_video->GetHWnd();

	/* Create a new item */
	//m = libvlc_media_new_location (libvlc_inst, "http://mycool.movie.com/test.mov"); 

	libvlc_m = libvlc_media_new_path(libvlc_inst, "video\\telemed.mp4");  
	//libvlc_m = libvlc_media_new_location(libvlc_inst, "http://localhost:8080/test/a.flv");
	if (0 == libvlc_m) {
		return;
	}

	/* Create a media player playing environement */
	libvlc_mp = libvlc_media_player_new_from_media(libvlc_m);
	if (0 == libvlc_mp) {
		return;
	}

	/* No need to keep the media now */
	libvlc_media_release(libvlc_m);

	//on windows
	libvlc_media_player_set_hwnd(libvlc_mp, screen_hwnd);

	/* play the media_player */
	int x = libvlc_media_player_play(libvlc_mp);

	SetTimer(GetHWND(), VLC_TIMER, 1000, 0);
}

CControlUI * CHelpDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyWnd")) {
		return new CMyWndUI;
	}
	else if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgressUI;
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CHelpDlg::Notify(TNotifyUI& msg) {
	if (msg.sType == "menu") {
		//CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender, IsPlaying(), m_bFullScreen);
		//pMenu->Init(*this, msg.ptMouse);
		//pMenu->ShowWindow(TRUE);
	}
	else if (msg.sType == "menu_full_screen") {
		if (!m_bFullScreen) {
			m_header->SetVisible(false);
			//m_bottom->SetVisible(false);
		}
		else {
			m_header->SetVisible(true);
			//m_bottom->SetVisible(true); 
		}
		m_bFullScreen = !m_bFullScreen;
	}
	else if (msg.sType == "click") {
		if (msg.pSender->GetName() == "btnStart") {
			libvlc_state_t playerstate = libvlc_media_player_get_state(libvlc_mp);
			if (playerstate == libvlc_Playing) {
				libvlc_media_player_set_pause(libvlc_mp, 1);
				m_btnStart->SetNormalImage("start.png");
			}
			else if (playerstate == libvlc_Paused) {
				libvlc_media_player_set_pause(libvlc_mp, 0);
				m_btnStart->SetNormalImage("pause.png");
			}
			else if (playerstate == libvlc_Stopped) {
				libvlc_media_player_play(libvlc_mp);
				SetTimer(GetHWND(), VLC_TIMER, 1000, 0);
				m_btnStart->SetNormalImage("pause.png");
			}
			else if (playerstate == libvlc_Ended) {
				libvlc_media_player_stop(libvlc_mp);
				libvlc_media_player_play(libvlc_mp);
				m_progress->SetValue(0);
				SetTimer(GetHWND(), VLC_TIMER, 1000, 0);
				m_btnStart->SetNormalImage("pause.png");
				m_lblCurTime->SetText("00:00:00");
			}
		}
		else if (msg.pSender->GetName() == "btnStop") {
			libvlc_media_player_stop(libvlc_mp);
			KillTimer(GetHWND(), VLC_TIMER);
			m_btnStart->SetNormalImage("start.png");
			m_progress->SetValue(0);
			m_lblCurTime->SetText("00:00:00");
			m_lblDuration->SetText("00:00:00");
		}
	}
	else if (msg.sType == "progress_click") {
		libvlc_state_t playerstate = libvlc_media_player_get_state(libvlc_mp);
		if (msg.lParam > 0 && playerstate != libvlc_Ended) {
			float posf = (float)msg.wParam / msg.lParam;
			libvlc_media_player_set_position(libvlc_mp, posf);
			m_progress->SetValue((int)(posf * 1000.0f));
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CHelpDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if (wParam == VLC_TIMER) {
			char szTime[256];
			int curtime = (int)libvlc_media_player_get_time(libvlc_mp);
			VlcTime2Str(szTime, sizeof(szTime), curtime);
			m_lblCurTime->SetText(szTime);

			int duration = (int)libvlc_media_player_get_length(libvlc_mp);
			VlcTime2Str(szTime, sizeof(szTime), duration);
			m_lblDuration->SetText(szTime);

			if (duration > 0) {
				double progress = (double)curtime / duration;
				m_progress->SetValue((int)(progress * 1000));
			}

			libvlc_state_t state = libvlc_media_player_get_state(libvlc_mp);

			if (libvlc_media_player_get_state(libvlc_mp) == libvlc_Ended) {
				// 如果播放过
				if (m_progress->GetValue() > 0) {
					m_progress->SetValue(1000);
					m_btnStart->SetNormalImage("start.png");
				}
				KillTimer(GetHWND(), wParam);
			}
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

BOOL CHelpDlg::IsPlaying() {
	BOOL bPlay = FALSE;
	if (libvlc_mp && 1 == libvlc_media_player_is_playing(libvlc_mp)) {
		bPlay = TRUE;
	}
	return bPlay; 
}