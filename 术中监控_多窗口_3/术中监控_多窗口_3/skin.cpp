#include "common.h"
#include "skin.h"


CMySkin::CMySkin() {
	m_skin = SKIN_BLACK;
}

CMySkin::~CMySkin() {

}

void CMySkin::SetSkin(ENUM_SKIN eSkin) {
	m_skin = eSkin;
}

CMySkin::ENUM_SKIN  CMySkin::GetSkin() const {
	return m_skin;
}

DWORD  CMySkin::operator [] (ENUM_UI eUI) const {
	if (m_skin == SKIN_BLACK) {
		switch (eUI)
		{
		case LAYOUT_MAIN_BK:
			//return 0xFF192431;
			return 0xFF434248;
		case LAYOUT_MAIN_BK_1:
			return 0xFF293441;
		case LABEL_STATUS_BK:
			return 0xFF3A4F67;
		case GRID_BORDER:
			//return 0xFFFFFFFF;
			return 0xFF4F4F4F;
		case COMMON_TEXT:
			return 0xFFFFFFFF;
		case MYIMAGE_BK:
			//return 0xFF192431;
			return 0xFF434248;
		case EDIT_BK:
			return 0xFFFFFFFF;
		case EDIT_TEXT:
			return 0xFF000000;
		case LAYOUT_READER_BK:
			return 0xFF444444;
		case HIGH_TEMP_ALARM_TEXT_COLOR:
			return 0xFFDF455F;
		case LOW_TEMP_ALARM_TEXT_COLOR:
			return 0xFF01AFC3;
		case NORMAL_TEMP_TEXT_COLOR:
			return 0xFF4E8B20;
		default:
			break;
		}
	}
	else if (m_skin == SKIN_WHITE) {
		switch (eUI)
		{
		case LAYOUT_MAIN_BK:
			return 0xFFFFFFFF;
		case LAYOUT_MAIN_BK_1:
			return 0xFFDDDDDD;
		case LABEL_STATUS_BK:
			return 0xFF007ACC;
		case GRID_BORDER:
			return 0xFF768D9B;
		case COMMON_TEXT:
			return 0xFF555555;
		case MYIMAGE_BK:
			return 0xFFDDDDDD;
		case EDIT_BK:
			return 0xFFAAAAAA;
		case EDIT_TEXT:
			return 0xFFFFFFFF;
		case LAYOUT_READER_BK:
			return 0xFFDDDDDD;
		case HIGH_TEMP_ALARM_TEXT_COLOR:
			return 0xFFDF455F;
		case LOW_TEMP_ALARM_TEXT_COLOR:
			return 0xFF01AFC3;
		case NORMAL_TEMP_TEXT_COLOR:
			return 0xFF4E8B20;
		default:
			break;
		}
	}
	return 0;
}

const char * CMySkin::GetImageName(CMySkin::ENUM_IMG_NAME e) const {
	if (m_skin == SKIN_BLACK) {
		switch (e)
		{
		case OPT_SELECTED:
			return "checked_0.png";
		case OPT_NOT_SELECTED:
			return "not_checked_0.png";
		default:
			break;
		}
	}
	else if (m_skin == SKIN_WHITE) {
		switch (e)
		{
		case OPT_SELECTED:
			return "checked_1.png";
		case OPT_NOT_SELECTED:
			return "not_checked_1.png";
		default:
			break;
		}
	}

	return "";
}


DWORD  CMySkin::GetReaderIndicator(DWORD dwReaderIndex) const {
	if (m_skin == SKIN_BLACK) {
		switch (dwReaderIndex)
		{
		case 0:
			return 0xFF00FF00;
		case 1:
			return 0xFF1b9375;
		case 2:
			return 0xFF00FFFF;
		case 3:
			return 0xFF51786C;
		case 4:
			return 0xFFFFFF00;
		case 5:
			return 0xFFCA5100;
		case 6:
			return 0xFFFF00FF;
		case 7:
			return 0xFFA5A852;
		case 8:
			return 0xFFCCCCCC;
		default:
			break;
		}
	}
	else if (m_skin == SKIN_WHITE) {
		switch (dwReaderIndex)
		{
		case 0:
			return 0xFF00FF00;
		case 1:
			return 0xFF1b9375;
		case 2:
			return 0xFF00FFFF;
		case 3:
			return 0xFF51786C;
		case 4:
			return 0xFFFFFF00;
		case 5:
			return 0xFFCA5100;
		case 6:
			return 0xFFFF00FF;
		case 7:
			return 0xFFA5A852;
		case 8:
			return 0xFFCCCCCC;
		default:
			break;
		}
	}
	return 0;
}

COLORREF   CMySkin::GetRgb(CMySkin::ENUM_GDI e) const {
	if (m_skin == SKIN_BLACK) {
		switch (e)
		{
		case COMMON_PEN:
			return RGB(0x66, 0x66, 0x66);
		case COMMON_TEXT_COLOR:
			return RGB(255, 255, 255);
		case BRIGHT_PEN:
			return RGB(0x99, 0x99, 0x99);
		case COMMON_BRUSH:
			//return RGB(0x19, 0x24, 0x31);
			return RGB(0x43, 0x42, 0x48);
		case LOW_ALARM_PEN:
			return RGB(0x02, 0xA5, 0xF1);
		case HIGH_ALARM_PEN:
			return RGB(0xFC, 0x23, 0x5C);		
		default:
			break;
		}
	}
	else {
		switch (e)
		{
		case COMMON_PEN:
			return RGB(0x99, 0x99, 0x99);
		case COMMON_TEXT_COLOR:
			return RGB(0x55, 0x55, 0x55);
		case BRIGHT_PEN:
			return RGB(0x33, 0x33, 0x33);
		case COMMON_BRUSH:
			return RGB(0xDD, 0xDD, 0xDD);
		case LOW_ALARM_PEN:
			return RGB(0x02, 0xA5, 0xF1);
		case HIGH_ALARM_PEN:
			return RGB(0xFC, 0x23, 0x5C);
		default:
			break;
		}
	}
	
	return RGB(255, 255, 255);
}