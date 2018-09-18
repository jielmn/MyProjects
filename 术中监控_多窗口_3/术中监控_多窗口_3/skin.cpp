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