#pragma once

#ifndef _XML_TO_CHART_
#define _XML_TO_CHART_

#include <Windows.h>
#include <vector>
#include <map>

#include "tinyxml.h"  

#ifdef GetNextSibling
#undef GetNextSibling
#endif

/*
某某医院
体温单
姓名：xx     住院号：xx   住院日期：xx-xx-xx  床号：aa
|-------------|-------------|--------------------|------------------------------|
|-------------|-------------|--------------------|------------------------------|
|-------------|-------------|--------------------|------------------------------|
*/


// 一个矩形，内部可能有等分线
typedef struct  tagSplitPen {
	int       nSplitNum;        // 划分几块
	HGDIOBJ   HPen;             // 线条
}SplitPen;


enum LayoutType{
	LAYOUT_HORIZONTAL  = 0,
	LAYOUT_VERTICAL,
	LAYOUT_END
};

enum DirectionType {
	DIRECTION_LEFT     = 0,
	DIRECTION_RIGHT,
	DIRECTION_TOP,
	DIRECTION_BOTTOM,
	DIRECTION_END,
};

enum  AlignType {
	ALIGN_CENTER     = 0,
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_END,
};

enum  VAlignType {
	VALIGN_CENTER = 0,
	VALIGN_TOP,
	VALIGN_BOTTOM,
	VALIGN_END,
};



/*

<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<Window width="400" height="300" >
<Font   id="0" />
<Font   id="1" />
<Thread id="0" />
<Thread id="1" />

<VerticalLayout>
<Item text="某某医院" align="center" valign="center" />
<Item text="体温单" align="center" valign="center" />
<HorizontalLayout>
<Item text="姓名" align="left" /><Item text="入院日期" align="left" /><Item text="科室" align="left" /><Item text="床号" align="left" /><Item text="住院号" align="left" />
</HorizontalLayout>
</VerticalLayout>

</Window>

*/

class CXml2ChartFile;

// 使用的时候，用new出来的堆内存，以便递归释放子UI
class  CXml2ChartUI {
public:
	CXml2ChartUI(CXml2ChartFile * pManager = 0);
	~CXml2ChartUI();

protected:
	RECT       m_tRect;          // (左，右，上，下位置，相对于父UI)，计算出来的UI的相对父UI位置(每次父窗口size改变，则重新计算该大小)

	RECT       m_tPadding;       // 填充(左，右，上，下），父UI size改变，该值不变
	BOOL       m_bFloat;         // floating(如果是floating状态，则不考虑父窗口的padding和本窗口的margin)，父UI size改变，该值不变
	RECT       m_tPosition;      // 和float一起确定位置，父UI size改变，该值不变
	int        m_nFixedWidth;    // 固定宽度(初始值为-1，表示自动计算，例如根据子窗口计算出的20%，100%等父窗口宽度)
	int        m_nFixedHeight;   // 固定高度(初始值为-1)
	BOOL       m_bVisible;       // 是否可见

	HGDIOBJ    m_BorderPen[4];   // 边线Pen(包括线条粗细，颜色)

	std::vector<SplitPen>    m_vHSplitLines;   // 矩形的水平内部划分线，从大到小排序
	std::vector<SplitPen>    m_vVSplitLines;   // 矩形的垂直内部划分线，从大到小排序

	CXml2ChartUI *                m_parent;
	LayoutType                    m_eChildrenLayout;       // 子项布局
	std::vector<CXml2ChartUI *>   m_children;              // 子UI
	CXml2ChartUI *                m_PreSibling;
	CXml2ChartUI *                m_NextSibling;

	std::string                   m_strText;
	HGDIOBJ                       m_hFontText;
	COLORREF                      m_TextColor;
	AlignType                     m_eAlign;                // 水平排列
	VAlignType                    m_eVAlign;               // 垂直排列

	std::string                   m_strName;               // 名字
	CXml2ChartFile *              m_pManager;

public:
	int   GetWidth() const;
	int   GetHeight() const;

	void  SetVisible(BOOL bVisible);
	BOOL  GetVisible() const;

	void  SetFloat(BOOL bFloat);
	BOOL  GetFloat() const;

	void  SetFixedWidth(int nWidth);
	int   GetFixedWidth() const;

