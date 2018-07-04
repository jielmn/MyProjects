#include "MyTreeCfgUI.h"

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
	

CMyTreeCfgUI::ConfigValue::ConfigValue() {
	m_nComboSel = -1;
	m_bCheckbox = FALSE;
	m_eConfigType = ConfigType_NONE;
}



















CMyTreeCfgUI::CMyTreeCfgUI() : _root(NULL), m_dwDelayDeltaY(0), m_dwDelayNum(0), m_dwDelayLeft(0)
{
	SetItemShowHtml(true);

	_root = new Node;
	_root->data()._level = -1;
	_root->data()._expand = true;
	_root->data()._pListElement = NULL;
	_root->data()._pUserData = 0;

	m_dwFixedLeft = 160;

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
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;

	if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()) == NULL)
		return CListUI::RemoveAt(iIndex, bDoNotDestroy);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()));
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

CMyTreeCfgUI::Node* CMyTreeCfgUI::AddNode( LPCTSTR text, Node* parent /*= NULL*/, void * pUserData /*= 0*/, CControlUI * pConfig /*= NULL*/ )
{
	if (!parent) parent = _root;

	CListContainerElementUI* pListContainerElement = new CListContainerElementUI;
	pListContainerElement->SetMinHeight(26);

	CHorizontalLayoutUI * pLayout = new CHorizontalLayoutUI;
	pListContainerElement->Add(pLayout);

	CControlUI * place_holder = new CControlUI;
	//place_holder->SetFixedWidth(0);
	pLayout->Add(place_holder);

	CLabelUI * pTitle = new CLabelUI;
	pLayout->Add(pTitle);
	pTitle->SetText(text);

	if (pConfig) {
		pConfig->SetAttribute("padding", "10,3,10,3");
		pLayout->Add(pConfig);
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
			CButtonUI * btn_expand = new CButtonUI;
			btn_expand->SetFixedWidth(15);
			btn_expand->SetFixedHeight(15);
			btn_expand->SetBkImage("expand.png");
			btn_expand->SetAttribute("padding", "0,5,4,5");
			layout_parent->AddAt(btn_expand, 1);
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
	return true;
}

void CMyTreeCfgUI::ExpandNode(CMyTreeCfgUI::Node* node, bool expand)
{
	if (!node || node == _root) return;

	if (node->data()._expand == expand) return;
	node->data()._expand = expand;

	CHorizontalLayoutUI * pLayout = (CHorizontalLayoutUI *)node->data()._pListElement->GetItemAt(0);
	CButtonUI * btnExpand = (CButtonUI *)pLayout->GetItemAt(1);

	if (expand) {
		btnExpand->SetBkImage("expand.png");
	}
	else {
		btnExpand->SetBkImage("collapse.png");
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
	NeedUpdate();
}

SIZE CMyTreeCfgUI::GetExpanderSizeX(CMyTreeCfgUI::Node* node) const
{
	if (!node || node == _root) return CDuiSize();
	//if (node->data()._level >= 3) return CDuiSize();

	SIZE szExpander = { 0 };
	szExpander.cx = 6 + 24 * node->data()._level - 4/*�ʵ��Ŵ�һ��*/;
	szExpander.cy = szExpander.cx + 16 + 8/*�ʵ��Ŵ�һ��*/;
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
		int nHeight = this->GetItemAt(i)->GetHeight();
		h += nHeight;
		::MoveToEx(hDC, r.left, h, 0);
		::LineTo(hDC, r.right - 1, h);
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

	if ( pLayout->GetCount() < 3 ) {
		return false;
	}

	CControlUI * pCtl = pLayout->GetItemAt(2);
	if ( 0 == strcmp( pCtl->GetClass(),DUI_CTR_EDIT) ) {
		cfgValue.m_eConfigType = ConfigType_EDIT;
		cfgValue.m_strEdit = pCtl->GetText();
	}
	else if ( 0 == strcmp( pCtl->GetClass(),DUI_CTR_COMBO ) ) {
		cfgValue.m_eConfigType = ConfigType_COMBO;
		cfgValue.m_nComboSel = ((CComboUI*)pCtl)->GetCurSel();
	}
	else if ( 0 == strcmp( pCtl->GetClass(),DUI_CTR_CHECKBOX) ) {
		cfgValue.m_eConfigType = ConfigType_CHECKBOX;
		cfgValue.m_bCheckbox = ((CCheckBoxUI*)pCtl)->IsSelected();
	}
	else {
		return false;
	}

	return true;
}