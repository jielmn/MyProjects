#include <windows.h>
#include <time.h>
#include "PatientDlg.h"
#include "sigslot.h"
#include "Business.h"

CPatientWnd::CPatientWnd (BOOL  bAdd) : m_bAdd(bAdd) {

}

void  CPatientWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (name == "btnPatientOk") {
			CControlUI * pControl = 0;
			CDuiString  strId;         // 编号
			CDuiString  strInId;       // 住院号
			CDuiString  strName;       // 姓名
			BOOL        bMale;         // 性别
			int         nAge;          // 年龄
			CDuiString  strOffice;     // 科室
			CDuiString  strBedId;      // 床号
			time_t      tInDate;       // 入院日期
			CDuiString  strCardNo;     // 就诊卡

			pControl = m_PaintManager.FindControl("edPatientId");
			if (pControl) {
				strId = pControl->GetText();
			}

			pControl = m_PaintManager.FindControl("edPatientInId");
			if (pControl) {
				strInId = pControl->GetText();
			}

			pControl = m_PaintManager.FindControl("edPatientName");
			if (pControl) {
				strName = pControl->GetText();
			}

			pControl = m_PaintManager.FindControl("btnMale");
			if (pControl) {
				COptionUI * pBtn = (COptionUI *)pControl;
				if ( pBtn->IsSelected() ) {
					bMale = TRUE;
				}
				else
				{
					bMale = FALSE;
				}
			}

			pControl = m_PaintManager.FindControl("edPatientAge");
			if (pControl) {
				sscanf_s( (const char *)pControl->GetText(), "%d", &nAge);
			}

			pControl = m_PaintManager.FindControl("edPatientOffice");
			if (pControl) {
				strOffice = pControl->GetText();
			}

			pControl = m_PaintManager.FindControl("edBedId");
			if (pControl) {
				strBedId = pControl->GetText();
			}

			pControl = m_PaintManager.FindControl("dtIndate");
			if (pControl) {
				CDuiString  strInDate = pControl->GetText();
				struct tm t;
				memset(&t, 0, sizeof(t));
				sscanf_s((const char*)strInDate, "%d-%d-%d", &t.tm_year, &t.tm_mon, &t.tm_mday );
				t.tm_year -= 1900;
				t.tm_mon--;
				tInDate = mktime(&t);
			}

			pControl = m_PaintManager.FindControl("edCardNo");
			if (pControl) {
				strCardNo = pControl->GetText();
			}

			PatientInfo tInfo;
			strncpy_s(tInfo.szId,     strId,      sizeof(tInfo.szId) - 1 );
			strncpy_s(tInfo.szInNo,   strInId,    sizeof(tInfo.szInNo) - 1);
			strncpy_s(tInfo.szName,   strName,    sizeof(tInfo.szName) - 1);
			strncpy_s(tInfo.szOffice, strOffice,  sizeof(tInfo.szOffice) - 1);
			strncpy_s(tInfo.szBedNo,  strBedId,   sizeof(tInfo.szBedNo) - 1);
			strncpy_s(tInfo.szCardNo, strCardNo,  sizeof(tInfo.szCardNo) - 1);
			tInfo.bMale = bMale;
			tInfo.nAge = nAge;
			tInfo.tInDate = tInDate;

			int ret = 0;
			CBusiness::GetInstance()->sigAMPatient.emit( &tInfo, OPERATION_ADD, &ret );

			if ( 0 == ret )
				::PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
			else {
				::MessageBox(m_hWnd, GetErrDescription((GlobalErrorNo)ret), "添加/修改病人信息", 0);
			}
			return;
		}
	}
	WindowImplBase::Notify(msg);
}

void  CPatientWnd::InitWindow()
{
	CControlUI * pControl = 0;
	if (m_bAdd) {
		pControl = m_PaintManager.FindControl("txtTitle");
		if (pControl) {
			pControl->SetText("添加病人信息");
		}		
	}
	else {
		pControl = m_PaintManager.FindControl("txtTitle");
		if (pControl) {
			pControl->SetText("修改病人信息");
		}

		pControl = m_PaintManager.FindControl("edPatientId");
		if (pControl) {
			CEditUI * pEdit = (CEditUI *)pControl;
			pEdit->SetReadOnly(true);
		}
	}

	
	

	WindowImplBase::InitWindow();
}