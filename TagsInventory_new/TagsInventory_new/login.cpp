#include <afx.h>
#include "InvCommon.h"
#include "login.h"
#include "InvDatabase.h"
#include "InvReader.h"
#include "Business.h"

void  CLoginWnd::InitWindow() {
	m_lblDbStatus     = (DuiLib::CLabelUI *)m_PaintManager.FindControl("lblDatabaseStatus");
	m_lblReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl("lblReaderStatus");

	SET_CONTROL_TEXT(m_lblDbStatus,     DB_STATUS_CLOSE_TEXT);
	SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_CLOSE_TEXT);

	WindowImplBase::InitWindow();
}

void CLoginWnd::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

LRESULT CLoginWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_SHOW_DB_STATUS) {
		CInvDatabase::DATABASE_STATUS eStatus = (CInvDatabase::DATABASE_STATUS)wParam;
		if (eStatus == CInvDatabase::STATUS_OPEN) {
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
		}
		else {
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if (uMsg == UM_SHOW_READER_STATUS) {
		CInvReader::READER_STATUS eStatus = (CInvReader::READER_STATUS)wParam;
		if (eStatus == CInvReader::STATUS_OPEN) {
			SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_OK_TEXT );
		}
		else {
			SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if (uMsg == UM_INVENTORY_RESULT) {
		static DWORD  dwLastTick = 0;
		TagItem * pItem = (TagItem *)wParam;

		DWORD dwCur = LmnGetTickCount();
		// 不要发送太频繁
		if (dwCur - dwLastTick < 1000) {
			delete pItem;
			return 0;
		}
		dwLastTick = dwCur;

		// 用户登录
		CBusiness::GetInstance()->LoginUserAsyn(pItem);

		if (pItem) {
			delete pItem;
		}

		return 0;
	}
	else if (uMsg == UM_LOGIN_RESULT) {
		BOOL bLogin = (BOOL)wParam;
		User * pUser = (User *)lParam;

		// 登录成功
		if (bLogin) {
			::PostMessage(this->GetHWND(), WM_CLOSE, 0, 0);
		}

		if (pUser) {
			delete pUser;
		}
		return 0;
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}