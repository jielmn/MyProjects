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
ĳĳҽԺ
���µ�
������xx     סԺ�ţ�xx   סԺ���ڣ�xx-xx-xx  ���ţ�aa
|-------------|-------------|--------------------|------------------------------|
|-------------|-------------|--------------------|------------------------------|
|-------------|-------------|--------------------|------------------------------|
*/


// һ�����Σ��ڲ������еȷ���
typedef struct  tagSplitPen {
	int       nSplitNum;        // ���ּ���
	HGDIOBJ   HPen;             // ����
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
<Item text="ĳĳҽԺ" align="center" valign="center" />
<Item text="���µ�" align="center" valign="center" />
<HorizontalLayout>
<Item text="����" align="left" /><Item text="��Ժ����" align="left" /><Item text="����" align="left" /><Item text="����" align="left" /><Item text="סԺ��" align="left" />
</HorizontalLayout>
</VerticalLayout>

</Window>

*/

class CXml2ChartFile;

// ʹ�õ�ʱ����new�����Ķ��ڴ棬�Ա�ݹ��ͷ���UI
class  CXml2ChartUI {
public:
	CXml2ChartUI(CXml2ChartFile * pManager = 0);
	~CXml2ChartUI();

protected:
	RECT       m_tRect;          // (���ң��ϣ���λ�ã�����ڸ�UI)�����������UI����Ը�UIλ��(ÿ�θ�����size�ı䣬�����¼���ô�С)

	RECT       m_tPadding;       // ���(���ң��ϣ��£�����UI size�ı䣬��ֵ����
	BOOL       m_bFloat;         // floating(�����floating״̬���򲻿��Ǹ����ڵ�padding�ͱ����ڵ�margin)����UI size�ı䣬��ֵ����
	RECT       m_tPosition;      // ��floatһ��ȷ��λ�ã���UI size�ı䣬��ֵ����
	int        m_nFixedWidth;    // �̶����(��ʼֵΪ-1����ʾ�Զ����㣬��������Ӵ��ڼ������20%��100%�ȸ����ڿ��)
	int        m_nFixedHeight;   // �̶��߶�(��ʼֵΪ-1)
	BOOL       m_bVisible;       // �Ƿ�ɼ�

	HGDIOBJ    m_BorderPen[4];   // ����Pen(����������ϸ����ɫ)

	std::vector<SplitPen>    m_vHSplitLines;   // ���ε�ˮƽ�ڲ������ߣ��Ӵ�С����
	std::vector<SplitPen>    m_vVSplitLines;   // ���εĴ�ֱ�ڲ������ߣ��Ӵ�С����

	CXml2ChartUI *                m_parent;
	LayoutType                    m_eChildrenLayout;       // �����
	std::vector<CXml2ChartUI *>   m_children;              // ��UI
	CXml2ChartUI *                m_PreSibling;
	CXml2ChartUI *                m_NextSibling;

	std::string                   m_strText;
	HGDIOBJ                       m_hFontText;
	COLORREF                      m_TextColor;
	AlignType                     m_eAlign;                // ˮƽ����
	VAlignType                    m_eVAlign;               // ��ֱ����

	std::string                   m_strName;               // ����
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

	void SetSplitLine(BOOL bVertical, const SplitPen & splitPen);             // �Ѿ��λ���ΪС���ӵ�����
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


	void  RecacluteLayout();                             //���¼��㲼�ֺ���UI����
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