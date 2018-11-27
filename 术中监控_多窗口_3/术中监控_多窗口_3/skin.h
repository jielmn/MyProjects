#pragma once

#include "LmnCommon.h"

class CMySkin {
public:
	enum ENUM_SKIN {
		SKIN_BLACK = 0,
		SKIN_WHITE
	};

	enum ENUM_UI {
		LAYOUT_MAIN_BK = 0,
		LAYOUT_MAIN_BK_1,
		LABEL_STATUS_BK,
		GRID_BORDER,
		COMMON_TEXT,
		MYIMAGE_BK,
		EDIT_BK,
		EDIT_TEXT,
		LAYOUT_READER_BK,
		HIGH_TEMP_ALARM_TEXT_COLOR,
		LOW_TEMP_ALARM_TEXT_COLOR,
		NORMAL_TEMP_TEXT_COLOR,
	};

	enum ENUM_IMG_NAME {
		OPT_SELECTED = 0,
		OPT_NOT_SELECTED,
	};

	enum ENUM_GDI {
		COMMON_PEN = 0,
		COMMON_TEXT_COLOR,
		BRIGHT_PEN,
		COMMON_BRUSH,
		LOW_ALARM_PEN,
		HIGH_ALARM_PEN,
	};

	CMySkin();
	~CMySkin();

	void   SetSkin(ENUM_SKIN eSkin);
	ENUM_SKIN  GetSkin() const;
	DWORD  operator [] (ENUM_UI eUI) const;
	const char * GetImageName(ENUM_IMG_NAME e) const;
	DWORD  GetReaderIndicator( DWORD dwReaderIndex ) const;
	COLORREF   GetRgb(ENUM_GDI e) const;

private:
	ENUM_SKIN     m_skin;
};
