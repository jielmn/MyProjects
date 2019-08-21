#include "MyTreeCfgUI.h"
#include <time.h>
#pragma comment(lib,"Gdi32.lib")

void CMyTreeCfgUI::Node::set_parent(CMyTreeCfgUI::Node* parent) {
	_parent = parent;
}
		
CMyTreeCfgUI::Node::Node() : _parent(NULL) {  }

CMyTreeCfgUI::Node::~Node()
{
	for (int i = 0; i < num_children(); i++)
		delete _children[i];
}

CMyTreeCfgUI::NodeData& CMyTreeCfgUI::Node::data() { return _data; }

int CMyTreeCfgUI::Node::num_children() const { return _children.size(); }

CMyTreeCfgUI::Node* CMyTreeCfgUI::Node::child(int i) { return _children[i]; }

CMyTreeCfgUI::Node* CMyTreeCfgUI::Node::parent() { return (_parent); }

bool CMyTreeCfgUI::Node::has_children() const { return num_children() > 0; }

void CMyTreeCfgUI::Node::add_child(Node* child)
{
	child->set_parent(this);
	_children.push_back(child);
}

void CMyTreeCfgUI::Node::remove_child(CMyTreeCfgUI::Node* child)
{
	Children::iterator iter = _children.begin();
	for (; iter < _children.end(); ++iter)
	{
		if (*iter == child)
		{
			_children.erase(iter);
			return;
		}
	}
}

void CMyTreeCfgUI::Node::remove_all_child() {
	_children.clear();
}

CMyTreeCfgUI::Node* CMyTreeCfgUI::Node::get_last_child()
{
	if (has_children())
	{
		return child(num_children() - 1)->get_last_child();
	}
	else return this;  
}

bool CMyTreeCfgUI::Node::IsAllParentsExpanded() {
	if ( _parent ) {
		if ( !_parent->data()._expand ) {
			return false;
		}
		else {
			return _parent->IsAllParentsExpanded();
		}
	}
	else {
		return true;
	}
}
	

CMyTreeCfgUI::ConfigValue::ConfigValue() {
	m_nComboSel = -1;
	m_bCheckbox = FALSE;
	m_eConfigType = ConfigType_NONE;
}



class CMyExpandButtonUI : public CButtonUI {
public:
	CMyExpandButtonUI( CMyTreeCfgUI * p ) {
		sigClick.connect(p, &CMyTreeCfgUI::OnExpandClick);
	}

	void DoEvent(TEventUI& event) {	
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{			
			sigClick.emit(this);
			return;
		}
		CButtonUI::DoEvent(event);
	}

	LPCTSTR GetClass() const {
		return "MyExpandButton";
	}

private:
	sigslot::signal1<CControlUI *>     sigClick;
};















CMyTreeCfgUI::CMyTreeCfgUI(DWORD  dwFixedLeft /*= 160*/) : _root(NULL), m_dwDelayDeltaY(0), m_dwDelayNum(0), m_dwDelayLeft(0)
{
	SetItemShowHtml(true);

	_root = new Node;
	_root->data()._level = -1;
	_root->data()._expand = true;
	_root->data()._pListElement = NULL;
	_root->data()._pUserData = 0;

	m_dwFixedLeft = dwFixedLeft;
	m_dwFixedItemHeight = 26;
	m_hPen = CreatePen(PS_DOT, 1, RGB(0x99, 0x99, 0x99));
}

CMyTreeCfgUI::~CMyTreeCfgUI() { 
	if (_root) delete _root; 
	DeleteObject(m_hPen);
}

bool CMyTreeCfgUI::Add(CControlUI* pControl)
{
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;
	return CListUI::Add(pControl);
}

bool CMyTreeCfgUI::AddAt(CControlUI* pControl, int iIndex)
{
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;
	return CListUI::AddAt(pControl, iIndex);
}

