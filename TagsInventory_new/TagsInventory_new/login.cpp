#include <afx.h>
#include "InvCommon.h"
#include "login.h"
#include "InvDatabase.h"
#include "InvReader.h"
#include "Business.h"

void  CLoginWnd::InitWindow() {
	m_lblDbStatus     = (DuiLib::CLabelUI *)m_PaintManager.FindControl(DATABASE_STATUS_LABEL_ID);
	m_lblReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(READER_STATUS_LABEL_ID);

	SET_CONTROL_TEXT(m_lblDbStatus,     DB_STATUS_CLOSE_TEXT);
	SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_CLOSE_TEXT);

	WindowImplBase::InitWindow();
}

void CLoginWnd::Notify(DuiLib::TNotifyUI& msg) {
	char buf[8192];

	if ( msg.sType == "click" ) {
		if ( msg.pSender->GetName() == "btnCenter" ) {
#ifdef _DEBUG
			// 用户登录
			g_cfg->GetConfig( LOGIN_CARD_ID, buf, sizeof(buf), "" );
			DWORD  dwLen = strlen(buf);

			char tmp[64] = {0};
			BYTE uid[8192] = { 0 };
			for (DWORD i = 0; i < dwLen / 2; i++) {
				int n = 0;
				memcpy( tmp, buf + 2 * i, 2 );
				sscanf_s( tmp, "%x", &n );
				uid[i] = n;
			}

			TagItem item;
			//item.dwUidLen = 8;
			//memcpy(item.abyUid, "\xe0\x04\x01\x00\xa4\x79\xf3\x90", 8 );
			item.dwUidLen = dwLen / 2;
			memcpy(item.abyUid, uid, item.dwUidLen);
			CBusiness::GetInstance()->LoginUserAsyn(&item);
#endif
		}
	}
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

		if ( CBusiness::GetInstance()->GetDbStatus() == CInvDatabase::STATUS_CLOSE ) {
			::MessageBox(this->GetHWND(), MSG_BOX_DB_CLOSE, CAPTION_LOGIN, 0);
		}
		else {
			// 用户登录
			CBusiness::GetInstance()->LoginUserAsyn(pItem);
		}		

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