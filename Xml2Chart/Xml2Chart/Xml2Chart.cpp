#include "Xml2Chart.h"

#define MIN(a, b) ((a)<=(b)?(a):(b))

CXml2ChartUI::CXml2ChartUI() {
	m_tRect.left = m_tRect.right = m_tRect.top = m_tRect.bottom = 0;
	for (int i = 0; i < 4; i++) {
		m_BorderPen[i] = 0;
	}
	m_parent = m_NextSibling = m_PreSibling = 0;

	m_eChildrenLayout = LAYOUT_VERTICAL;
	m_hFontText = 0;
	m_eAlign = ALIGN_CENTER;
	m_eVAlign = VALIGN_CENTER;

	memset(&m_tPadding, 0, sizeof(m_tPadding));
	m_bFloat = FALSE;

	m_nFixedWidth = -1;
	m_nFixedHeight = -1;
	m_TextColor = RGB(0, 0, 0);
}

CXml2ChartUI::~CXml2ChartUI() {
	std::vector<CXml2ChartUI *>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++) {
		CXml2ChartUI * child = *it;
		delete child;
	}
	m_children.clear();
}

int CXml2ChartUI::GetWidth() const {
	return m_tRect.right - m_tRect.left;
}

int CXml2ChartUI::GetHeight() const {
	return m_tRect.bottom - m_tRect.top;
}

void  CXml2ChartUI::SetFixedWidth(int nWidth) {
	m_nFixedWidth = nWidth;
}

int   CXml2ChartUI::GetFixedWidth() const {
	return m_nFixedWidth;
}

void  CXml2ChartUI::SetFixedHeight(int nHeight) {
	m_nFixedHeight = nHeight;
}

int   CXml2ChartUI::GetFixedHeight() const {
	return m_nFixedHeight;
}

void  CXml2ChartUI::SetFloat(BOOL bFloat) {
	m_bFloat = bFloat;
}

BOOL  CXml2ChartUI::GetFloat() const {
	return m_bFloat;
}

void  CXml2ChartUI::SetPadding(const RECT &rect) {
	m_tPadding  = rect;

	int nWidth  = this->GetWidth();
	int nHeight = this->GetHeight();

	std::vector<CXml2ChartUI *>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++) {
		CXml2ChartUI * child = *it;

		if (!child->GetFont()) {
			
		}
	}
}

RECT  CXml2ChartUI::GetPadding() const {
	return m_tPadding;
}

int   CXml2ChartUI::GetLeft() const {
	return m_tRect.left;
}

int   CXml2ChartUI::GetRight() const {
	return m_tRect.right;
}

int   CXml2ChartUI::GetTop() const {
	return m_tRect.top;
}

int   CXml2ChartUI::GetBottom() const {
	return m_tRect.bottom;
}

int  CXml2ChartUI::GetAbsoluteLeft() const {
	if (m_parent) {
		return m_tRect.left + m_parent->GetAbsoluteLeft();
	}
	else {
		return m_tRect.left;
	}
}

int   CXml2ChartUI::GetAbsoluteRight() const {
	if (m_parent) {
		return m_tRect.right + m_parent->GetAbsoluteLeft();
	}
	else {
		return m_tRect.right;
	}
}

int   CXml2ChartUI::GetAbsoluteTop() const {
	if (m_parent) {
		return m_tRect.top + m_parent->GetAbsoluteTop();
	}
	else {
		return m_tRect.top;
	}
}

int   CXml2ChartUI::GetAbsoluteBottom() const {
	if (m_parent) {
		return m_tRect.bottom + m_parent->GetAbsoluteTop();
	}
	else {
		return m_tRect.bottom;
	}
}



void  CXml2ChartUI::SetRect(const RECT & rect) {
	m_tRect = rect;
}

void  CXml2ChartUI::SetRect(int nLeft, int nTop, int nRight, int nBottom) {
	m_tRect.left = nLeft;
	m_tRect.top = nTop;
	m_tRect.right = nRight;
	m_tRect.bottom = nBottom;
}

RECT  CXml2ChartUI::GetRect() const {
	return m_tRect;
}

RECT  CXml2ChartUI::GetAbsoluteRect() const {
	RECT  rect;
	memset(&rect, 0, sizeof(RECT));

	if (m_parent) {
		rect = m_parent->GetAbsoluteRect();
	}

	rect.left   += m_tRect.left;
	rect.top    += m_tRect.top;
	rect.right  += m_tRect.right;
	rect.bottom += m_tRect.bottom;

	return rect;
}