bool CMyTreeCfgUI::Remove(CControlUI* pControl, bool bDoNotDestroy /*= false*/)
{
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;

	if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()) == NULL)
		return CListUI::Remove(pControl, bDoNotDestroy);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()));
}

bool CMyTreeCfgUI::RemoveAt(int iIndex, bool bDoNotDestroy /*= false*/)
{
	//CControlUI* pControl = GetItemAt(iIndex);
	//if (!pControl) return false;
	//if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;

	//if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()) == NULL)
	//	return CListUI::RemoveAt(iIndex, bDoNotDestroy);
	//else
	//	return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()));

	CControlUI* pControl = GetItemAt(iIndex);
	if (0 == pControl) return true;
	Node* node = (Node*)pControl->GetTag();
	RemoveNode(node);
	return true;
}

void CMyTreeCfgUI::RemoveAll()
{
	CListUI::RemoveAll();
	for (int i = 0; i < _root->num_children(); ++i)
	{
		Node* child = _root->child(i);
		RemoveNode(child);
	}
	delete _root;
	_root = new Node;
	_root->data()._level = -1;
	_root->data()._expand = true;
	_root->data()._pListElement = NULL;
}
void CMyTreeCfgUI::SetVisible(bool bVisible /*= true*/)
{
	if (m_bVisible == bVisible) return;
	CControlUI::SetVisible(bVisible);
}

void CMyTreeCfgUI::SetInternVisible(bool bVisible /*= true*/)
{
	CControlUI::SetInternVisible(bVisible);
}

void CMyTreeCfgUI::DoEvent(TEventUI& event)
{
	CListUI::DoEvent(event);
}

CMyTreeCfgUI::Node* CMyTreeCfgUI::GetRoot() { return _root; }

