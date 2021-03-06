#pragma once

#include "Windows.h"
#include "sigslot.h"

#include "UIlib.h"
using namespace DuiLib;



class CMyTreeCfgUI : public CListUI, public sigslot::has_slots<>
{

public:
	struct NodeData
	{
		int  _level;
		bool _expand;
		CListContainerElementUI* _pListElement;
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

	enum ConfigType{
		ConfigType_NONE = 0,
		ConfigType_EDIT,
		ConfigType_COMBO,
		ConfigType_CHECKBOX,
		ConfigType_FileBrowse,
		ConfigType_DateTime,
	};

	class ConfigValue {
	public:
		ConfigValue();

		CDuiString  m_strEdit;
		int         m_nComboSel;
		BOOL        m_bCheckbox;
		ConfigType  m_eConfigType;
		time_t      m_time;
		UINT_PTR    m_tag;
	};

	CMyTreeCfgUI(DWORD  dwFixedLeft = 160);

	~CMyTreeCfgUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl, bool bDoNotDestroy = false);

	bool RemoveAt(int iIndex, bool bDoNotDestroy = false);

	void RemoveAll();
	void SetVisible(bool bVisible = true);

	void SetInternVisible(bool bVisible = true);

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode( LPCTSTR pTitleText, Node* parent = NULL, void * pUserData = 0, CControlUI * pConfig = NULL,
		           DWORD dwTitleFontIndex = -1, DWORD dwTitleColor = 0xFF000000, DWORD  dwCfgFontIndex = -1,
		           DWORD dwCfgColor=0xFF000000, DWORD dwItemHeight=0 );

	bool RemoveNode(Node* node);

	void ExpandNode(Node* node, bool expand);

	SIZE GetExpanderSizeX(Node* node) const;

	int GetItemIndex(CControlUI* pControl) const;

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	bool GetConfigValue(int nIndex, ConfigValue & cfgValue);
	bool SetConfigValue(int nIndex, const ConfigValue & cfgValue);

	void OnExpandClick(CControlUI* pSender);
	void SetTitle(int nIndex, LPCTSTR pTitle);

private:
	Node* _root;

	LONG m_dwDelayDeltaY;
	DWORD m_dwDelayNum;
	DWORD m_dwDelayLeft;

	DWORD  m_dwFixedLeft;
	DWORD  m_dwFixedItemHeight;
	HPEN   m_hPen;

private:
	int   CalculateMinHeight();
};