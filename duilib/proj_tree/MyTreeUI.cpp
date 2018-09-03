#include "MyTreeUI.h"
#include "LmnCommon.h"

void CMyTreeUI::Node::set_parent(CMyTreeUI::Node* parent) {
	_parent = parent;
}
		
CMyTreeUI::Node::Node() : _parent(NULL) {  }

CMyTreeUI::Node::~Node()
{
	for (int i = 0; i < num_children(); i++)
		delete _children[i];
}

CMyTreeUI::NodeData& CMyTreeUI::Node::data() { return _data; }

int CMyTreeUI::Node::num_children() const { return _children.size(); }

CMyTreeUI::Node* CMyTreeUI::Node::child(int i) { return _children[i]; }

CMyTreeUI::Node* CMyTreeUI::Node::parent() { return (_parent); }

bool CMyTreeUI::Node::has_children() const { return num_children() > 0; }

void CMyTreeUI::Node::add_child(Node* child)
{
	child->set_parent(this);
	_children.push_back(child);
}

void CMyTreeUI::Node::remove_child(CMyTreeUI::Node* child)
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

void CMyTreeUI::Node::remove_all_child() {
	_children.clear();
}

CMyTreeUI::Node* CMyTreeUI::Node::get_last_child()
{
	if (has_children())
	{
		return child(num_children() - 1)->get_last_child();
	}
	else return this;  
}

bool CMyTreeUI::Node::IsAllParentsExpanded() {
	if (_parent) {
		if (!_parent->data()._expand) {
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
	


class CMyListElement : public CListLabelElementUI {
public:
	CMyListElement(CMyTreeUI * p) {
		m_tree = p;
	}

	void DoEvent(TEventUI& event) {
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
			pt.x -= GetX();
			pt.y -= GetY();

			CMyTreeUI::Node* node = (CMyTreeUI::Node*)GetTag();
			SIZE sz = m_tree->GetExpanderSizeX(node);
			SIZE sy = m_tree->GetExpanderSizeY(node);
			if ( pt.x >= sz.cx && pt.x < sz.cy && pt.y >= sy.cx && pt.y < sz.cy )
				m_tree->ExpandNode(node, !node->data()._expand);
		}
		CListLabelElementUI::DoEvent(event);
	}

private:
	CMyTreeUI *                        m_tree;
};



















CMyTreeUI::CMyTreeUI(DWORD dwFixedItemHeight /*= 26*/, const char * szRootBkImage /*= 0 */, 
	DWORD dwItemFont /*= -1*/, DWORD dwItemTextColor /*= 0xFF000000*/, 
	DWORD dwSelectedItemTextColor /*= 0xFF000000*/, DWORD dwHotItemTextColor /*= 0xFF000000*/ )
	: _root(NULL), m_dwDelayDeltaY(0), m_dwDelayNum(0), m_dwDelayLeft(0)
{
	SetItemShowHtml(true);

	_root = new Node;
	_root->data()._level = -1;
	_root->data()._expand = true;
	_root->data()._pListElement = NULL;
	_root->data()._pUserData = 0;

	m_dwFixedItemHeight = dwFixedItemHeight;
	// "file='tree_top.png' corner='2,1,2,1' fade='100'"
	if (0 == szRootBkImage) {
		memset(m_szRootBkImage, 0, sizeof(m_szRootBkImage));
	}
	else {
		STRNCPY(m_szRootBkImage, szRootBkImage, sizeof(m_szRootBkImage));
	}
	
	this->SetItemFont(dwItemFont);
	this->SetItemTextColor(dwItemTextColor); 
	this->SetSelectedItemTextColor(dwSelectedItemTextColor);
	this->SetHotItemTextColor(dwHotItemTextColor);
	m_dwFixedImageMargin = 4;
	m_dwFirstItemMargin = 6;
	m_dwItemMargin = 24;
}

CMyTreeUI::~CMyTreeUI() { if (_root) delete _root; }

bool CMyTreeUI::Add(CControlUI* pControl)
{
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;
	return CListUI::Add(pControl);
}

bool CMyTreeUI::AddAt(CControlUI* pControl, int iIndex)
{
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;
	return CListUI::AddAt(pControl, iIndex);
}

bool CMyTreeUI::Remove(CControlUI* pControl, bool bDoNotDestroy /*= false*/)
{
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;

	if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()) == NULL)
		return CListUI::Remove(pControl, bDoNotDestroy);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()));
}

bool CMyTreeUI::RemoveAt(int iIndex, bool bDoNotDestroy /*= false*/)
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl) return false;
	if (_tcscmp(pControl->GetClass(), DUI_CTR_LISTLABELELEMENT) != 0) return false;

	if (reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()) == NULL)
		return CListUI::RemoveAt(iIndex, bDoNotDestroy);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListLabelElementUI*>(pControl->GetInterface(DUI_CTR_LISTLABELELEMENT))->GetTag()));
}

void CMyTreeUI::RemoveAll()
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
void CMyTreeUI::SetVisible(bool bVisible /*= true*/)
{
	if (m_bVisible == bVisible) return;
	CControlUI::SetVisible(bVisible);
}

void CMyTreeUI::SetInternVisible(bool bVisible /*= true*/)
{
	CControlUI::SetInternVisible(bVisible);
}

void CMyTreeUI::DoEvent(TEventUI& event)
{
	CListUI::DoEvent(event);
}

CMyTreeUI::Node* CMyTreeUI::GetRoot() { return _root; }


