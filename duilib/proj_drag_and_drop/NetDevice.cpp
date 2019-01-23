
#include "common.h"
#include "NetDevice.h"


CNetDevice::CNetDevice()
{
}


CNetDevice::~CNetDevice()
{
}

LPVOID CNetDevice::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, _T("NetDeviceInfo")) == 0)
	{
		return static_cast<CNetDevice*>(this);
	}
	else
	{
		return CContainerUI::GetInterface(pstrName);
	}
}

void CNetDevice::DoEvent(TEventUI& event)
{
	if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled())
	{
		if (::PtInRect(&m_rcItem, event.ptMouse))
		{
			m_uButtonState |= UISTATE_CAPTURED;
			m_ptLastMouse = event.ptMouse;
			m_rcNewPos = m_rcItem;

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

			this->SetPos(m_rcNewPos);                   //这句是拖拽到目的地的关机，否则无法拖动到目的位置

			if (m_pManager)
			{
				m_pManager->RemovePostPaint(this);
				m_pManager->Invalidate(m_rcNewPos);
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
			rcInvalidate.Join(m_rcNewPos);
			if (m_pManager) m_pManager->Invalidate(rcInvalidate);

			return;
		}
	}
	CContainerUI::DoEvent(event);
}

void CNetDevice::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
	if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
		CDuiRect rcParent = m_pParent->GetPos();
		RECT rcUpdate = { 0 };
		rcUpdate.left = m_rcNewPos.left < rcParent.left ? rcParent.left : m_rcNewPos.left;
		rcUpdate.top = m_rcNewPos.top < rcParent.top ? rcParent.top : m_rcNewPos.top;
		rcUpdate.right = m_rcNewPos.right > rcParent.right ? rcParent.right : m_rcNewPos.right;
		rcUpdate.bottom = m_rcNewPos.bottom > rcParent.bottom ? rcParent.bottom : m_rcNewPos.bottom;
		CRenderEngine::DrawColor(hDC, rcUpdate, 0xAA000000);
	}
}