	void  SetFixedHeight(int nHeight);
	int   GetFixedHeight() const;

	int   GetLeft() const;
	int   GetRight() const;
	int   GetTop() const;
	int   GetBottom() const;

	int   GetAbsoluteLeft() const;
	int   GetAbsoluteRight() const;
	int   GetAbsoluteTop() const;
	int   GetAbsoluteBottom() const;

	void  SetPadding(const RECT &rect);
	RECT  GetPadding() const;

	void  SetPosition(const RECT &rect);
	RECT  GetPosition() const;

	void  SetRect(const RECT & rect);
	void  SetRect(int nLeft, int nTop, int nRight, int nBottom);
	RECT  GetRect( ) const;
	RECT  GetAbsoluteRect() const;

	void  SetBorderPen(HGDIOBJ HPen, DirectionType eBorderType);
	HGDIOBJ  GetBorderPen( DirectionType eBorderType ) const;

	void  SetParent(CXml2ChartUI * parent);
	void  SetPreSibling(CXml2ChartUI * sibling);
	void  SetNextSibling(CXml2ChartUI * sibling);

	CXml2ChartUI *  GetParent( ) const;
	CXml2ChartUI *  GetPreSibling( ) const;
	CXml2ChartUI *  GetNextSibling( ) const;

	void SetChildrenLayout(LayoutType eLayoutType);
	LayoutType  GetChildrenLayout() const;

	void SetSplitLine(BOOL bVertical, const SplitPen & splitPen);             // 把矩形划分为小格子的线条
	SplitPen GetSplitLine(BOOL bVertical, int nIndex) const;
	int GetSplitLineCount(BOOL bVertical) const;

	void AddChild(CXml2ChartUI * child);
	CXml2ChartUI * GetChild(int nIndex) const;
	int  GetChildrenCount() const;

	void  SetText(const std::string & strText);
	std::string  GetText() const;

	void  SetAlignType(AlignType eType);
	AlignType  GetAlignType() const;

	void  SetVAlignType(VAlignType eType);
	VAlignType  GetVAlignType() const;

	void SetFont(HGDIOBJ  hFont);
	HGDIOBJ  GetFont() const;

	void SetFontIndex(int nIndex);
	int  GetFontIndex() const;

	void SetTextColor(COLORREF color);
	COLORREF  GetTextColor() const;

	void SetName( const std::string & sName);
	std::string GetName( ) const;


	void  RecacluteLayout();                             //重新计算布局和子UI布局
};

class  CXml2ChartFile {
public:
	CXml2ChartFile();
	~CXml2ChartFile();
	CXml2ChartUI * ReadXmlChartFile(const char * szFilePath);
	CXml2ChartUI * ReadXmlChartStr(const char * szXml, DWORD  dwLength = 0);
	CXml2ChartUI *             m_ChartUI;

	CXml2ChartUI * FindChartUIByName(const char * szName);

public:
	std::map<int,HGDIOBJ>      m_mapFonts;
protected:
	std::map<int,HGDIOBJ>      m_mapPens;	

	void ReadXmlChartFile(TiXmlElement* pElement, CXml2ChartUI * pParentUI );
	void ParseSplitPen(CXml2ChartUI * pUI, const char * szAttr, BOOL bVertical);

	void SetBorder(TiXmlElement* pEle, CXml2ChartUI * pUI);
	void SetSplit(TiXmlElement* pEle, CXml2ChartUI * pUI);
	void SetFixed(TiXmlElement* pEle, CXml2ChartUI * pUI);
	void SetFloat(TiXmlElement* pEle, CXml2ChartUI * pUI);
	void SetPadding(TiXmlElement* pEle, CXml2ChartUI * pUI);
	void SetText(TiXmlElement* pEle, CXml2ChartUI * pUI);
	void SetVisible(TiXmlElement* pEle, CXml2ChartUI * pUI);

	CXml2ChartUI *  FindChartUIByName(CXml2ChartUI * pUI, const char * szName);

	CXml2ChartUI *  ParseXml(TiXmlDocument & xmlDoc);
};


void  DrawXml2ChartUI(HDC hDc, CXml2ChartUI * pUI, int nOffsetX = 0, int nOffsetY = 0);


#endif