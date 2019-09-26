#pragma once

#include "common.h"

class CGridUI : public CContainerUI, INotifyUI
{
public:
	CGridUI();
	~CGridUI();

	void  SetIndex(int nIndex);
	void  SetBeat(BYTE beat, BYTE v);
	void  SetOxy(BYTE oxy, BYTE v);
	void  SetTemp(int nTemp);
	void  SetPose(BYTE pose);
	void  SetNo(BYTE no);
	BYTE  GetNo() const;
	void  SetFreq(BYTE freq);
	BYTE  GetFreq() const;
	void  SetGridBkColor(DWORD dwColor);

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void Notify(TNotifyUI& msg);	
	void SetIndex();
	void SetBeat();
	void SetOxy();
	void SetTemp();
	void SetPose();
	void SetNo();
	void SetFreq();

private:
	int                m_nIndex;
	BYTE               m_beat;
	BYTE               m_beatV;
	BYTE               m_oxy;
	BYTE               m_oxyV;
	int                m_nTemp;
	BYTE               m_pose;
	BYTE               m_no;
	BYTE               m_freq;

	CLabelUI *         m_lblIndex;
	CLabelUI *         m_lblBeat;
	CLabelUI *         m_lblBeatV;
	CLabelUI *         m_lblOxy;
	CLabelUI *         m_lblOxyV;
	CLabelUI *         m_lblTemp;
	CLabelUI *         m_lblPose;
	CLabelUI *         m_lblNo;
	CLabelUI *         m_lblFreq;
	CVerticalLayoutUI *  m_main;
};