void  CXml2ChartUI::SetBorderPen(HGDIOBJ HPen, DirectionType eBorderType) {
	if (eBorderType >= DIRECTION_END) {
		return;
	}

	m_BorderPen[eBorderType] = HPen;
}

HGDIOBJ  CXml2ChartUI::GetBorderPen(DirectionType eBorderType) const {
	if (eBorderType >= DIRECTION_END) {
		return 0;
	}

	return m_BorderPen[eBorderType];
}

void  CXml2ChartUI::SetParent(CXml2ChartUI * parent) {
	m_parent = parent;
}

void  CXml2ChartUI::SetPreSibling(CXml2ChartUI * sibling) {
	m_PreSibling = sibling;
}

void  CXml2ChartUI::SetNextSibling(CXml2ChartUI * sibling) {
	m_NextSibling = sibling;
}

CXml2ChartUI *  CXml2ChartUI::GetParent() const {
	return m_parent;
}

CXml2ChartUI *  CXml2ChartUI::GetPreSibling() const {
	return m_PreSibling;
}

CXml2ChartUI *  CXml2ChartUI::GetNextSibling() const {
	return m_NextSibling;
}

void CXml2ChartUI::SetChildrenLayout(LayoutType eLayoutType) {
	if (eLayoutType >= LAYOUT_END) {
		return;
	}

	m_eChildrenLayout = eLayoutType;
}

LayoutType  CXml2ChartUI::GetChildrenLayout() const {
	return m_eChildrenLayout;
}

void  CXml2ChartUI::SetSplitLine(BOOL bVertical, const SplitPen & splitPen) {
	std::vector<SplitPen> * pVec = 0;
	if (bVertical) {
		pVec = &m_vVSplitLines;
	}
	else {
		pVec = &m_vHSplitLines;
	}

	std::vector<SplitPen>::iterator it;
	for (it = pVec->begin(); it != pVec->end(); it++) {
		SplitPen & item = *it;
		if (splitPen.nSplitNum > item.nSplitNum) {
			break;
		}
		else if (splitPen.nSplitNum == item.nSplitNum) {
			item.HPen = splitPen.HPen;
			return;
		}
	}

	pVec->insert(it, splitPen);
}

SplitPen CXml2ChartUI::GetSplitLine(BOOL bVertical, int nIndex) const {
	const std::vector<SplitPen> * pVec = 0;
	if (bVertical) {
		pVec = &m_vVSplitLines;
	}
	else {
		pVec = &m_vHSplitLines;
	}

	if (nIndex < (int)pVec->size()) {
		return (*pVec)[nIndex];
	}

	SplitPen t;
	memset(&t, 0, sizeof(t));
	return t;
}

