#include "ColumnItemDlg.h"

CColumnItemWnd::CColumnItemWnd(BOOL  bAdd /*= TRUE*/) {
	memset(m_szColumnName, 0, sizeof(m_szColumnName));
	m_eColumnType = ColumnType::INT;
	m_bAdd = bAdd;
}

void  CColumnItemWnd::Notify(TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (strName == "btnItemOk") {
			OnMyOk();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CColumnItemWnd::InitWindow() {
	m_name = (CEditUI *)m_PaintManager.FindControl("edName");
	m_type = (CComboUI *)m_PaintManager.FindControl("cmbType");

	CListLabelElementUI * pComboItem;
	pComboItem = new CListLabelElementUI;
	pComboItem->SetText("字符串");
	m_type->Add(pComboItem);

	pComboItem = new CListLabelElementUI;
	pComboItem->SetText("整型");
	m_type->Add(pComboItem);
	m_type->SelectItem(0);

	WindowImplBase::InitWindow();
}

void  CColumnItemWnd::OnMyOk() {
	CDuiString strName = m_name->GetText();
	m_eColumnType = (ColumnType)m_type->GetCurSel();

	if ( strName.GetLength() == 0 ) {
		MessageBox(GetHWND(), "请输入名称", "系统提示", 0);
		return;
	}

	PostMessage(WM_CLOSE);
}