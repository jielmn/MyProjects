//#include <windows.h>
#include <afx.h>   
#include <time.h>
#include "PatientDlg.h"
#include "sigslot.h"
#include "Business.h"

CPatientWnd::CPatientWnd (BOOL  bAdd) : m_bAdd(bAdd) {
	memset(&m_tPatientInfo, 0, sizeof(PatientInfo));
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

			if (m_bAdd) {
				CBusiness::GetInstance()->sigAMPatient.emit(&tInfo, OPERATION_ADD, &ret);
			}
			else {
				// 先删除Tag
				std::vector<TagInfo *> vTags;
				CBusiness::GetInstance()->GetPatientTags(m_tPatientInfo.szId, vTags);
				std::vector<TagInfo *>::iterator it;

				CListUI * pList = (CListUI *)m_PaintManager.FindControl("patient_tags");
				if (pList) {
					int nCount = pList->GetCount();
					for (int i = 0; i < nCount; i++) {
						TagId tagId;
						CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(i);
						ConvertTagId( &tagId, pItem->GetText(0) );

						for (it = vTags.begin(); it != vTags.end(); it++) {
							TagInfo * p = *it;
							// 如果找到，则从vector中删除
							if (0 == memcmp(&p->tagId, &tagId, sizeof(TagId))) {
								delete p;
								vTags.erase(it);
								break;
							}
						}
					}
				}

				// vector中的item是要删除的tag!
				for (it = vTags.begin(); it != vTags.end(); it++) {
					TagInfo * p = *it;
					CBusiness::GetInstance()->DeleteTag(&p->tagId);
				}
				ClearVector(vTags);

				// 再删除病人信息
				CBusiness::GetInstance()->sigAMPatient.emit(&tInfo, OPERATION_MODIFY, &ret);
			}
			
			if ( 0 == ret )
				::PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
			else {
				::MessageBox(m_hWnd, GetErrDescription((GlobalErrorNo)ret), "添加/修改病人信息", 0);
			}
			return;
		} 
		else if (name == "btnDelTag") {
			CListUI * pList = (CListUI *)m_PaintManager.FindControl("patient_tags");
			if (0 == pList) {
				return;
			}

			int nSel = pList->GetCurSel();
			if (nSel < 0) {
				::MessageBox(m_hWnd, "没有选中要删除的Tag", "删除Tag", 0);
				return;
			}

			pList->RemoveAt(nSel);
			return;
		}
	}
	WindowImplBase::Notify(msg);
}

void  CPatientWnd::InitWindow()
{
	char buf[8192];
	CControlUI * pControl = 0;
	if (m_bAdd) {
		pControl = m_PaintManager.FindControl("txtTitle");
		if (pControl) {
			pControl->SetText("添加病人信息");
		}

		pControl = m_PaintManager.FindControl("layoutTags");
		if (pControl) {
			pControl->SetVisible(false);
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
			pEdit->SetText(m_tPatientInfo.szId);
			pEdit->SetBkColor(0xFFCCCCCC);
			pEdit->SetNativeEditBkColor(0xFFCCCCCC);
		}

		pControl = m_PaintManager.FindControl("edPatientInId");
		if (pControl) {
			pControl->SetText(m_tPatientInfo.szInNo);
		}

		pControl = m_PaintManager.FindControl("edPatientName");
		if (pControl) {
			pControl->SetText(m_tPatientInfo.szName);
		}

		pControl = m_PaintManager.FindControl("edPatientOffice");
		if (pControl) {
			pControl->SetText(m_tPatientInfo.szOffice);
		}

		pControl = m_PaintManager.FindControl("edBedId");
		if (pControl) {
			pControl->SetText(m_tPatientInfo.szBedNo);
		}

		pControl = m_PaintManager.FindControl("edCardNo");
		if (pControl) {
			pControl->SetText(m_tPatientInfo.szCardNo);
		}

		COptionUI * pMale   = (COptionUI *)m_PaintManager.FindControl("btnMale");
		COptionUI * pFemale = (COptionUI *)m_PaintManager.FindControl("btnFemale");

		if (m_tPatientInfo.bMale) {
			if (pMale)
				pMale->Selected(true);
		}
		else {
			if (pFemale) {
				pFemale->Selected(true);
			}
		}

		pControl = m_PaintManager.FindControl("edPatientAge");
		if (pControl) {
			_snprintf_s(buf, sizeof(buf), "%d", m_tPatientInfo.nAge);
			pControl->SetText(buf);
		}
		
		pControl = m_PaintManager.FindControl("dtIndate");
		if (pControl) {
			CDateTimeUI * p = (CDateTimeUI *)pControl;
			SYSTEMTIME s;
			struct tm t;
			localtime_s( &t, &m_tPatientInfo.tInDate);
			memset(&s, 0, sizeof(s));
			s.wYear = t.tm_year + 1900;
			s.wMonth = t.tm_mon + 1;
			s.wDay = t.tm_mday;
			p->SetTime(&s);
			p->SetText( ConvertDate( buf, sizeof(buf), &m_tPatientInfo.tInDate ) );
		}

		pControl = m_PaintManager.FindControl("patient_tags");
		if (pControl) {
			CListUI * pList = (CListUI *)pControl;

			std::vector<TagInfo *> vTags;
			CBusiness::GetInstance()->GetPatientTags(m_tPatientInfo.szId, vTags);
			std::vector<TagInfo *>::iterator it;
			for (it = vTags.begin(); it != vTags.end(); it++) {
				TagInfo * pTag = *it;
				ConvertTagId(buf, sizeof(buf), &pTag->tagId );

				CListTextElementUI* pListElement = new CListTextElementUI;
				pList->Add(pListElement);
				pListElement->SetText(0, buf);
			}

			if (pList->GetCount() > 0) {
				pList->SelectItem(0);
			}

			ClearVector(vTags);
		}
		

	}

	
	

	WindowImplBase::InitWindow();
}