void  CXml2ChartUI::RecacluteLayout() {
	std::vector<CXml2ChartUI *>::iterator it;
	int  nWidth  = this->GetWidth();
	int  nHeight = this->GetHeight();
	int  nAccu         = 0;
	int  nAutoChildCnt = 0;
	int  nAve   = 0;                   // 自动计算的平均值
	int  nMod   = 0;                   // 自动计算的余数
	int  nIndex = 0;                   // 用于判断是否最后一个
	int  nLeft  = 0;
	RECT  tmpPadding;
	int  nTop = 0;
	int  iLeft = 0;

	// 首先让padding的范围不能超出自身的width和height
	memcpy( &tmpPadding, &m_tPadding, sizeof(RECT) );
	if (tmpPadding.left > nWidth) {
		tmpPadding.left = nWidth;
		tmpPadding.right = 0;
	}
	else if (tmpPadding.left + tmpPadding.right > nWidth) {
		tmpPadding.right = nWidth - tmpPadding.left;
	}

	if (tmpPadding.top > nHeight ) {
		tmpPadding.top = nHeight;
		tmpPadding.bottom = 0;
	}
	else if (tmpPadding.top + tmpPadding.bottom > nHeight) {
		tmpPadding.bottom = nHeight - tmpPadding.top;
	}

	if ( m_eChildrenLayout == LAYOUT_VERTICAL ) {
		// 计算垂直方向，自动子UI的高度
		for ( it = m_children.begin(); it != m_children.end(); it++ ) {
			CXml2ChartUI * child = *it;
			if (!child->m_bFloat) {
				// 固定大小的
				if (child->m_nFixedHeight != -1) {
					nAccu += child->m_nFixedHeight;
				}
				// 自动计算大小的
				else {
					nAutoChildCnt++;
				}				
			}
		}

		// 如果自动计算大小的存在
		if (nAutoChildCnt > 0) {
			nLeft = nHeight - nAccu - tmpPadding.top - tmpPadding.bottom;
			if (nLeft > 0) {
				nAve = (nLeft) / nAutoChildCnt;
				nMod = (nLeft) % nAutoChildCnt;
			}
		}
		
		nLeft = nHeight - tmpPadding.top - tmpPadding.bottom;
		nTop = tmpPadding.top;

		for ( it = m_children.begin(), nIndex = 0; it != m_children.end(); it++, nIndex++ ) {
			CXml2ChartUI * child = *it;
			// 如果是浮动的
			if ( child->m_bFloat ) {
				if (child->m_tPosition.left > nWidth) {
					child->m_tRect.left = nWidth;
				}

				if ( child->m_tPosition.right > nWidth ) {
					child->m_tRect.right = nWidth;
				}

				if (child->m_tPosition.top > nHeight) {
					child->m_tRect.top = nHeight;
				}

				if ( child->m_tPosition.bottom > nHeight ) {
					child->m_tRect.bottom = nHeight;
				}
				child->RecacluteLayout();
			}
			// 如果是固定的高度
			else if (child->m_nFixedHeight != -1) {
				// 剩余的高度不足以支持固定高度
				if (child->m_nFixedHeight > nLeft) {
					// 如果还是固定宽度
					if (child->m_nFixedWidth != -1) {
						child->SetRect(tmpPadding.left, nTop, MIN(child->m_nFixedWidth, nWidth - tmpPadding.right), nTop + nLeft );
					}
					else {
						child->SetRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + nLeft );
					}
					nTop += nLeft;
					nLeft = 0;
				}
				else {
					// 如果还是固定宽度
					if (child->m_nFixedWidth != -1) {
						child->SetRect(tmpPadding.left, nTop, MIN(child->m_nFixedWidth, nWidth - tmpPadding.right), nTop + child->m_nFixedHeight);
					}
					else {
						child->SetRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + child->m_nFixedHeight);
					}
					nTop += child->m_nFixedHeight;
					nLeft -= child->m_nFixedHeight;
				}
				child->RecacluteLayout();
			}
			// 如果是自动计算
			else {
				// 如果是固定宽度
				if (child->m_nFixedWidth != -1) {
					// 如果是最后一个
					if (nIndex == nAutoChildCnt - 1) {
						child->SetRect(tmpPadding.left, nTop, MIN(child->m_nFixedWidth, nWidth - tmpPadding.right), nTop + nAve + nMod );
						nTop += nAve + nMod;
						nLeft -= nAve + nMod;
					}
					else {
						child->SetRect(tmpPadding.left, nTop, MIN(child->m_nFixedWidth, nWidth - tmpPadding.right), nTop + nAve);
						nTop += nAve;
						nLeft -= nAve;
					}		
				}
				else {
					if (nIndex == nAutoChildCnt - 1) {
						child->SetRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + nAve + nMod);
						nTop += nAve + nMod;
						nLeft -= nAve + nMod;
					}
					else {
						child->SetRect(tmpPadding.left, nTop, nWidth - tmpPadding.right, nTop + nAve);
						nTop += nAve;
						nLeft -= nAve;
					}
				}
				child->RecacluteLayout();
			}
		}
	}
	else if (m_eChildrenLayout == LAYOUT_HORIZONTAL) {
		// 计算垂直方向，自动子UI的高度
		for (it = m_children.begin(); it != m_children.end(); it++) {
			CXml2ChartUI * child = *it;
			if (!child->m_bFloat) {
				// 固定大小的
				if (child->m_nFixedWidth != -1) {
					nAccu += child->m_nFixedWidth;
				}
				// 自动计算大小的
				else {
					nAutoChildCnt++;
				}
			}
		}

		// 如果自动计算大小的存在
		if (nAutoChildCnt > 0) {
			nLeft = nWidth - nAccu - tmpPadding.left - tmpPadding.right;
			if (nLeft > 0) {
				nAve = (nLeft) / nAutoChildCnt;
				nMod = (nLeft) % nAutoChildCnt;
			}
		}

		nLeft = nWidth - tmpPadding.left - tmpPadding.right;
		iLeft = tmpPadding.left;

		for (it = m_children.begin(), nIndex = 0; it != m_children.end(); it++, nIndex++) {
			CXml2ChartUI * child = *it;
			// 如果是浮动的
			if (child->m_bFloat) {
				if (child->m_tPosition.left > nWidth) {
					child->m_tRect.left = nWidth;
				}

				if (child->m_tPosition.right > nWidth) {
					child->m_tRect.right = nWidth;
				}

				if (child->m_tPosition.top > nHeight) {
					child->m_tRect.top = nHeight;
				}

				if (child->m_tPosition.bottom > nHeight) {
					child->m_tRect.bottom = nHeight;
				}
				child->RecacluteLayout();
			}
			// 如果是固定的宽度
			else if (child->m_nFixedWidth != -1) {
				// 剩余的宽度不足以支持固定宽度
				if (child->m_nFixedWidth > nLeft) {
					// 如果还是固定高度
					if (child->m_nFixedHeight != -1) {
						child->SetRect(iLeft, tmpPadding.top, iLeft + nLeft, MIN(nHeight - tmpPadding.bottom, child->m_nFixedHeight));
					}
					else {
						child->SetRect(iLeft, tmpPadding.top, iLeft + nLeft, nHeight - tmpPadding.bottom );
					}
					iLeft += nLeft;
					nLeft = 0;
				}
				else {
					// 如果还是固定高度
					if (child->m_nFixedHeight != -1) {
						child->SetRect( iLeft, tmpPadding.top, iLeft + child->m_nFixedWidth, MIN(nHeight - tmpPadding.bottom, child->m_nFixedHeight));
					}
					else {
						child->SetRect(iLeft, tmpPadding.top, iLeft + child->m_nFixedWidth, nHeight - tmpPadding.bottom );
					}
					iLeft += child->m_nFixedWidth;
					nLeft -= child->m_nFixedWidth;
				}
				child->RecacluteLayout();
			}
			// 如果是自动计算
			else {
				// 如果是固定宽度
				if (child->m_nFixedHeight != -1) {
					// 如果是最后一个
					if (nIndex == nAutoChildCnt - 1) {
						child->SetRect(iLeft, tmpPadding.top, iLeft + nAve + nMod, MIN(nHeight - tmpPadding.bottom, child->m_nFixedHeight));
						iLeft += nAve + nMod;
						nLeft -= nAve + nMod;
					}
					else {
						child->SetRect(iLeft, tmpPadding.top, iLeft + nAve, MIN(nHeight - tmpPadding.bottom, child->m_nFixedHeight));
						iLeft += nAve;
						nLeft -= nAve;
					}
				}
				else {
					if (nIndex == nAutoChildCnt - 1) {
						child->SetRect(iLeft, tmpPadding.top, iLeft + nAve + nMod, nHeight - tmpPadding.bottom );
						iLeft += nAve + nMod;
						nLeft -= nAve + nMod;
					}
					else {
						child->SetRect(iLeft, tmpPadding.top, iLeft + nAve, nHeight - tmpPadding.bottom);
						iLeft += nAve;
						nLeft -= nAve;
					}
				}
				child->RecacluteLayout();
			}
		}
	}
}

