#include "common.h"
#include "skin.h"


CMySkin::CMySkin() {
	m_dwSkinIndex        = -1;
	m_hCommonThreadPen   = 0;
	m_hCommonBrush       = 0;
	m_hBrighterThreadPen = 0;
	m_hLowTempAlarmPen   = 0;
	m_hHighTempAlarmPen  = 0;

	//SetSkin(g_dwSkinIndex);
}

void CMySkin::SetSkin(DWORD dwSkinIndex) {
	assert(dwSkinIndex <= SKIN_WHITE);
	if ( m_dwSkinIndex == dwSkinIndex ) {
		return;
	}

	m_dwSkinIndex = dwSkinIndex;
	if (m_hCommonThreadPen != 0) {
		DeleteObject(m_hCommonThreadPen);
		m_hCommonThreadPen = 0;
	}

	if (m_hCommonBrush != 0) {
		DeleteObject(m_hCommonBrush);
		m_hCommonBrush = 0;
	}

	if (m_hBrighterThreadPen != 0) {
		DeleteObject(m_hBrighterThreadPen);
		m_hBrighterThreadPen = 0;
	}

	if (m_hLowTempAlarmPen != 0) {
		DeleteObject(m_hLowTempAlarmPen);
		m_hLowTempAlarmPen = 0;
	}

	if (m_hHighTempAlarmPen != 0) {
		DeleteObject(m_hHighTempAlarmPen);
		m_hHighTempAlarmPen = 0;
	}

	if (m_dwSkinIndex == SKIN_BLACK) {
		m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
		m_hCommonBrush     = ::CreateSolidBrush(RGB(0x19, 0x24, 0x31));
		m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
		m_hLowTempAlarmPen   = ::CreatePen(PS_DASH, 1, RGB(0x02, 0xA5, 0xF1));
		m_hHighTempAlarmPen  = ::CreatePen(PS_DASH, 1, RGB(0xFC, 0x23, 0x5C));
	}
	else if (m_dwSkinIndex == SKIN_WHITE) {
		m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
		m_hCommonBrush     = ::CreateSolidBrush(RGB(0xDD, 0xDD, 0xDD));
		m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x33, 0x33, 0x33));
		m_hLowTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0x02, 0xA5, 0xF1));
		m_hHighTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0xFC, 0x23, 0x5C));
	}
}

DWORD  CMySkin::GetSkin() const {
	return m_dwSkinIndex;
}

DWORD CMySkin::operator [] (DWORD dwPartIndex) const {
	if ( m_dwSkinIndex == SKIN_BLACK ) {
		switch (dwPartIndex)
		{
		case LAYOUT_MAIN_BK_COLOR_INDEX:
			return 0xFF192431;
		case LABEL_STATUS_BK_COLOR_INDEX:
			return 0xFF3A4F67;
		case GRID_BORDER_COLOR_INDEX:
			return 0xFFFFFFFF;
		case COMMON_TEXT_COLOR_INDEX:
			return 0xFFFFFFFF;
		case MYIMAGE_BK_COLOR_INDEX:
			return 0xFF192431;
		case MYIMAGE_TEMP_THREAD_COLOR_INDEX:
			return 0xFF00FF00;
		case MYIMAGE_TEMP_DOT_COLOR_INDEX:
			return 0xFF00FF00;
		default:
			break;
		}
	}
	else if ( m_dwSkinIndex == SKIN_WHITE ) {
		switch (dwPartIndex)
		{
		case LAYOUT_MAIN_BK_COLOR_INDEX:
			return 0xFFFFFFFF;
		case LABEL_STATUS_BK_COLOR_INDEX:
			return 0xFF007ACC;  
		case GRID_BORDER_COLOR_INDEX:
			return 0xFF768D9B;
		case COMMON_TEXT_COLOR_INDEX:
			return 0xFF555555;
		case MYIMAGE_BK_COLOR_INDEX:
			return 0xFFDDDDDD;
		case MYIMAGE_TEMP_THREAD_COLOR_INDEX:
			return 0xFFCA5100;
		case MYIMAGE_TEMP_DOT_COLOR_INDEX:
			return 0xFFCA5100;
		default:
			break;
		}
	}
	return 0;
}
 
HGDIOBJ  CMySkin::GetGdiObject(DWORD dwIndex) const {
	switch (dwIndex)
	{
	case COMMON_PEN_INDEX:
		return m_hCommonThreadPen;
	case COMMON_BRUSH_INDEX:
		return m_hCommonBrush;
	case BRIGHTER_PEN_INDEX:
		return m_hBrighterThreadPen;
	case LOW_TEMP_PEN_INDEX:
		return m_hLowTempAlarmPen;
	case HIGH_TEMP_PEN_INDEX:
		return m_hHighTempAlarmPen;
	default:
		break;
	}
	return 0;
}
