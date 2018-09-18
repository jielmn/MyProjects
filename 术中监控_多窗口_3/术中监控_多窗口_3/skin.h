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
		LABEL_STATUS_BK,
		GRID_BORDER,
		COMMON_TEXT,
		MYIMAGE_BK,
	};

	enum ENUM_IMG_NAME {
		OPT_SELECTED = 0,
		OPT_NOT_SELECTED,
	};

	CMySkin();
	~CMySkin();

	void   SetSkin(ENUM_SKIN eSkin);
	ENUM_SKIN  GetSkin() const;
	DWORD  operator [] (ENUM_UI eUI) const;
	const char * GetImageName(ENUM_IMG_NAME e) const;

private:
	ENUM_SKIN     m_skin;
};