void CXml2ChartUI::AddChild(CXml2ChartUI * child) {
	if (0 == child) {
		return;
	}

	child->m_parent = this;
	int nCount = (int)m_children.size();
	CXml2ChartUI * preSibling = 0;
	if (nCount > 0) {
		preSibling = m_children[nCount - 1];
	}
	child->m_PreSibling = preSibling;
	child->m_NextSibling = 0;

	if (preSibling) {
		preSibling->m_NextSibling = child;
	}

	m_children.push_back(child);

	RecacluteLayout();	
}

CXml2ChartUI * CXml2ChartUI::GetChild(int nIndex) const {
	if (nIndex >= (int)m_children.size()) {
		return 0;
	}

	return m_children[nIndex];
}

int  CXml2ChartUI::GetChildrenCount() const {
	return m_children.size();
}

void  CXml2ChartUI::SetText(const std::string & strText) {
	m_strText = strText;
}

std::string  CXml2ChartUI::GetText() const {
	return m_strText;
}

void  CXml2ChartUI::SetAlignType(AlignType eType) {
	if (eType >= ALIGN_END) {
		return;
	}

	m_eAlign = eType;
}

AlignType  CXml2ChartUI::GetAlignType() const {
	return m_eAlign;
}

void  CXml2ChartUI::SetVAlignType(VAlignType eType) {
	if (eType >= VALIGN_END) {
		return;
	}

	m_eVAlign = eType;
}

VAlignType  CXml2ChartUI::GetVAlignType() const {
	return m_eVAlign;
}

void CXml2ChartUI::SetFont(HGDIOBJ  hFont) {
	m_hFontText = hFont;
}

