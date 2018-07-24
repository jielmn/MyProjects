#pragma once

#include "LmnCommon.h"

class CMySkin {
public:
	CMySkin();

	void   SetSkin(DWORD dwSkinIndex);
	DWORD  GetSkin() const;
	DWORD  operator [] (DWORD dwPartIndex) const;
	HPEN   GetPen(DWORD dwPenIndex) const;
	const Pen &  GetPenEx(DWORD dwPenIndex) const;
	const SolidBrush &  GetBrushEx(DWORD dwBrushIndex) const;

private:
	DWORD        m_dwSkinIndex;
	HPEN         m_hCommonThreadPen;
	Pen          m_temperature_pen;
	SolidBrush   m_temperature_brush;
};
