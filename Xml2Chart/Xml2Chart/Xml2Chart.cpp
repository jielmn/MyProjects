#include "Xml2Chart.h"
#include "LmnCommon.h"

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

	m_strName = "";
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
}

RECT  CXml2ChartUI::GetPadding() const {
	return m_tPadding;
}

void  CXml2ChartUI::SetPosition(const RECT &rect) {
	m_tPosition = rect;
}

RECT  CXml2ChartUI::GetPosition() const {
	return m_tPosition;
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

	RECT tmp;
	memcpy(&tmp, &m_tRect, sizeof(RECT));

	tmp.left   += rect.left;
	tmp.right  += rect.left;
	tmp.top    += rect.top;
	tmp.bottom += rect.top;

	return tmp;
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
	if (splitPen.nSplitNum <= 1) {
		return;
	}

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
		if (splitPen.nSplitNum < item.nSplitNum) {
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

int CXml2ChartUI::GetSplitLineCount(BOOL bVertical) const {
	if (bVertical) {
		return m_vVSplitLines.size();
	}
	else {
		return m_vHSplitLines.size();
	}
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

void CXml2ChartUI::SetName(const std::string & sName) {
	m_strName = sName;
}

std::string CXml2ChartUI::GetName() const {
	return m_strName;
}




static void  SaveGdiPen(HGDIOBJ & hOldPen, HGDIOBJ hNewPen, HDC hDc ) {
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

	int nCount = 0;
	int nAve = 0;
	int nMod = 0;

	nCount = pUI->GetSplitLineCount(TRUE);
	for (int i = 0; i < nCount; i++) {
		SplitPen splitPen = pUI->GetSplitLine(TRUE, nCount - i - 1 );
		nAve = pUI->GetWidth() / splitPen.nSplitNum;
		nMod = pUI->GetWidth() % splitPen.nSplitNum;

		SaveGdiPen(hOldPen, splitPen.HPen, hDc);
		for (int j = 0; j < splitPen.nSplitNum - 1; j++) {
			::MoveToEx(hDc, pUI->GetAbsoluteLeft() + (j + 1)*nAve, pUI->GetAbsoluteTop(), 0);			
			::LineTo(hDc, pUI->GetAbsoluteLeft() + (j + 1)*nAve, pUI->GetAbsoluteBottom());
		}
	}

	nCount = pUI->GetSplitLineCount(FALSE);
	for (int i = 0; i < nCount; i++) {
		SplitPen splitPen = pUI->GetSplitLine(FALSE, i);
		nAve = pUI->GetHeight() / splitPen.nSplitNum;
		nMod = pUI->GetHeight() % splitPen.nSplitNum;

		SaveGdiPen(hOldPen, splitPen.HPen, hDc);
		for (int j = 0; j < splitPen.nSplitNum - 1; j++) {
			::MoveToEx(hDc, pUI->GetAbsoluteLeft(), pUI->GetAbsoluteTop() + (j+1)*nAve, 0);
			::LineTo(hDc, pUI->GetAbsoluteRight(), pUI->GetAbsoluteTop() + (j+1)*nAve );
		}
	}

	HGDIOBJ  hOldFont = 0;
	if (pUI->GetFont() != 0) {
		hOldFont = ::SelectObject(hDc, pUI->GetFont());
	}

	
	std::string strText = pUI->GetText();
	int  nLeft = 0;
	int  nTop  = 0;

	RECT rcText;
	::DrawText(hDc, strText.c_str(), strText.length(), &rcText, DT_CALCRECT);
	int  nStrWidth = rcText.right - rcText.left;
	int  nHeight   = rcText.bottom - rcText.top;

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

static bool GetBoolean(const char * value) {
	if (0 == value) {
		return false;
	}

	if (0 == _stricmp(value, "true")) {
		return true;
	}

	return false;
}

static int GetInt(const char * value) {
	if (0 == value)
		return 0;

	int ret = 0;
	if (1 == sscanf_s(value, " %d", &ret)) {
		return ret;
	}

	return 0;
}

static AlignType  GetAlignValue(const char * value) {
	if (0 == value)
		return ALIGN_END;

	if (0 == _stricmp(value, "center")) {
		return ALIGN_CENTER;
	}
	else if (0 == _stricmp(value, "left")) {
		return ALIGN_LEFT;
	}
	else if (0 == _stricmp(value, "right")) {
		return ALIGN_RIGHT;
	}

	return ALIGN_END;
}

static VAlignType  GetVAlignValue(const char * value) {
	if (0 == value)
		return VALIGN_END;

	if (0 == _stricmp(value, "center")) {
		return VALIGN_CENTER;
	}
	else if (0 == _stricmp(value, "top")) {
		return VALIGN_TOP;
	}
	else if (0 == _stricmp(value, "bottom")) {
		return VALIGN_BOTTOM;
	}

	return VALIGN_END;
}

static RECT GetRect(const char * value) {
	RECT rect;
	memset(&rect, 0, sizeof(RECT));

	if (0 == value)
		return rect;

	sscanf_s(value, " %d, %d, %d, %d", &rect.left, &rect.top, &rect.right, &rect.bottom);
	return rect;
}

static DWORD  GetTextColor_(const char * value) {
	DWORD  dwValue = RGB(0,0,0);

	if (0 == value)
		return dwValue;

	int ret = sscanf_s(value, "#%x", &dwValue);
	if ( 0 == ret )
		return dwValue;

	BYTE r = LOBYTE(HIWORD(dwValue));
	BYTE g = HIBYTE(LOWORD(dwValue));
	BYTE b = LOBYTE(LOWORD(dwValue));

	return RGB(r, g, b);
}




CXml2ChartFile::CXml2ChartFile() {
	m_ChartUI = 0;
}

CXml2ChartFile::~CXml2ChartFile() {
	std::map<int, HGDIOBJ>::iterator it;
	for (it = m_mapFonts.begin(); it != m_mapFonts.end(); it++) {
		DeleteObject( it->second );
	}
	m_mapFonts.clear();

	for (it = m_mapPens.begin(); it != m_mapPens.end(); it++) {
		DeleteObject(it->second);
	}
	m_mapPens.clear();

	if (m_ChartUI) {
		delete m_ChartUI;
		m_ChartUI = 0;
	}
}

void CXml2ChartFile::SetBorder(TiXmlElement* pEle, CXml2ChartUI * pUI) {
	const char * szAttr = 0;
	szAttr = pEle->Attribute("border");
	if (szAttr) {
		RECT rect = GetRect(szAttr);
		std::map<int, HGDIOBJ>::iterator it;
		it = m_mapPens.find(rect.left);
		if (it != m_mapPens.end()) {
			pUI->SetBorderPen(it->second, DIRECTION_LEFT);
		}

		it = m_mapPens.find(rect.top);
		if (it != m_mapPens.end()) {
			pUI->SetBorderPen(it->second, DIRECTION_TOP);
		}

		it = m_mapPens.find(rect.right);
		if (it != m_mapPens.end()) {
			pUI->SetBorderPen(it->second, DIRECTION_RIGHT);
		}

		it = m_mapPens.find(rect.bottom);
		if (it != m_mapPens.end()) {
			pUI->SetBorderPen(it->second, DIRECTION_BOTTOM);
		}
	}
}

void  CXml2ChartFile::SetSplit(TiXmlElement* pEle, CXml2ChartUI * pUI) {
	const char * szAttr = 0;
	szAttr = pEle->Attribute("VSplit");
	if ( szAttr )
		ParseSplitPen(pUI, szAttr, TRUE);

	szAttr = pEle->Attribute("HSplit");
	if (szAttr)
		ParseSplitPen(pUI, szAttr, FALSE);

	szAttr = pEle->Attribute("VSplit1");
	if (szAttr)
		ParseSplitPen(pUI, szAttr, TRUE);

	szAttr = pEle->Attribute("HSplit1");
	if (szAttr)
		ParseSplitPen(pUI, szAttr, FALSE);

	szAttr = pEle->Attribute("VSplit2");
	if (szAttr)
		ParseSplitPen(pUI, szAttr, TRUE);

	szAttr = pEle->Attribute("HSplit2");
	if (szAttr)
		ParseSplitPen(pUI, szAttr, FALSE);
}

void CXml2ChartFile::SetFixed(TiXmlElement* pEle, CXml2ChartUI * pUI) {
	const char * szAttr = 0;
	szAttr = pEle->Attribute("width");
	if (szAttr) {
		pUI->SetFixedWidth(GetInt(szAttr));
	}

	szAttr = pEle->Attribute("height");
	if (szAttr) {
		pUI->SetFixedHeight(GetInt(szAttr));
	}
}

void CXml2ChartFile::SetFloat(TiXmlElement* pEle, CXml2ChartUI * pUI) {
	const char * szAttr = 0;
	szAttr = pEle->Attribute("float");
	if (szAttr)
		pUI->SetFloat(GetBoolean(szAttr));

	szAttr = pEle->Attribute("position");
	if (szAttr)
		pUI->SetPosition(GetRect(szAttr));
}

void CXml2ChartFile::SetPadding(TiXmlElement* pEle, CXml2ChartUI * pUI) {
	const char * szAttr = 0;
	szAttr = pEle->Attribute("padding");
	if (szAttr)
		pUI->SetPadding(GetRect(szAttr));

}

void CXml2ChartFile::SetText(TiXmlElement* pEle, CXml2ChartUI * pUI) {
	const char * szAttr = 0;
	char buf[8192];

	szAttr = pEle->Attribute("text");
	if (szAttr)
		pUI->SetText(Utf8ToAnsi(buf, sizeof(buf), szAttr));

	szAttr = pEle->Attribute("align");
	if (szAttr)
		pUI->SetAlignType(GetAlignValue(szAttr));

	szAttr = pEle->Attribute("valign");
	if (szAttr)
		pUI->SetVAlignType(GetVAlignValue(szAttr));

	szAttr = pEle->Attribute("font");
	if (szAttr) {
		int nFont = GetInt(szAttr);
		std::map<int, HGDIOBJ>::iterator it = m_mapFonts.find(nFont);
		if (it != m_mapFonts.end() ) {
			pUI->SetFont(it->second);
		}
	}

	szAttr = pEle->Attribute("textcolor");
	if (szAttr)
		pUI->SetTextColor(GetTextColor_(szAttr));
		
}

// VSplit="num='3',pen='0'" HSplit="num='3',pen='1'"
void CXml2ChartFile::ParseSplitPen(CXml2ChartUI * pUI, const char * szAttr, BOOL bVertical) {
	assert(pUI);

	if (0 == szAttr) {
		return;
	}
	
	const char * pComma = strchr(szAttr,',');
	if (0 == pComma) {
		return;
	}

	int nNum = 0;
	int nPenIndex = -1;

	const char * pNum = strstr(szAttr, "num");
	if (0 == pNum || pNum >= pComma) {
		return;
	}

	const char * pEqual = strchr(pNum + 3, '=');
	if (0 == pEqual || pEqual >= pComma) {
		return;
	}

	const char * pQuote = strchr(pEqual + 1, '\'');
	if (0 == pQuote|| pQuote >= pComma) {
		return;
	}

	if (1 != sscanf_s(pQuote+1, " %d", &nNum)) {
		return;
	}

	const char * pPen = strstr(pComma + 1, "pen");
	if (0 == pPen) {
		return;
	}

	pEqual = strchr(pPen + 3, '=');
	if (0 == pEqual) {
		return;
	}

	pQuote = strchr(pEqual + 1, '\'');
	if (0 == pQuote) {
		return;
	}

	if (1 != sscanf_s(pQuote + 1, " %d", &nPenIndex)) {
		return;
	}

	std::map<int, HGDIOBJ>::iterator it;
	it = m_mapPens.find(nPenIndex);
	if ( it == m_mapPens.end()) {
		return;
	}

	SplitPen s;
	s.nSplitNum = nNum;
	s.HPen = it->second;

	pUI->SetSplitLine(bVertical, s);
}

void CXml2ChartFile::ReadXmlChartFile(TiXmlElement* pEleParent, CXml2ChartUI * pParentUI) {
	assert(pEleParent && pParentUI);

	CXml2ChartUI * pChildUI = 0;
	const char * szAttr = 0;

	TiXmlElement * pChild = pEleParent->FirstChildElement();
	while ( pChild ) {
		const char * szElmName = pChild->Value();
		if ( 0 == strcmp( "Item", szElmName ) ) {
			pChildUI = new CXml2ChartUI;
			pParentUI->AddChild(pChildUI);

			SetText(pChild, pChildUI);
			SetFloat(pChild, pChildUI);
			SetFixed(pChild, pChildUI);
			SetBorder(pChild, pChildUI);
			
			szAttr = pChild->Attribute("name");
			if (szAttr) {
				pChildUI->SetName(szAttr);
			}
		}
		else {
			BOOL  bVertical = FALSE;
			if (0 == strcmp("VerticalLayout", szElmName)) {
				bVertical = TRUE;
			}
			else if (0 == strcmp("HorizontalLayout", szElmName)) {
				bVertical = FALSE;
			}
			else {
				pChild = pChild->NextSiblingElement();
				continue;;
			}

			pChildUI = new CXml2ChartUI;
			pParentUI->AddChild(pChildUI);
			if ( bVertical )
				pChildUI->SetChildrenLayout(LAYOUT_VERTICAL);
			else
				pChildUI->SetChildrenLayout(LAYOUT_HORIZONTAL);


			SetFloat(pChild, pChildUI);
			SetFixed(pChild, pChildUI);
			SetSplit(pChild, pChildUI);
			SetBorder(pChild, pChildUI);
			SetPadding(pChild, pChildUI);

			szAttr = pChild->Attribute("name");
			if (szAttr) {
				pChildUI->SetName(szAttr);
			}

			ReadXmlChartFile(pChild, pChildUI);

		}

		pChild = pChild->NextSiblingElement();
	}
}

CXml2ChartUI * CXml2ChartFile::ReadXmlChartStr(const char * szXml, DWORD  dwLength /*= 0*/) {
	TiXmlDocument xmlDoc;

	// 以'\0'结束
	if (0 == dwLength) {
		xmlDoc.Parse(szXml);
	}
	else {
		char * pXml = new char[dwLength + 1];
		memcpy(pXml, szXml, dwLength);
		pXml[dwLength] = '\0';

		xmlDoc.Parse(pXml);
		delete[] pXml;
	}

	return ParseXml(xmlDoc);
}


CXml2ChartUI * CXml2ChartFile::ReadXmlChartFile(const char * szFilePath) {
	if (0 == szFilePath) {
		return 0;
	}

	TiXmlDocument xmlDoc(szFilePath); // 输入XML路径  
	if (!xmlDoc.LoadFile())           // 读取XML并检查是否读入正确  
		return 0;

	return ParseXml(xmlDoc);
}

CXml2ChartUI *  CXml2ChartFile::ParseXml(TiXmlDocument & xmlDoc) {
	int  nWinWidth = 0;
	int  nWinHeight = 0;
	TiXmlElement* pElmRoot = NULL; // 根节点  
	pElmRoot = xmlDoc.FirstChildElement("Window"); // 得到根节点  
	if (!pElmRoot)
	{
		return 0;
	}

	nWinWidth = GetInt(pElmRoot->Attribute("width"));
	nWinHeight = GetInt(pElmRoot->Attribute("height"));

	BOOL  bFindLayout = FALSE;
	TiXmlElement* pElmChild = pElmRoot->FirstChildElement();
	while (pElmChild) {
		const char * szElmName = pElmChild->Value();
		if (0 == szElmName) {
			pElmChild = pElmChild->NextSiblingElement();
			continue;
		}

		// 如果是font节点
		if (0 == strcmp(szElmName, "Font")) {
			const char * sFontId = pElmChild->Attribute("id");
			if (0 == sFontId) {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			int nFontId = 0;
			if (1 != sscanf_s(sFontId, " %d", &nFontId)) {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			if (m_mapFonts.find(nFontId) != m_mapFonts.end()) {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			const char * sAttr = 0;
			LOGFONT  logfont;
			BOOL bValue = false;

			GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);

			sAttr = pElmChild->Attribute("bold");
			bValue = GetBoolean(sAttr);
			if (bValue) {
				logfont.lfWeight = FW_BOLD;
			}
			else {
				logfont.lfWeight = FW_NORMAL;
			}


			sAttr = pElmChild->Attribute("italic");
			logfont.lfItalic = GetBoolean(sAttr);

			sAttr = pElmChild->Attribute("charset");
			if (sAttr && (0 == _stricmp(sAttr, "gb2312") || 0 == _stricmp(sAttr, "gbk"))) {
				logfont.lfCharSet = GB2312_CHARSET;
			}
			else {
				logfont.lfCharSet = ANSI_CHARSET;
			}

			sAttr = pElmChild->Attribute("facename");
			if (sAttr) {
				sprintf_s(logfont.lfFaceName, sAttr);
			}

			//sAttr = pElmChild->Attribute("width");
			//if (sAttr)
			//	logfont.lfWeight = GetInt(sAttr);

			sAttr = pElmChild->Attribute("height");
			if (sAttr)
				logfont.lfHeight = GetInt(sAttr);

			HFONT hfont = CreateFontIndirect(&logfont);
			m_mapFonts.insert(std::make_pair(nFontId, hfont));
		}
		// 如果是Thread节点
		else if (0 == strcmp(szElmName, "Thread")) {
			const char * sThreadId = pElmChild->Attribute("id");
			if (0 == sThreadId) {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			int nThreadId = 0;
			if (1 != sscanf_s(sThreadId, " %d", &nThreadId)) {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			if (m_mapPens.find(nThreadId) != m_mapPens.end()) {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			const char * sAttr = 0;
			sAttr = pElmChild->Attribute("width");
			int nPenWidth = GetInt(sAttr);
			if (nPenWidth <= 0) {
				nPenWidth = 1;
			}

			COLORREF  color = RGB(0, 0, 0);
			sAttr = pElmChild->Attribute("color");
			if (sAttr) {
				sscanf_s(sAttr, "#%x", &color);
			}
			BYTE b[sizeof(color)];
			memcpy(b, &color, sizeof(color));
			DWORD rgb = RGB(b[2], b[1], b[0]);
			HGDIOBJ hPen = ::CreatePen(PS_SOLID, nPenWidth, rgb);
			m_mapPens.insert(std::make_pair(nThreadId, hPen));
		}
		// 如果是布局节点
		else {
			BOOL  bVertical = FALSE;
			if (0 == strcmp(szElmName, "VerticalLayout")) {
				if (bFindLayout) {
					pElmChild = pElmChild->NextSiblingElement();
					continue;
				}
				bFindLayout = TRUE;
				bVertical = TRUE;
			}
			else if (0 == strcmp(szElmName, "HorizontalLayout")) {
				if (bFindLayout) {
					pElmChild = pElmChild->NextSiblingElement();
					continue;
				}
				bFindLayout = TRUE;
				bVertical = FALSE;
			}
			else {
				pElmChild = pElmChild->NextSiblingElement();
				continue;
			}

			if (m_ChartUI)
				delete m_ChartUI;

			m_ChartUI = new CXml2ChartUI;

			const char * sAttr = 0;
			int nWidth = 0;
			int nHeight = 0;

			sAttr = pElmChild->Attribute("width");
			if (sAttr) {
				nWidth = GetInt(sAttr);
				if (nWidth > nWinWidth) {
					nWidth = nWinWidth;
				}
			}
			else {
				nWidth = nWinWidth;
			}

			sAttr = pElmChild->Attribute("height");
			if (sAttr) {
				nHeight = GetInt(sAttr);
			}
			else {
				nHeight = nWinHeight;
				if (nHeight > nWinHeight) {
					nHeight = nWinHeight;
				}
			}

			sAttr = pElmChild->Attribute("name");
			if (sAttr) {
				m_ChartUI->SetName(sAttr);
			}

			m_ChartUI->SetRect(0, 0, nWidth, nHeight);

			if (bVertical)
				m_ChartUI->SetChildrenLayout(LAYOUT_VERTICAL);
			else
				m_ChartUI->SetChildrenLayout(LAYOUT_HORIZONTAL);

			SetSplit(pElmChild, m_ChartUI);
			SetBorder(pElmChild, m_ChartUI);
			SetPadding(pElmChild, m_ChartUI);

			ReadXmlChartFile(pElmChild, m_ChartUI);
		}
		pElmChild = pElmChild->NextSiblingElement();
	}

	if (m_ChartUI) {
		m_ChartUI->RecacluteLayout();
	}

	return m_ChartUI;
}

CXml2ChartUI *  CXml2ChartFile::FindChartUIByName(CXml2ChartUI * pUI, const char * szName) {
	assert(pUI);
	assert(szName);

	std::string sName = pUI->GetName();
	if (0 == strcmp(sName.c_str(), szName)) {
		return pUI;
	}

	int nChildrenCount = pUI->GetChildrenCount();
	for (int i = 0; i < nChildrenCount; i++) {
		CXml2ChartUI * pChild = pUI->GetChild(i);
		CXml2ChartUI * pFind = FindChartUIByName(pChild, szName);
		if (0 != pFind) {
			return pFind;
		}
	}

	return 0;
}

CXml2ChartUI * CXml2ChartFile::FindChartUIByName(const char * szName) {
	if (0 == m_ChartUI) {
		return 0;
	}

	if (0 == szName) {
		return 0;
	}

	return FindChartUIByName(m_ChartUI, szName);	
}