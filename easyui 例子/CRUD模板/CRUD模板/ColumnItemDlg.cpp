#include "ColumnItemDlg.h"
#include "LmnString.h"

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

	if (!m_bAdd) {
		m_name->SetText( m_szColumnName );
		m_type->SelectItem((int)m_eColumnType);  
	}

	WindowImplBase::InitWindow();
}

void  CColumnItemWnd::OnMyOk() {
	CDuiString strName = m_name->GetText();
	if ( strName.GetLength() == 0 ) {
		MessageBox(GetHWND(), "请输入名称", "系统提示", 0);
		return;
	}

	STRNCPY(m_szColumnName, strName, sizeof(m_szColumnName));
	m_eColumnType = (ColumnType)m_type->GetCurSel();
	PostMessage(WM_CLOSE);
}

const char * CColumnItemWnd::ColumnType2String(ColumnType e) {
	switch (e)
	{
	case CColumnItemWnd::STRING:
		return "string";
		break;
	case CColumnItemWnd::INT:
		return "int";
		break;
	default:
		break;
	}

	return "unknown";
}

CColumnItemWnd::ColumnType CColumnItemWnd::String2ColumnType(const char * szType) {
	if ( 0 == StrICmp(szType, "string") ) {
		return ColumnType::STRING;
	}
	else if (0 == StrICmp(szType, "int")) {
		return ColumnType::INT;
	}
	else {
		return ColumnType::STRING;
	}
}