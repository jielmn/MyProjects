#pragma once

#include "Windows.h"
#include "UIlib.h"
using namespace DuiLib;


class CMyTreeUI : public CListUI
{

public:
	struct NodeData
	{
		int  _level;
		bool _expand;
		CDuiString _text;
		CListLabelElementUI* _pListElement;
		void * _pUserData;
	};

	class Node
	{
		typedef std::vector <Node*>	Children;
		Children	_children;
		Node*		_parent;
		NodeData    _data;

	private:
		void set_parent(Node* parent);

	public:
		Node();
		~Node();

		NodeData& data();
		int num_children() const;
		Node* child(int i);
		Node* parent();
		bool has_children() const;
		void add_child(Node* child);
		void remove_child(Node* child);
		void remove_all_child();
		Node* get_last_child();
		bool IsAllParentsExpanded();
	};

	struct ImageListData {
		char    szImageFileName[128];
		DWORD   dwImagesCount;
		DWORD   dwImageIndex;
	};

	CMyTreeUI( DWORD dwFixedItemHeight=26, const char * szRootBkImage=0, 
		DWORD dwItemFont = -1, DWORD dwItemTextColor = 0xFF000000, 
		DWORD dwSelectedItemTextColor = 0xFF000000, DWORD dwHotItemTextColor = 0xFF000000 );

	~CMyTreeUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl, bool bDoNotDestroy = false);

	bool RemoveAt(int iIndex, bool bDoNotDestroy = false);

	void RemoveAll();
	void SetVisible(bool bVisible = true);

	void SetInternVisible(bool bVisible = true);

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(LPCTSTR text, Node* parent = NULL, void * pUserData = 0, ImageListData * pImageListData = 0);

	bool RemoveNode(Node* node);

	void ExpandNode(Node* node, bool expand);

	SIZE GetExpanderSizeX(Node* node) const;
	SIZE GetExpanderSizeY(Node* node) const;
private:
	int   CalculateMinHeight();

private:
	Node* _root;

	LONG m_dwDelayDeltaY;
	DWORD m_dwDelayNum;
	DWORD m_dwDelayLeft;

	DWORD  m_dwFixedItemHeight;
	char   m_szRootBkImage[128];
	DWORD  m_dwFixedImageMargin;
	DWORD  m_dwFirstItemMargin;
	DWORD  m_dwItemMargin;
};