#include "common.h"
#include "skin.h"


CMySkin::CMySkin() : m_temperature_pen(Gdiplus::Color(0, 0, 0), 1.0), m_temperature_brush(Gdiplus::Color(0, 255, 0)) {
	m_dwSkinIndex      = -1;
	m_hCommonThreadPen = 0;

#if 1
	SetSkin(SKIN_BLACK);
#else
	SetSkin(SKIN_WHITE);
#endif
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

	if (m_dwSkinIndex == SKIN_BLACK) {
		m_hCommonThreadPen = ::CreatePen(0, 1, RGB(0x66, 0x66, 0x66));
	}
	else if (m_dwSkinIndex == SKIN_WHITE) {
		m_hCommonThreadPen = ::CreatePen(0, 1, RGB(0x66, 0x66, 0x66));
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
		default:
			break;
		}
	}
	return 0;
}
 
HPEN   CMySkin::GetPen(DWORD dwPenIndex) const {
	switch (dwPenIndex)
	{
	case COMMON_THREAD_INDEX:
		return m_hCommonThreadPen;
	default:
		break;
	}
	return 0;
}

const Pen &  CMySkin::GetPenEx(DWORD dwPenIndex) const {
	switch (dwPenIndex)
	{
	case TEMPERATURE_THREAD_INDEX:
		return m_temperature_pen;

	default:
		break;
	}

	assert(0);
	return m_temperature_pen;
}

const SolidBrush &  CMySkin::GetBrushEx(DWORD dwBrushIndex) const {
	switch (dwBrushIndex)
	{
	case TEMPERATURE_BRUSH_INDEX:
		return m_temperature_brush;

	default:
		break;
	}

	assert(0);
	return m_temperature_brush;
}