CMyTreeCfgUI::Node* CMyTreeCfgUI::AddNode( LPCTSTR text, Node* parent /*= NULL*/, 
	             void * pUserData /*= 0*/, CControlUI * pConfig /*= NULL*/, DWORD dwTitleFontIndex /*= -1*/, 
	DWORD dwTitleColor /*= 0xFF000000*/, DWORD  dwCfgFontIndex /*= -1*/, DWORD dwCfgColor /*= 0xFF000000*/, DWORD dwItemHeight /*= 0*/)
{
	if (!parent) parent = _root;

	CListContainerElementUI* pListContainerElement = new CListContainerElementUI;
	if (0 == dwItemHeight) {
		pListContainerElement->SetMinHeight(m_dwFixedItemHeight);
		//pListContainerElement->SetTag(m_dwFixedItemHeight);
	}
	else {
		pListContainerElement->SetMinHeight(dwItemHeight);
		//pListContainerElement->SetTag(dwItemHeight);
	}

	CHorizontalLayoutUI * pLayout = new CHorizontalLayoutUI;
	pListContainerElement->Add(pLayout);

	CControlUI * place_holder = new CControlUI;
	//place_holder->SetFixedWidth(0);
	pLayout->Add(place_holder);

	CLabelUI * pTitle = new CLabelUI;
	pTitle->SetFont(dwTitleFontIndex);
	pTitle->SetTextColor(dwTitleColor);
	pLayout->Add(pTitle);
	pTitle->SetText(text);

	if (pConfig) {
		pConfig->SetAttribute("padding", "10,3,10,3");
		pLayout->Add(pConfig);

		if ( 0 == strcmp( pConfig->GetClass(),DUI_CTR_EDIT ) ) {
			CEditUI * pEdit = (CEditUI *)pConfig;
			pEdit->SetFont(dwCfgFontIndex);
			pEdit->SetTextColor(dwCfgColor);
		}
		else if ( 0 == strcmp(pConfig->GetClass(), DUI_CTR_COMBO) ) {
			CComboUI * pCombo = (CComboUI *)pConfig;
			pCombo->SetItemFont(dwCfgFontIndex);
			pCombo->SetSelectedItemTextColor(dwCfgColor);
			pCombo->SetItemTextColor(dwCfgColor);
			pCombo->SetAttributeList("hotimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");
		}
		else if ( 0 == strcmp(pConfig->GetClass(), DUI_CTR_CHECKBOX) ) {
			CCheckBoxUI * pCheckBox = (CCheckBoxUI *)pConfig;
			pCheckBox->SetAttribute("normalimage", "file='checkbox_unchecked.png' dest='0,2,16,18'");
			pCheckBox->SetAttribute("selectedimage", "file='checkbox_checked.png' dest='0,2,16,18'");
		}
		else if (0 == strcmp(pConfig->GetClass(), DUI_CTR_FILEBROWSE)) {
			CFileBrowseUI * pFileBrowse = (CFileBrowseUI *)pConfig;
			pFileBrowse->SetAttribute("normalimage", "file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
			pFileBrowse->SetAttribute("hotimage", "file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
			pFileBrowse->SetAttribute("pushedimage", "file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
		}
		else if (0 == strcmp(pConfig->GetClass(), DUI_CTR_DATETIME)) {
			CDateTimeUI * pDateTime = (CDateTimeUI *)pConfig;
			RECT r = { 5,0,5,0 };
			pDateTime->SetTextPadding( r );
			pDateTime->SetFont(dwCfgFontIndex);
			pDateTime->SetTextColor(dwCfgColor);
		}
		else {
			assert(0);
		}
	}
	
	Node* node = new Node;
	memset(&node->data(), 0, sizeof(NodeData));

	node->data()._level = parent->data()._level + 1;
	if (node->data()._level == 0) node->data()._expand = true;
	else node->data()._expand = false;
	node->data()._pListElement = pListContainerElement;
	node->data()._pUserData = pUserData;	
	node->data()._expand = true;

	int nPlaceHolderWidth = 2;
	for (int i = 0; i < node->data()._level; ++i) {
		nPlaceHolderWidth += 12;
	}
	place_holder->SetFixedWidth(nPlaceHolderWidth);
	if ( m_dwFixedLeft > (DWORD)nPlaceHolderWidth ) {
		pTitle->SetFixedWidth(m_dwFixedLeft - nPlaceHolderWidth);
	}

	if (!parent->has_children()) {
		CListContainerElementUI * parent_element = parent->data()._pListElement;
		if (parent_element) {
			CHorizontalLayoutUI * layout_parent = (CHorizontalLayoutUI *)parent_element->GetItemAt(0);
			CButtonUI * btn_expand = new CMyExpandButtonUI(this);
			btn_expand->SetFixedWidth(15);
			btn_expand->SetFixedHeight(15);
			btn_expand->SetBkImage("collapse.png");
			btn_expand->SetAttribute("padding", "0,5,4,5");
			layout_parent->AddAt(btn_expand, 1);

			// 缩减title的宽度
			CControlUI * parent_title = layout_parent->GetItemAt(2);
			int title_width = parent_title->GetFixedWidth();
			if (title_width >= 20) {
				parent_title->SetFixedWidth(title_width - 20);
			}
		}
	}

	pListContainerElement->SetTag((UINT_PTR)node);
	int index = 0;
	if (parent->has_children()) {
		Node* prev = parent->get_last_child();
		index = prev->data()._pListElement->GetIndex() + 1;
	}
	else {
		if (parent == _root) index = 0;
		else index = parent->data()._pListElement->GetIndex() + 1;
	}
	if (!CListUI::AddAt(pListContainerElement, index)) {
		delete pListContainerElement;
		delete node;
		node = NULL;
	}

	parent->add_child(node);
	SetFixedHeight(CalculateMinHeight());

	return node;
}

bool CMyTreeCfgUI::RemoveNode(CMyTreeCfgUI::Node* node)
{
	if (!node || node == _root) return false;
	while (node->num_children()>0) {
		Node* child = node->child(0);
		RemoveNode(child);
	}
	CListUI::Remove(node->data()._pListElement);
	node->parent()->remove_child(node);

	if (!node->parent()->has_children()) {
		if (node->parent()->data()._pListElement) {
			CDuiString strText = node->parent()->data()._pListElement->GetText();
			int pos = strText.Find("<v center>");
			if (pos >= 0) {
				CDuiString strNewText = strText.Mid(0, pos) + strText.Mid(pos + 44);
				node->parent()->data()._pListElement->SetText(strNewText);
			}
		}		
	}
	
	delete node;

	SetFixedHeight(CalculateMinHeight());
	return true;
}

void CMyTreeCfgUI::ExpandNode(CMyTreeCfgUI::Node* node, bool expand)
{
	if (!node || node == _root) return;
	if (!node->has_children()) return;


	if (node->data()._expand == expand) return;
	node->data()._expand = expand;

	CHorizontalLayoutUI * pLayout = (CHorizontalLayoutUI *)node->data()._pListElement->GetItemAt(0);
	CButtonUI * btnExpand = (CButtonUI *)pLayout->GetItemAt(1);

	if (expand) {
		btnExpand->SetBkImage("collapse.png");
	}
	else {
		btnExpand->SetBkImage("expand.png");
	}

	if (!node->data()._pListElement->IsVisible()) return;
	if (!node->has_children()) return;

	Node* begin = node->child(0);
	Node* end = node->get_last_child();
	for (int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i) {
		CControlUI* control = GetItemAt(i);
		if (_tcscmp(control->GetClass(), DUI_CTR_LISTCONTAINERELEMENT) == 0) {
			Node* local_parent = ((CMyTreeCfgUI::Node*)control->GetTag())->parent();
			control->SetInternVisible(local_parent->data()._expand && local_parent->data()._pListElement->IsVisible());
		}
	}
	SetFixedHeight(CalculateMinHeight());
	NeedUpdate();	
}

SIZE CMyTreeCfgUI::GetExpanderSizeX(CMyTreeCfgUI::Node* node) const
{
	if (!node || node == _root) return CDuiSize();
	//if (node->data()._level >= 3) return CDuiSize();

	SIZE szExpander = { 0 };
	szExpander.cx = 6 + 24 * node->data()._level - 4/*适当放大一点*/;
	szExpander.cy = szExpander.cx + 16 + 8/*适当放大一点*/;
	return szExpander;
}


int CMyTreeCfgUI::GetItemIndex(CControlUI* pControl) const {
	int index = CListUI::GetItemIndex(pControl);
	if (index != -1) {
		return index;
	}

	int nCnt = this->GetCount();
	for ( int i = 0; i < nCnt; i++ ) {
		CListContainerElementUI * pContainer = (CListContainerElementUI *)GetItemAt(i);
		CHorizontalLayoutUI * pLayout =  (CHorizontalLayoutUI *)pContainer->GetItemAt(0);
		if (pLayout == pControl) {
			return i;
		}

		int nLayoutCnt = pLayout->GetCount();
		for (int j = 0; j < nLayoutCnt; j++) {
			if (pLayout->GetItemAt(j) == pControl) {
				return i;
			}
		}
	}

	return -1;
}

int   CMyTreeCfgUI::CalculateMinHeight() {
	int nCount = this->GetCount();
	int sum = 0;

	for (int i = 0; i < nCount; i++) {
		Node* node = (Node*)this->GetItemAt(i)->GetTag();
		if ( node->IsAllParentsExpanded() ) {
			//int nHeight = this->GetItemAt(i)->GetHeight();
			//int nHeight = m_dwFixedItemHeight; 
			int nHeight = node->data()._pListElement->GetMinHeight();
			sum += nHeight;
		}
	}

	return sum + 2;
}

bool CMyTreeCfgUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CListUI::DoPaint(hDC, rcPaint, pStopControl);

	HPEN hOldPen = (HPEN)::SelectObject(hDC, m_hPen);

	RECT r = this->GetPos();
	//::MoveToEx(hDC, r.left, r.top, 0);
	//::LineTo(hDC, r.right-1, r.top);
	//::LineTo(hDC, r.right-1, r.bottom-1);
	//::LineTo(hDC, r.left, r.bottom-1);
	//::LineTo(hDC, r.left, r.top);

	int nCount = this->GetCount();
	int h = r.top;
	for (int i = 0; i < nCount; i++) {
		if (0 == i) {
			::MoveToEx(hDC, r.left, h, 0);
			::LineTo(hDC, r.right - 1, h);
		}
		if (this->GetItemAt(i)->IsVisible()) {
			int nHeight = this->GetItemAt(i)->GetHeight();
			h += nHeight;
			::MoveToEx(hDC, r.left, h, 0);
			::LineTo(hDC, r.right - 1, h); 
		}		
	}

	::MoveToEx(hDC, r.left + m_dwFixedLeft, r.top, 0);
	::LineTo(hDC, r.left + m_dwFixedLeft, h);

	::SelectObject(hDC, hOldPen);
	return true;
}

bool  CMyTreeCfgUI::GetConfigValue(int nIndex, ConfigValue & cfgValue) {
	if (nIndex < 0) {
		return false;
	}

	int nCount = this->GetCount();
	if (nIndex >= nCount) {
		return false;
	}

	CListContainerElementUI * pContainer =  (CListContainerElementUI *)this->GetItemAt(nIndex);
	CHorizontalLayoutUI * pLayout = (CHorizontalLayoutUI *)pContainer->GetItemAt(0);
	// place_holder + expand_button + title + [config],   可伸展的父节点
	// 或者 place_holder + title + config,  叶子节点
	if ( pLayout->GetCount() < 3 ) {
		return false;
	}

	BOOL  bLeaf = TRUE;
	CControlUI * pCtl = 0;

	pCtl = pLayout->GetItemAt(1);
	// 如果有伸展按钮
	if ( 0 == strcmp(pCtl->GetClass(), "MyExpandButton") ) {
		bLeaf = FALSE;
	}

	// 如果是叶子节点
	if (bLeaf) {
		pCtl = pLayout->GetItemAt(2);
	}
	// 如果是父节点
	else {
		if (pLayout->GetCount() > 3)
			pCtl = pLayout->GetItemAt(3);
		else
			return false;
	}		

	if ( 0 == strcmp( pCtl->GetClass(),DUI_CTR_EDIT) ) {
		cfgValue.m_eConfigType = ConfigType_EDIT;
		cfgValue.m_strEdit = pCtl->GetText();
		cfgValue.m_tag = pCtl->GetTag();
	}
	else if ( 0 == strcmp( pCtl->GetClass(),DUI_CTR_COMBO ) ) {
		CComboUI * pCombo = (CComboUI*)pCtl;
		cfgValue.m_eConfigType = ConfigType_COMBO;
		cfgValue.m_nComboSel = ((CComboUI*)pCtl)->GetCurSel();
		cfgValue.m_strEdit = ((CComboUI*)pCtl)->GetText();
		if (cfgValue.m_nComboSel >= 0) {
			cfgValue.m_tag = pCombo->GetItemAt(cfgValue.m_nComboSel)->GetTag();
		}
		else {
			cfgValue.m_tag = 0;
		}
	}
	else if ( 0 == strcmp( pCtl->GetClass(),DUI_CTR_CHECKBOX) ) {
		cfgValue.m_eConfigType = ConfigType_CHECKBOX;
		cfgValue.m_bCheckbox = ((CCheckBoxUI*)pCtl)->IsSelected();
		cfgValue.m_tag = pCtl->GetTag();
	}
	else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_FILEBROWSE)) {
		cfgValue.m_eConfigType = ConfigType_FileBrowse;
		cfgValue.m_strEdit = ((CFileBrowseUI*)pCtl)->GetFileName();
		cfgValue.m_tag = pCtl->GetTag();
	}
	else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_DATETIME)) {
		cfgValue.m_eConfigType = ConfigType_DateTime;
		SYSTEMTIME t1 = ((CDateTimeUI*)pCtl)->GetTime();

		struct tm t2;
		memset(&t2, 0, sizeof(t2));
		t2.tm_year = t1.wYear - 1900;
		t2.tm_mon = t1.wMonth - 1;
		t2.tm_mday = t1.wDay;
		cfgValue.m_time = mktime(&t2);

		cfgValue.m_tag = pCtl->GetTag();
	}
	else {
		assert(0);
		return false;
	}

	return true;
}

