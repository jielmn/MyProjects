//#include <windows.h>
#include <afx.h>   
#include "PatientDlg.h"

CPatientWnd::CPatientWnd(BOOL  bAdd) : m_bAdd(bAdd) {
	memset(&m_tPatientInfo, 0, sizeof(PatientInfo));
}

void  CPatientWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	CDuiString strText;

	if (msg.sType == "click") {
		if (name == PATIENT_OK_BUTTON_ID) {
			strText = GET_CONTROL_TEXT(m_edPatientId);
			strncpy_s(m_tPatientInfo.szId, strText, sizeof(m_tPatientInfo.szId));

			strText = GET_CONTROL_TEXT(m_edPatientName);
			strncpy_s(m_tPatientInfo.szName, strText, sizeof(m_tPatientInfo.szName));

			strText = GET_CONTROL_TEXT(m_edBedNo);
			strncpy_s(m_tPatientInfo.szBedNo, strText, sizeof(m_tPatientInfo.szBedNo));

			if ( m_opFemale && m_opFemale->IsSelected() ) {
				m_tPatientInfo.bFemale = TRUE;
			}
			else {
				m_tPatientInfo.bFemale = FALSE;
			}

			if (m_opOutHos && m_opOutHos->IsSelected()) {
				m_tPatientInfo.bOutHos = TRUE;
			}
			else {
				m_tPatientInfo.bOutHos = FALSE;
			}

			if ( m_bAdd ) {
				CBusiness::GetInstance()->AddPatientAsyn(&m_tPatientInfo, this->GetHWND());
			}				
			else {
				CBusiness::GetInstance()->ModifyPatientAsyn(&m_tPatientInfo, this->GetHWND());
			}
		}		
	}
	WindowImplBase::Notify(msg);
}

void  CPatientWnd::InitWindow()
{
	DuiLib::CDuiString  strText;

	m_edPatientId = (DuiLib::CEditUI *)m_PaintManager.FindControl(PATIENT_ID_EDIT_ID);
	m_edPatientName = (DuiLib::CEditUI *)m_PaintManager.FindControl(PATIENT_NAME_EDIT_ID);
	m_opMale = (DuiLib::COptionUI *)m_PaintManager.FindControl(MALE_OPTION_ID);
	m_opFemale = (DuiLib::COptionUI *)m_PaintManager.FindControl(FEMALE_OPTION_ID);
	m_opInHos = (DuiLib::COptionUI *)m_PaintManager.FindControl(IN_HOS_OPTION_ID);
	m_opOutHos = (DuiLib::COptionUI *)m_PaintManager.FindControl(OUT_HOS_OPTION_ID);
	m_edBedNo = (DuiLib::CEditUI *)m_PaintManager.FindControl(BED_NO_EDIT_ID);
	m_lyTags = (DuiLib::CHorizontalLayoutUI *)m_PaintManager.FindControl(TAGS_LAYOUT_ID);

	if (m_bAdd) {		
		SET_CONTROL_VISIBLE(m_lyTags, false);
	}
	else {
		SET_CONTROL_TEXT(m_edPatientId,   m_tPatientInfo.szId);
		SET_CONTROL_TEXT(m_edPatientName, m_tPatientInfo.szName);
		SET_CONTROL_TEXT(m_edBedNo,       m_tPatientInfo.szBedNo);
		if (m_tPatientInfo.bFemale) {
			if (m_opFemale) {
				m_opFemale->Selected(true);
			}
		}

		if (m_tPatientInfo.bOutHos) {
			if (m_opOutHos) {
				m_opOutHos->Selected(true);
			}
		}
	}

	WindowImplBase::InitWindow();
}


LRESULT CPatientWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int ret = 0;
	if ( uMsg == UM_ADD_PATIENT_RET ) {
		ret = (int)wParam;
		DWORD dwId = (DWORD)lParam;

		// 成功
		if (0 == ret) {
			m_tPatientInfo.dwId = dwId;
			PostMessage(WM_CLOSE);
		}
		else {
			::MessageBox( this->GetHWND(), GetErrDescription( ret ), CAPTION_PATIENT_MSG_BOX, 0 );
		}
	}
	else if (uMsg == UM_MODIFY_PATIENT_RET) {
		ret = (int)wParam;

		// 成功
		if (0 == ret) {
			PostMessage(WM_CLOSE);
		}
		else {
			::MessageBox(this->GetHWND(), GetErrDescription(ret), CAPTION_PATIENT_MSG_BOX, 0);
		}
	}

	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}