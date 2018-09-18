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
			return 0xFF192431;
		case LABEL_STATUS_BK:
			return 0xFF3A4F67;
		case GRID_BORDER:
			return 0xFFFFFFFF;
		case COMMON_TEXT:
			return 0xFFFFFFFF;
		case MYIMAGE_BK:
			return 0xFF192431;
		default:
			break;
		}
	}
	else if (m_skin == SKIN_WHITE) {
		switch (eUI)
		{
		case LAYOUT_MAIN_BK:
			return 0xFFFFFFFF;
		case LABEL_STATUS_BK:
			return 0xFF007ACC;
		case GRID_BORDER:
			return 0xFF768D9B;
		case COMMON_TEXT:
			return 0xFF555555;
		case MYIMAGE_BK:
			return 0xFFDDDDDD;
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