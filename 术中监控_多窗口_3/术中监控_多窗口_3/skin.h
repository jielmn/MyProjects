#pragma once

#include "LmnCommon.h"

class CMySkin {
public:
	enum ENUM_SKIN {
		SKIN_BLACK = 0,
		SKIN_WHITE
	};
	CMySkin();
	~CMySkin();

	void   SetSkin(ENUM_SKIN eSkin);
	ENUM_SKIN  GetSkin() const;

private:
	ENUM_SKIN     m_skin;
};