HGDIOBJ  CXml2ChartUI::GetFont() const {
	return m_hFontText;
}

void CXml2ChartUI::SetTextColor(COLORREF color) {
	m_TextColor = color;
}

COLORREF  CXml2ChartUI::GetTextColor() const {
	return m_TextColor;
}





void  SaveGdiPen(HGDIOBJ & hOldPen, HGDIOBJ hNewPen, HDC hDc ) {
	if (0 == hOldPen) {
		hOldPen = ::SelectObject(hDc, hNewPen);
	}
	else {
		::SelectObject(hDc, hNewPen);
	}
}

void  DrawXml2ChartUI(HDC hDc, CXml2ChartUI * pUI) {
	if (pUI == 0) {
		return;
	}

	if (0 == hDc) {
		return;
	}

	HGDIOBJ hPenLeft   = pUI->GetBorderPen(DIRECTION_LEFT);
	HGDIOBJ hPenRight  = pUI->GetBorderPen(DIRECTION_RIGHT);
	HGDIOBJ hPenTop    = pUI->GetBorderPen(DIRECTION_TOP);
	HGDIOBJ hPenBottom = pUI->GetBorderPen(DIRECTION_BOTTOM);

	HGDIOBJ  hOldPen = 0;

	if ( 0 != hPenLeft) {
		SaveGdiPen(hOldPen, hPenLeft, hDc);
		::MoveToEx(hDc, pUI->GetAbsoluteLeft(), pUI->GetAbsoluteTop(), 0);
		::LineTo(hDc, pUI->GetAbsoluteLeft(), pUI->GetAbsoluteBottom());
	}

	if (0 != hPenRight) {
		SaveGdiPen(hOldPen, hPenRight, hDc);
		::MoveToEx(hDc, pUI->GetAbsoluteRight(), pUI->GetAbsoluteTop(), 0);
		::LineTo(hDc, pUI->GetAbsoluteRight(), pUI->GetAbsoluteBottom());
	}

	if (0 != hPenTop ) {
		SaveGdiPen(hOldPen, hPenTop, hDc);
		::MoveToEx(hDc, pUI->GetAbsoluteLeft(), pUI->GetAbsoluteTop(), 0);
		::LineTo(hDc, pUI->GetAbsoluteRight(), pUI->GetAbsoluteTop());
	}

	if (0 != hPenBottom) {
		SaveGdiPen(hOldPen, hPenBottom, hDc);
		::MoveToEx(hDc, pUI->GetAbsoluteLeft(), pUI->GetAbsoluteBottom(), 0);
		::LineTo(hDc, pUI->GetAbsoluteRight(), pUI->GetAbsoluteBottom());
	}

	TEXTMETRIC tm;
	GetTextMetrics(hDc, &tm);
	
	std::string strText = pUI->GetText();
	int  nStrWidth = tm.tmAveCharWidth * strText.length();
	int  nHeight   = tm.tmExternalLeading + tm.tmHeight;
	int  nLeft = 0;
	int  nTop  = 0;

	if (pUI->GetAlignType() == ALIGN_CENTER) {
		nLeft = (pUI->GetWidth() - nStrWidth) / 2;
	}
	else if (pUI->GetAlignType() == ALIGN_RIGHT) {
		nLeft = pUI->GetWidth() - nStrWidth;
	}

	if (pUI->GetVAlignType() == VALIGN_CENTER) {
		nTop = (pUI->GetHeight() - nHeight) / 2;
	}
	else if (pUI->GetVAlignType() == VALIGN_BOTTOM) {
		nTop = pUI->GetHeight() - nHeight;
	}

	HGDIOBJ  hOldFont = 0;
	if (pUI->GetFont() != 0) {
		hOldFont = ::SelectObject(hDc, pUI->GetFont());
	}

	COLORREF hOldColor = ::GetTextColor(hDc);
	::SetTextColor(hDc, pUI->GetTextColor());

	::TextOut(hDc, nLeft + pUI->GetAbsoluteLeft(), nTop + pUI->GetAbsoluteTop(), strText.c_str(), strText.length());

	if (0 != hOldPen) {
		::SelectObject(hDc, hOldPen);
	}
	
	if (0 != hOldFont) {
		::SelectObject(hDc, hOldFont);
	}

	::SetTextColor(hDc, hOldColor);
	

	int nChildrenCount = pUI->GetChildrenCount();
	for (int i = 0; i < nChildrenCount; i++) {
		CXml2ChartUI * pChild = pUI->GetChild(i);
		DrawXml2ChartUI(hDc, pChild);
	}
}