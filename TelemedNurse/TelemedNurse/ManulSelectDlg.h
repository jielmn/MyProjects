#pragma once

#include "Business.h"
#include "UIlib.h"
using namespace DuiLib;

class CManulSelectWnd : public WindowImplBase
{
public:
	CManulSelectWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIPatientFrame"); }
	virtual CDuiString GetSkinFile() { return _T("ManulSelect.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

private:
	virtual void  Notify(TNotifyUI& msg);
	virtual void   InitWindow();
	LRESULT   HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnTimeChanged(time_t & tNewTime);
	void  OnItemSelected(const CDuiString & name);

public:
	std::vector<TagData*> *  m_pvTempetatureData;
	TagData                  m_TempData[MAX_SPAN_DAYS][6];
	time_t                   m_tFirstTime;
	int                      m_nWeekIndex;

	sigslot::signal1<TagData[MAX_SPAN_DAYS][6]>    m_sigOK;

private:
	
};
