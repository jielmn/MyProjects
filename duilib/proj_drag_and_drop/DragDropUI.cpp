
#include "common.h"
#include "DragDropUI.h"
#include "resource.h"

CDragDropUI::CDragDropUI()
{
	m_uButtonState = 0;
	memset( &m_ptLastMouse, 0, sizeof(m_ptLastMouse));
	memset(&m_rcNewPos, 0, sizeof(m_rcNewPos));

	m_forbit = CRenderEngine::LoadImage( IDB_PNG1,"png",0x00000000);
	m_bCheckPos = TRUE; 
}


CDragDropUI::~CDragDropUI()
{
	CRenderEngine::FreeImage(m_forbit);
}

LPVOID CDragDropUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, _T("DragDropUI")) == 0)
	{
		return static_cast<CDragDropUI*>(this);
	}
	else
	{
		return CControlUI::GetInterface(pstrName);
	}
}

void CDragDropUI::DoEvent(TEventUI& event)
{
	if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled())
	{
		if (::PtInRect(&m_rcItem, event.ptMouse))
		{
			m_uButtonState |= UISTATE_CAPTURED;
			m_ptLastMouse  = event.ptMouse;
			m_rcNewPos     = m_rcItem;   

			if (m_pManager)
				m_pManager->AddPostPaint(this);
			return;
		}
	}
	else if (event.Type == UIEVENT_BUTTONUP)
	{
		if ((m_uButtonState & UISTATE_CAPTURED) != 0)
		{
			m_uButtonState &= ~UISTATE_CAPTURED;

			// 在窗体的notify消息处理处处理
			m_pManager->SendNotify(this, "CheckPos", TRUE);
			CDuiRect rcParent = m_pParent->GetPos();

			// 新位置有效（没有超出父控件的范围）
			if (m_bCheckPos) {
				RECT rcNewPos    = m_rcNewPos;
				rcNewPos.left   -= rcParent.left;
				rcNewPos.right  -= rcParent.left;
				rcNewPos.top    -= rcParent.top;
				rcNewPos.bottom -= rcParent.top;
				this->SetPos(rcNewPos);
			}


			if (m_pManager)
			{
				m_pManager->RemovePostPaint(this);
				m_pManager->Invalidate();
			}
			NeedParentUpdate();
			return;
		}
	}
	else if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if ((m_uButtonState & UISTATE_CAPTURED) != 0)
		{
			LONG cx = event.ptMouse.x - m_ptLastMouse.x;
			LONG cy = event.ptMouse.y - m_ptLastMouse.y;

			m_ptLastMouse = event.ptMouse;
			RECT rcCurPos = m_rcNewPos;

			rcCurPos.left += cx;
			rcCurPos.right += cx;
			rcCurPos.top += cy;
			rcCurPos.bottom += cy;

			//将当前拖拽块的位置 和 当前拖拽块的前一时刻的位置，刷新  
			CDuiRect rcInvalidate = m_rcNewPos;
			m_rcNewPos = rcCurPos;

			// CheckPos();
			// 在窗体的notify消息处理处处理
			m_pManager->SendNotify(this, "CheckPos");

			rcInvalidate.Join(m_rcNewPos);
			if (m_pManager) m_pManager->Invalidate(rcInvalidate);

			return;
		}
	}
	CControlUI::DoEvent(event);
}

void CDragDropUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
	if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
		int width = GetWidth();
		int height = GetHeight();

		RECT rcUpdate = m_rcNewPos;
		//CRenderEngine::DrawColor(hDC, rcUpdate, 0xAA000000);		

		HDC  hComDC = CreateCompatibleDC(hDC);
		HBITMAP hBmp = CreateCompatibleBitmap(hDC, width, height);
		SelectObject(hComDC, hBmp);
		::BitBlt(hComDC, 0, 0, width, height, hDC, m_rcItem.left, m_rcItem.top, SRCCOPY);

		HDC  hForbit = 0;
		// 显示禁止放下图标
		if (!m_bCheckPos) {
			hForbit = CreateCompatibleDC(hDC);
			SelectObject(hForbit, m_forbit->hBitmap);
			::BitBlt(hComDC, width-16, height-16, 16, 16, hForbit, 0, 0, SRCCOPY);
		}			

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0x80;
		bf.AlphaFormat = 0;
		AlphaBlend( hDC, rcUpdate.left, rcUpdate.top, width, height, hComDC, 0, 0, width, height, bf);
		// ::BitBlt(hDC, rcUpdate.left, rcUpdate.top, width, height, hComDC, 0, 0, SRCCOPY);

		DeleteObject(hBmp);
		DeleteDC(hComDC);
		if (!m_bCheckPos) {
			DeleteDC(hForbit);
		}
	}
}
