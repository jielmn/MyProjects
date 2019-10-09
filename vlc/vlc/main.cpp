// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "DuiMenu.h"

#define VLC_TIMER    1

CDuiFrameWnd::CDuiFrameWnd() {
	libvlc_inst = 0;
	libvlc_mp = 0;
	libvlc_m = 0;
	m_bFullScreen = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	WindowImplBase::InitWindow();

	m_video = (CMyWndUI *)m_PaintManager.FindControl("video");
	m_header = m_PaintManager.FindControl("header");
	m_bottom = m_PaintManager.FindControl("bottom");
	m_lblCurTime = (CLabelUI *)m_PaintManager.FindControl("lblCurTime");
	m_lblDuration = (CLabelUI *)m_PaintManager.FindControl("lblDuration");
	m_progress = (CMyProgressUI *)m_PaintManager.FindControl("ProgressDemo1");
	m_btnStart = (CButtonUI *)m_PaintManager.FindControl("btnStart");
	m_btnStop = (CButtonUI *)m_PaintManager.FindControl("btnStop");

	libvlc_inst = libvlc_new(0, NULL);

	//HWND screen_hwnd = GetHWND();
	HWND screen_hwnd = m_video->GetHWnd();

	/* Create a new item */
	//m = libvlc_media_new_location (libvlc_inst, "http://mycool.movie.com/test.mov");

	//libvlc_m = libvlc_media_new_path(libvlc_inst, "d:\\test.flv"); 
	libvlc_m = libvlc_media_new_location(libvlc_inst, "http://localhost:8080/test/a.flv");

	/* Create a media player playing environement */
	libvlc_mp = libvlc_media_player_new_from_media(libvlc_m);

	/* No need to keep the media now */
	libvlc_media_release(libvlc_m);

	//on windows
	libvlc_media_player_set_hwnd(libvlc_mp, screen_hwnd);

	/* play the media_player */
	int x = libvlc_media_player_play(libvlc_mp);

	SetTimer(GetHWND(), VLC_TIMER, 1000, 0);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if ( 0 == strcmp(pstrClass, "MyWnd") ) {
		return new CMyWndUI;
	}
	else if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgressUI;
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == "menu") {
		CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender, IsPlaying(), m_bFullScreen);
		pMenu->Init(*this, msg.ptMouse);
		pMenu->ShowWindow(TRUE);  
	}
	else if (msg.sType == "menu_full_screen") {
		if ( !m_bFullScreen ) {
			m_header->SetVisible(false);
			m_bottom->SetVisible(false);
		}
		else {
			m_header->SetVisible(true);
			m_bottom->SetVisible(true);
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
			m_progress->SetValue( (int)(posf * 1000.0f) );
		}		
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == WM_TIMER ) {
		if ( wParam == VLC_TIMER ) {
			char szTime[256];
			int curtime = (int)libvlc_media_player_get_time(libvlc_mp);
			VlcTime2Str(szTime, sizeof(szTime), curtime);
			m_lblCurTime->SetText(szTime);

			int duration = (int)libvlc_media_player_get_length(libvlc_mp); 
			VlcTime2Str(szTime, sizeof(szTime), duration);
			m_lblDuration->SetText(szTime);

			if (duration > 0) {
				double progress = (double)curtime / duration;
				m_progress->SetValue( (int)(progress * 1000) ); 
			}

			libvlc_state_t state = libvlc_media_player_get_state(libvlc_mp);

			if (libvlc_media_player_get_state(libvlc_mp) == libvlc_Ended) {
				// 如果播放过
				if (m_progress->GetValue() > 0) {
					m_progress->SetValue(1000);
				}	
				KillTimer(GetHWND(), wParam);
			}
		}
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

BOOL CDuiFrameWnd::IsPlaying() {
	BOOL bPlay = FALSE;
	if (libvlc_mp && 1 == libvlc_media_player_is_playing(libvlc_mp)) {
		bPlay = TRUE;
	}
	return bPlay;
}






                     
     
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