// text: "{x 4}{i gameicons.png 18 3}{x 4}ÍÆ¼öÓÎÏ·"
CMyTreeUI::Node* CMyTreeUI::AddNode(LPCTSTR text, Node* parent /*= NULL*/, void * pUserData /*= 0*/, 
	     ImageListData * pImageListData /*= 0*/)
{
	CDuiString  strText;

	if (!parent) parent = _root;
	
	//CListLabelElementUI* pListElement = new CListLabelElementUI;
	CMyListElement* pListElement = new CMyListElement(this);
	Node* node = new Node;
	node->data()._level = parent->data()._level + 1;
	if (node->data()._level == 0) node->data()._expand = true;
	else node->data()._expand = false;
	if (0 != pImageListData) {
		node->data()._text.Format("{x %lu}{i %s %lu %lu}{x %lu}%s", m_dwFixedImageMargin,
			pImageListData->szImageFileName, pImageListData->dwImagesCount, pImageListData->dwImageIndex, 
			m_dwFixedImageMargin, text);
	}
	else {
		node->data()._text.Format("{x %lu}%s", m_dwFixedImageMargin, text);
	}	
	node->data()._pListElement = pListElement;
	node->data()._pUserData = pUserData;
	
	if (parent != _root) {
		if (!(parent->data()._expand && parent->data()._pListElement->IsVisible()))
			pListElement->SetInternVisible(false);
	}

	CDuiString html_text;
	strText.Format("<x %lu>", m_dwFirstItemMargin);
	html_text += strText;
	strText.Format("<x %lu>", m_dwItemMargin);
	for (int i = 0; i < node->data()._level; ++i) {
		html_text += strText;
	}

	int parent_prefix_len = html_text.GetLength() - strText.GetLength();

	if (!parent->has_children()) {
		CListLabelElementUI* parent_element = parent->data()._pListElement;
		if (parent_element) {
			DuiLib::CDuiString html_parent = parent_element->GetText();
			parent_element->SetText( html_parent.Mid(0, parent_prefix_len) + "<v center><a><i tree_expand.png 2 0></a></v>" + html_parent.Mid(parent_prefix_len) );
		}		
	}    

	html_text += node->data()._text;
	pListElement->SetText(html_text);
	pListElement->SetTag((UINT_PTR)node);
	if (node->data()._level == 0) {
		//pListElement->SetBkImage(_T("file='tree_top.png' corner='2,1,2,1' fade='100'"));
		pListElement->SetBkImage(m_szRootBkImage);
	}
	pListElement->SetFixedHeight(m_dwFixedItemHeight);

	int index = 0;
	if (parent->has_children()) {
		Node* prev = parent->get_last_child();
		index = prev->data()._pListElement->GetIndex() + 1;
	}
	else {
		if (parent == _root) index = 0;
		else index = parent->data()._pListElement->GetIndex() + 1;
	}
	if (!CListUI::AddAt(pListElement, index)) {
		delete pListElement;
		delete node;
		node = NULL;
	}
	parent->add_child(node);
	SetFixedHeight(CalculateMinHeight());
	return node;
}

bool CMyTreeUI::RemoveNode(CMyTreeUI::Node* node)
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

void CMyTreeUI::ExpandNode(CMyTreeUI::Node* node, bool expand)
{
	CDuiString  strText;

	if (!node || node == _root) return;

	if (node->data()._expand == expand) return;
	node->data()._expand = expand;

	CDuiString html_text;
	strText.Format("<x %lu>", m_dwFirstItemMargin);
	html_text += strText;
	strText.Format("<x %lu>", m_dwItemMargin);
	for (int i = 0; i < node->data()._level; ++i) {
		html_text += strText;
	}

	if (node->has_children()) {
		if (node->data()._expand) html_text += _T("<v center><a><i tree_expand.png 2 1></a></v>");
		else html_text += _T("<v center><a><i tree_expand.png 2 0></a></v>");
	}

	html_text += node->data()._text;
	node->data()._pListElement->SetText(html_text);

	if (!node->data()._pListElement->IsVisible()) return;
	if (!node->has_children()) return;

	Node* begin = node->child(0);
	Node* end = node->get_last_child();
	for (int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i) {
		CControlUI* control = GetItemAt(i);
		if (_tcscmp(control->GetClass(), DUI_CTR_LISTLABELELEMENT) == 0) {
			Node* local_parent = ((CMyTreeUI::Node*)control->GetTag())->parent();
			control->SetInternVisible(local_parent->data()._expand && local_parent->data()._pListElement->IsVisible());
		}
	}

	SetFixedHeight(CalculateMinHeight());
	NeedUpdate();
}

// button size : 16 * 16
SIZE CMyTreeUI::GetExpanderSizeX(CMyTreeUI::Node* node) const
{
	if (!node || node == _root) return CDuiSize();

	SIZE szExpander = { 0 };
	szExpander.cx = m_dwFirstItemMargin + m_dwItemMargin * node->data()._level;
	szExpander.cy = szExpander.cx + 16;
	return szExpander;
}

SIZE CMyTreeUI::GetExpanderSizeY(Node* node) const {
	if (!node || node == _root) return CDuiSize();

	SIZE szExpander = { 0 };
	szExpander.cx = (m_dwFixedItemHeight - 16) / 2;
	szExpander.cy = szExpander.cx + 16;
	return szExpander;
}


int  CMyTreeUI::CalculateMinHeight() {
	int nCount = this->GetCount();
	int sum = 0;

	for (int i = 0; i < nCount; i++) {
		Node* node = (Node*)this->GetItemAt(i)->GetTag();
		if (node->IsAllParentsExpanded()) {
			int nHeight = m_dwFixedItemHeight; // this->GetItemAt(i)->GetHeight();
			sum += nHeight;
		}
	}

	return sum + 2;
}
