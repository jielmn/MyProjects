#include "AreaDlg.h"

CAreaWnd::CAreaWnd(BOOL  bAdd /*= TRUE*/) {
	m_bAdd = bAdd;
}

void  CAreaWnd::OnMyOk() {
	TArea  area;
	memset(&area, 0, sizeof(area));

	DuiLib::CDuiString  strText;
	strText = m_edtAreaNo->GetText();

	if ( 1 != sscanf( strText, "%lu", &area.dwAreaNo ) ) {
		MessageBox( GetHWND(), "请输入病区号", "错误", 0 );
		return;
	}

	if ( area.dwAreaNo == 0 || area.dwAreaNo > 100 ) {
		MessageBox(GetHWND(), "病区号范围1~100", "错误", 0);
		return;
	}

	STRNCPY( area.szAreaName, m_edtAreaName->GetText(), sizeof(area.szAreaName) );
	// memcpy( &m_tArea, &area, sizeof(TArea) );

	std::vector<TArea *>::iterator it;
	// 如果是添加
	if ( m_bAdd ) {
		for (it = g_vArea.begin(); it != g_vArea.end(); ++it) {
			TArea * pArea = *it;
			if ( pArea->dwAreaNo == area.dwAreaNo ) {
				MessageBox(GetHWND(), "病区号和已有的病区号重复", "错误", 0);
				return;
			}
		}

		TArea * pNewArea = new TArea;
		memcpy(pNewArea, &area, sizeof(TArea));
		g_vArea.push_back(pNewArea);

		SaveAreas();

		memcpy(&m_tArea, &area, sizeof(TArea));
		PostMessage(WM_CLOSE);
	}
	// 如果是修改
	else {

	}
}

void  CAreaWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == "click") {
		if ( name == "btnMyOk" ) {
			OnMyOk();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CAreaWnd::InitWindow() {
	m_lblTitle = static_cast<DuiLib::CTextUI *>(m_PaintManager.FindControl("txtTitle"));
	if (m_bAdd) {
		m_lblTitle->SetText("添加病区信息");
	}
	else {
		m_lblTitle->SetText("修改病区信息");
	}

	m_edtAreaNo = static_cast<DuiLib::CEditUI *>(m_PaintManager.FindControl("edAreaNo"));
	m_edtAreaName = static_cast<DuiLib::CEditUI *>(m_PaintManager.FindControl("edAreaName"));

	WindowImplBase::InitWindow();
}