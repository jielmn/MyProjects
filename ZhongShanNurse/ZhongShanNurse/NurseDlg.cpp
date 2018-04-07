//#include <windows.h>
#include <afx.h>   
#include "NurseDlg.h"

CNurseWnd::CNurseWnd(BOOL  bAdd) : m_bAdd(bAdd) {
	
}

void  CNurseWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	CDuiString strText;

	if (msg.sType == "click") {
		if (name == NURSE_OK_BUTTON_ID) {
			strText = GET_CONTROL_TEXT(m_edNurseId);
			// 如果id改变
			if (!m_bAdd && 0 != strcmp(strText, m_tNurseInfo.szId)) {
				m_tNurseInfo.bStrIdChanged = TRUE;
			}
			strncpy_s(m_tNurseInfo.szId, strText, sizeof(m_tNurseInfo.szId));

			strText = GET_CONTROL_TEXT(m_edNurseName);
			strncpy_s(m_tNurseInfo.szName, strText, sizeof(m_tNurseInfo.szName));

			if (m_bAdd) {
				CBusiness::GetInstance()->AddNurseAsyn( &m_tNurseInfo, this->GetHWND());
			}
			else {
				CBusiness::GetInstance()->ModifyNurseAsyn(&m_tNurseInfo, this->GetHWND());
			}

		}
	}
	WindowImplBase::Notify(msg);
}

void  CNurseWnd::InitWindow()
{
	DuiLib::CDuiString  strText;

	m_edNurseId = (DuiLib::CEditUI *)m_PaintManager.FindControl(NURSE_ID_EDIT_ID);
	m_edNurseName = (DuiLib::CEditUI *)m_PaintManager.FindControl(NURSE_NAME_EDIT_ID);
	

	if ( !m_bAdd ) {
		SET_CONTROL_TEXT( m_edNurseId, m_tNurseInfo.szId);
		SET_CONTROL_TEXT( m_edNurseName, m_tNurseInfo.szName);		
	}

	WindowImplBase::InitWindow();
}


LRESULT CNurseWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int ret = 0;
	if (uMsg == UM_ADD_NURSE_RET) {
		ret = (int)wParam;
		DWORD dwId = (DWORD)lParam;

		// 成功
		if (0 == ret) {
			m_tNurseInfo.dwId = dwId;
			PostMessage(WM_CLOSE);
		}
		else {
			::MessageBox(this->GetHWND(), GetErrDescription(ret), CAPTION_NURSE_MSG_BOX, 0);
		}
	}
	else if (uMsg == UM_MODIFY_NURSE_RET) {
		ret = (int)wParam;

		// 成功
		if (0 == ret) {
			PostMessage(WM_CLOSE);
		}
		else {
			::MessageBox(this->GetHWND(), GetErrDescription(ret), CAPTION_NURSE_MSG_BOX, 0);
		}
	}

	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}