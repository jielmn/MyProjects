#pragma once

#include "LmnCommon.h"

class CMySkin {
public:
	CMySkin();

	void   SetSkin(DWORD dwSkinIndex);
	DWORD  GetSkin() const;
	DWORD  operator [] (DWORD dwPartIndex) const;
	HGDIOBJ  GetGdiObject(DWORD dwIndex) const;

private:
	DWORD        m_dwSkinIndex;
	HPEN         m_hCommonThreadPen;
	HBRUSH       m_hCommonBrush;
	HPEN         m_hBrighterThreadPen;
	HPEN         m_hLowTempAlarmPen;
	HPEN         m_hHighTempAlarmPen;
};
