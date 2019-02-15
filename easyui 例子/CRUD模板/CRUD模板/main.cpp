// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "ColumnItemDlg.h"


// menu
class CDuiMenu : public WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	CDuiString  m_strXMLPath;
	CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual CDuiString GetSkinFolder() { return _T(""); }
	virtual CDuiString GetSkinFile() { return m_strXMLPath; }
	virtual void       OnFinalMessage(HWND hWnd) { delete this; }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close();
		bHandled = FALSE;
		return 0;
	}

	void Init(HWND hWndParent, POINT ptPos)
	{
		Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual void  Notify(TNotifyUI& msg) {
		if (msg.sType == "itemclick") {
			if (m_pOwner) {
				m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
				this->PostMessage(WM_CLOSE);
			}
			return;
		}
		WindowImplBase::Notify(msg);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};




CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	m_list = (CListUI *)m_PaintManager.FindControl("lstDbColumns");
	m_edEntity = (CEditUI *)m_PaintManager.FindControl("edEntity");
	m_html = (CRichEditUI *)m_PaintManager.FindControl("rhHtml");
	m_javascript = (CRichEditUI *)m_PaintManager.FindControl("rhJavascript");
	m_java = (CRichEditUI *)m_PaintManager.FindControl("rhJava");
	m_edPool = (CEditUI *)m_PaintManager.FindControl("edPool");

#ifdef _DEBUG
	m_edPool->SetText("test");
	m_edEntity->SetText("user");

	CListTextElementUI * pItem;
	pItem = new CListTextElementUI;
	m_list->Add(pItem);
	pItem->SetText(0, "id");
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(CColumnItemWnd::INT) );

	pItem = new CListTextElementUI;
	m_list->Add(pItem);
	pItem->SetText(0, "firstname");
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(CColumnItemWnd::STRING));

	pItem = new CListTextElementUI;
	m_list->Add(pItem);
	pItem->SetText(0, "lastname");
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(CColumnItemWnd::STRING));

	pItem = new CListTextElementUI;
	m_list->Add(pItem);
	pItem->SetText(0, "phone");
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(CColumnItemWnd::STRING));

	pItem = new CListTextElementUI;
	m_list->Add(pItem);
	pItem->SetText(0, "email");
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(CColumnItemWnd::STRING));
#endif

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString strName = msg.pSender->GetName();
	if (msg.sType == "menu") {
		if (strName == "lstDbColumns") {
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

			pMenu->Init(*this, pt);
			pMenu->ShowWindow(TRUE);
			return;
		}
	}
	else if (msg.sType == "menu_add") {
		OnAdd();
	}
	else if (msg.sType == "menu_modify") {
		OnModify();
	}
	else if (msg.sType == "menu_delete") {
		OnDelete();
	}
	else if (msg.sType == "click") {
		if (strName == "btnGenerate" ) {
			OnGenerate();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void  CDuiFrameWnd::OnAdd() {
	CColumnItemWnd * pDlg = new CColumnItemWnd;
	pDlg->Create(this->m_hWnd, "", UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	pDlg->ShowModal();

	CListTextElementUI * pItem = new CListTextElementUI;
	m_list->Add(pItem);
	pItem->SetText(0, pDlg->m_szColumnName);
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(pDlg->m_eColumnType));

	delete pDlg;
}

void  CDuiFrameWnd::OnModify() {
	int nSel = m_list->GetCurSel();
	if ( nSel < 0 ) {
		return;
	}

	CListTextElementUI * pItem = (CListTextElementUI *)m_list->GetItemAt(nSel);

	CColumnItemWnd * pDlg = new CColumnItemWnd(FALSE);
	STRNCPY(pDlg->m_szColumnName, pItem->GetText(0), sizeof(pDlg->m_szColumnName));
	pDlg->m_eColumnType = CColumnItemWnd::String2ColumnType(pItem->GetText(1));

	pDlg->Create(this->m_hWnd, "", UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	pDlg->ShowModal();

	pItem->SetText(0, pDlg->m_szColumnName);
	pItem->SetText(1, CColumnItemWnd::ColumnType2String(pDlg->m_eColumnType));

	delete pDlg;
}

void  CDuiFrameWnd::OnDelete() {
	int nSel = m_list->GetCurSel();
	if (nSel < 0) {
		return;
	}

	m_list->RemoveAt(nSel);
}

void   CDuiFrameWnd::OnGenerate() {
	CDuiString  strEntity = m_edEntity->GetText();
	if ( strEntity.GetLength() == 0) {
		::MessageBox(GetHWND(), "请输入实体名称", "系统提示", 0);
		return;
	}

	if (m_list->GetCount() <= 1) {
		::MessageBox(GetHWND(), "请输入数据库设计对应的表的列", "系统提示", 0);
		return;
	}

	int cnt = m_list->GetCount();

	char  szEntity0[256];    // 全小写
	char  szEntity1[256];    // 第一个字符大写

	Str2Lower(strEntity, szEntity0, sizeof(szEntity0));
	STRNCPY(szEntity1, szEntity0, sizeof(szEntity1));
	szEntity1[0] = Char2Upper(szEntity1[0]);

	char szKey[256];
	CListTextElementUI * pKeyItem = (CListTextElementUI *)m_list->GetItemAt(0);
	STRNCPY(szKey, pKeyItem->GetText(0), sizeof(szKey));

	CDuiString  strLine;
	CDuiString  strHtml;	
	CDuiString  strJavascript;
	CDuiString  strJava;

	strLine.Format( "<table id=\"dg%s\" title=\"%s list\" class=\"easyui-datagrid\" style=\"width:700px;height:250px\" "
					"url=\"%s?type=list\" "
					"toolbar=\"#toolbar%s\" pagination=\"true\" "
					"rownumbers=\"true\" fitColumns=\"true\" singleSelect=\"true\">\r\n", szEntity1, szEntity0, szEntity0,
					 szEntity1 );
	strHtml += strLine;
	strHtml += "\t<thead>\r\n\t\t<tr>\r\n";

	for (int i = 0; i < cnt; i++) {
		CListTextElementUI * pItem = (CListTextElementUI *)m_list->GetItemAt(i);
		CDuiString  name = pItem->GetText(0);
		CColumnItemWnd::ColumnType type = CColumnItemWnd::String2ColumnType(pItem->GetText(1));

		if (i == 0) {
			if (type == CColumnItemWnd::INT) {
				strLine.Format("\t\t\t<th field=\"%s\" width=\"50\" hidden=\"true\">%s</th>\r\n", (const char *)name, (const char *)name);
			}
			else {
				strLine.Format("\t\t\t<th field=\"%s\" width=\"50\">%s</th>\r\n", (const char *)name, (const char *)name);
			}
		}
		else {
			strLine.Format("\t\t\t<th field=\"%s\" width=\"50\">%s</th>\r\n", (const char *)name, (const char *)name);
		}
		strHtml += strLine;
	}
	strHtml += "\t\t</tr>\r\n\t</thead>\r\n</table>\r\n";

	strLine.Format("<div id=\"toolbar%s\">\r\n", szEntity1);
	strHtml += strLine;

	strLine.Format("\t<a href=\"javascript:void(0)\" class=\"easyui-linkbutton\" iconCls=\"icon-add\" "
		           "plain=\"true\" onclick=\"new%s()\">New %s</a>\r\n", szEntity1, szEntity1 );
	strHtml += strLine;
	strLine.Format("\t<a href=\"javascript:void(0)\" class=\"easyui-linkbutton\" iconCls=\"icon-edit\" "
		"plain=\"true\" onclick=\"edit%s()\">Edit %s</a>\r\n", szEntity1, szEntity1);
	strHtml += strLine;
	strLine.Format("\t<a href=\"javascript:void(0)\" class=\"easyui-linkbutton\" iconCls=\"icon-remove\" "
		"plain=\"true\" onclick=\"destroy%s()\">Destroy %s</a>\r\n", szEntity1, szEntity1);
	strHtml += strLine;
	strHtml += "</div>\r\n";
	
	strLine.Format( "<div id=\"dlg%s\" class=\"easyui-dialog\" style=\"width:400px\" "
					"data-options=\"closed:true,modal:true,border:'thin',buttons:'#dlg%s-buttons'\">\r\n", szEntity1, szEntity1);
	strHtml += strLine;
	strLine.Format("\t<form id=\"fm%s\" method=\"post\" novalidate style=\"margin:0;padding:20px 50px\">\r\n", szEntity1);
	strHtml += strLine;
	strLine.Format("\t\t<h3>%s Information</h3>\r\n", szEntity1);
	strHtml += strLine;
	
	for (int i = 0; i < cnt; i++) {
		CListTextElementUI * pItem = (CListTextElementUI *)m_list->GetItemAt(i);
		CDuiString  name = pItem->GetText(0);
		CColumnItemWnd::ColumnType type = CColumnItemWnd::String2ColumnType(pItem->GetText(1));

		strLine = "";
		if ( i == 0 ) {
			if ( type == CColumnItemWnd::STRING ) {
				strLine.Format("\t\t<div style=\"margin-bottom:10px\">"
					"<input name=\"%s\" class=\"easyui-textbox\" required=\"true\" label=\"%s:\" style=\"width:100%%\">"
					"</div>\r\n", (const char *)name, (const char *)name);
			}
		}
		else {
			strLine.Format("\t\t<div style=\"margin-bottom:10px\">"
				"<input name=\"%s\" class=\"easyui-textbox\" required=\"true\" label=\"%s:\" style=\"width:100%%\">"
				"</div>\r\n", (const char *)name, (const char *)name);
		}		
		strHtml += strLine;
	}
	strHtml += "\t</form>\r\n</div>\r\n";

	strLine.Format("<div id=\"dlg%s-buttons\">\r\n", szEntity1);
	strHtml += strLine;

	strLine.Format("\t<a href=\"javascript:void(0)\" class=\"easyui-linkbutton c6\" iconCls=\"icon-ok\" "
					"onclick=\"save%s()\" style=\"width:90px\">Save</a>\r\n", szEntity1);
	strHtml += strLine;
	strLine.Format(	"\t<a href=\"javascript:void(0)\" class=\"easyui-linkbutton\" iconCls=\"icon-cancel\" "
					"onclick=\"javascript:$('#dlg%s').dialog('close')\" style=\"width:90px\">Cancel</a>\r\n", szEntity1);
	strHtml += strLine;
	strHtml += "</div>\r\n";

	m_html->SetText(strHtml);


	// javascript
	strLine.Format("var url%s;\r\n", szEntity1);
	strJavascript += strLine;

	strLine.Format( "function new%s(){ \r\n"
					"\t$('#dlg%s').dialog('open').dialog('center').dialog('setTitle', 'add %s');\r\n"
					"\t$('#fm%s').form('clear');\r\n"
					"\turl%s='%s?type=add';\r\n"
					"}\r\n", szEntity1, szEntity1, szEntity0, szEntity1, szEntity1, szEntity0);
	strJavascript += strLine;

	strLine.Format(	"function edit%s(){\r\n"
					"\tvar row=$('#dg%s').datagrid('getSelected');\r\n"
					"\tif (row) {\r\n"
					"\t\t$('#dlg%s').dialog('open').dialog('center').dialog('setTitle', 'edit %s');\r\n"
					"\t\t$('#fm%s').form('load', row);\r\n"
					"\t\turl%s='%s?type=modify&%s='+row.%s;\r\n"
					"\t}\r\n"
					"}\r\n", szEntity1, szEntity1, szEntity1, szEntity0, szEntity1, szEntity1, szEntity0, szKey,szKey);
	strJavascript += strLine;

	strLine.Format(	"function save%s(){\r\n"
					"\t$('#fm%s').form('submit',{\r\n"
					"\t\turl:url%s,\r\n"
					"\t\tonSubmit:function(){\r\n"
					"\t\t\treturn $(this).form('validate');\r\n"
					"\t\t},\r\n"
					"\t\tsuccess:function(result){\r\n"
					"\t\t\tvar result=eval('('+result+')');\r\n"
					"\t\t\tif(result.errorMsg){\r\n"
					"\t\t\t\t$.messager.show({\r\n"
					"\t\t\t\t\ttitle: 'system alert',\r\n"
					"\t\t\t\t\tmsg : result.errorMsg\r\n"
					"\t\t\t\t});\r\n"
					"\t\t\t} else {\r\n"
					"\t\t\t\t$('#dlg%s').dialog('close');\r\n"
					"\t\t\t\t$('#dg%s').datagrid('reload');\r\n"
					"\t\t\t}\r\n"
					"\t\t}\r\n"
					"\t});\r\n"
					"}\r\n", szEntity1, szEntity1, szEntity1, szEntity1, szEntity1 );
	strJavascript += strLine;

	strLine.Format(	"function destroy%s(){\r\n"
					"\tvar row=$('#dg%s').datagrid('getSelected');\r\n"
					"\tif (row) {\r\n"
					"\t\t$.messager.confirm('system alert', 'are you sure to delete this record?', function(r){\r\n"
					"\t\t\tif (r) {\r\n"
					"\t\t\t\t$.post('%s?type=delete',{%s:row.%s},function(result){ \r\n"
					"\t\t\t\t\tif (result.success) {\r\n"
					"\t\t\t\t\t\t$('#dg%s').datagrid('reload');\r\n"
					"\t\t\t\t\t } else {\r\n"
					"\t\t\t\t\t\t$.messager.show({ \r\n"
					"\t\t\t\t\t\t\ttitle: 'system alert',\r\n"
					"\t\t\t\t\t\t\tmsg : result.errorMsg\r\n"
					"\t\t\t\t\t\t});\r\n"
					"\t\t\t\t\t}\r\n"
					"\t\t\t\t}, 'json');\r\n"
					"\t\t\t}\r\n"
					"\t\t});\r\n"
					"\t}\r\n"
					"}\r\n", szEntity1, szEntity1, szEntity0, szKey, szKey, szEntity1);
	strJavascript += strLine;
	m_javascript->SetText(strJavascript);

	// java
	HRSRC hRes = ::FindResource(0, MAKEINTRESOURCE(IDR_TEMPLATE1), "TEMPLATE");
	DWORD  dwSize = ::SizeofResource(0, hRes);
	DWORD  dwNewSize = dwSize + 2000;
	char * szTemplate = new char[dwNewSize];

	HGLOBAL h = LoadResource(0, hRes);
	LPVOID p = LockResource(h);
	memcpy(szTemplate, p, dwSize);
	szTemplate[dwSize] = '\0';
	FreeResource(h);

	CDuiString strPool = m_edPool->GetText();
	char * szJava = new char[dwNewSize];

	StrReplaceAll(szJava,     dwNewSize, szTemplate, "<%entity1%>", szEntity1 );
	StrReplaceAll(szTemplate, dwNewSize, szJava,     "<%entity%>",  szEntity0 );

	CDuiString  strColumns;
	for (int i = 0; i < cnt; i++) {
		CListTextElementUI * pItem = (CListTextElementUI *)m_list->GetItemAt(i);
		CDuiString  name = pItem->GetText(0);
		char szType[64];
		Str2Upper(pItem->GetText(1),szType, sizeof(szType));

		strLine.Format("new ColumnInfo(\"%s\",ColumnType.%s)", (const char *)name, szType );
		if ( i > 0 ) {
			strColumns += ",";
		}
		strColumns += strLine;
	}

	StrReplaceFirst(szJava,     dwNewSize, szTemplate, "<%columns%>", (const char *)strColumns );
	StrReplaceFirst(szTemplate, dwNewSize, szJava,     "<%pool%>",    (const char *)strPool );

	m_java->SetText(szTemplate);

	delete[] szTemplate;
	delete[] szJava;
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