bool CMyTreeCfgUI::SetConfigValue(int nIndex, const ConfigValue & cfgValue) {
	if (nIndex < 0) {
		return false;
	}

	int nCount = this->GetCount();
	if (nIndex >= nCount) {
		return false;
	}

	CListContainerElementUI * pContainer = (CListContainerElementUI *)this->GetItemAt(nIndex);
	CHorizontalLayoutUI * pLayout = (CHorizontalLayoutUI *)pContainer->GetItemAt(0);

	if (pLayout->GetCount() < 3) {
		return false;
	}

	CControlUI * pCtl = pLayout->GetItemAt(2);
	if (0 == strcmp(pCtl->GetClass(), DUI_CTR_EDIT)) {
		pCtl->SetText(cfgValue.m_strEdit);
	}
	else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_COMBO)) {
		CComboUI * pCombo = (CComboUI*)pCtl;
		pCombo->SelectItem(cfgValue.m_nComboSel, false, false);
	}
	else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_CHECKBOX)) {
		CCheckBoxUI * pCheckUI = (CCheckBoxUI *)pCtl;
		if (cfgValue.m_bCheckbox)
			pCheckUI->SetCheck(true, false);
		else
			pCheckUI->SetCheck(false, false);
	}
	else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_FILEBROWSE)) {
		CFileBrowseUI * pFileBrowser = (CFileBrowseUI *)pCtl;
		pFileBrowser->SetFileName(cfgValue.m_strEdit);
	}
	else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_DATETIME)) {
		CDateTimeUI * pDateTime = (CDateTimeUI *)pCtl;

		struct tm  tmp;
		localtime_s(&tmp, &cfgValue.m_time);

		SYSTEMTIME  st;
		memset(&st, 0, sizeof(st));
		st.wYear  = tmp.tm_year + 1900;
		st.wMonth = tmp.tm_mon + 1;
		st.wDay   = tmp.tm_mday;
		st.wHour = tmp.tm_hour;
		st.wMinute = tmp.tm_min;
		st.wSecond = tmp.tm_sec;

		pDateTime->SetTime(&st);

		char buf[256];
		snprintf(buf, sizeof(buf), "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
		pDateTime->SetText(buf);
	}
	else {
		assert(0);
		return false;
	}

	return true;
}

void  CMyTreeCfgUI::OnExpandClick(CControlUI* pSender) {
	CMyTreeCfgUI::Node* node = (CMyTreeCfgUI::Node*)pSender->GetParent()->GetParent()->GetTag();
	ExpandNode(node, !node->data()._expand);	
}

void  CMyTreeCfgUI::SetTitle(int nIndex, LPCTSTR pTitle) {
	if (nIndex < 0) {
		return;
	}

	int nCount = this->GetCount();
	if (nIndex >= nCount) {
		return;
	}

	CListContainerElementUI * pContainer = (CListContainerElementUI *)this->GetItemAt(nIndex);
	CHorizontalLayoutUI * pLayout = (CHorizontalLayoutUI *)pContainer->GetItemAt(0);

	nCount = pLayout->GetCount();
	if (nCount < 2) {
		return;
	}

	for (int i = 1; i < nCount; i++) {
		CControlUI * pCtl = pLayout->GetItemAt(i);
		if ( 0 == strcmp(pCtl->GetClass(), DUI_CTR_LABEL) ) {
			CLabelUI * pLbl = (CLabelUI *)pCtl;
			pLbl->SetText(pTitle);
			break;
		}
	}
}