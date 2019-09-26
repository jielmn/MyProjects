#include "GridUI.h"
#include "business.h"
#include "LmnExcel.h"

CGridUI::CGridUI() {
	m_nIndex = 0;
	m_beat = 0;
	m_beatV = 0;
	m_oxy = 0;
	m_oxyV = 0;
	m_nTemp = 0;
	m_pose = 0;
	m_no = 0;
	m_freq = 0;

	m_lblIndex = 0;
	m_lblBeat = 0;
	m_lblBeatV = 0;
	m_lblOxy = 0;
	m_lblOxyV = 0;
	m_lblTemp = 0;
	m_lblPose = 0;
	m_lblNo = 0;
	m_lblFreq = 0;

	m_main = 0;
}

CGridUI::~CGridUI() {
}

LPCTSTR CGridUI::GetClass() const {
	return "GridUI";
}

void CGridUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create("grid.xml", (UINT)0, 0, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();                   
		return;
	}

	m_lblIndex = static_cast<CLabelUI*>(m_pManager->FindControl("lblIndex"));
	m_lblBeat  = static_cast<CLabelUI*>(m_pManager->FindControl("lblBeat"));
	m_lblOxy   = static_cast<CLabelUI*>(m_pManager->FindControl("lblOxy"));
	m_lblTemp  = static_cast<CLabelUI*>(m_pManager->FindControl("lblTemp"));
	m_lblPose  = static_cast<CLabelUI*>(m_pManager->FindControl("lblPose"));
	m_lblNo = static_cast<CLabelUI*>(m_pManager->FindControl("lblNo"));
	m_lblFreq = static_cast<CLabelUI*>(m_pManager->FindControl("lblFrequence"));
	m_main = static_cast<CVerticalLayoutUI*>(m_pManager->FindControl("main"));

	SetIndex();
	SetBeat();
	SetOxy();
	SetTemp();
	SetPose();
	SetNo();
	SetFreq();
}

void CGridUI::Notify(TNotifyUI& msg) {

}

void  CGridUI::SetIndex(int nIndex) {
	m_nIndex = nIndex;	
	SetIndex();
}

void CGridUI::SetIndex() {
	CDuiString  strText;
	if (m_lblIndex) {
		strText.Format("%d", m_nIndex);
		m_lblIndex->SetText(strText);
	}
}

void  CGridUI::SetBeat(BYTE beat, BYTE v) {
	m_beat = beat;
	m_beatV = v;
	SetBeat();
}

void CGridUI::SetBeat() {
	CDuiString  strText;
	if ( m_lblBeat && m_beatV > 0 ) {
		strText.Format("%lu", (DWORD)m_beat);
		m_lblBeat->SetText(strText);
	}
}

void CGridUI::SetOxy(BYTE oxy, BYTE v) {
	m_oxy = oxy;
	m_oxyV = v;
	SetOxy();
}

void CGridUI::SetOxy() {
	CDuiString  strText;
	if (m_lblOxy && m_oxyV > 0) {
		strText.Format("%lu", (DWORD)m_oxy);
		m_lblOxy->SetText(strText);
	}
}

void  CGridUI::SetTemp(int nTemp) {
	m_nTemp = nTemp;
	SetTemp();
}

void  CGridUI::SetTemp() {
	CDuiString  strText;
	if (m_lblTemp) {
		strText.Format("%.2f", m_nTemp / 100.0f);
		m_lblTemp->SetText(strText);
	}
}

void  CGridUI::SetPose(BYTE pose) {
	m_pose = pose;
	SetPose();
}

void  CGridUI::SetPose() {
	CDuiString  strText;
	if (m_lblPose) {
		strText.Format("%lu", (DWORD)m_pose);
		m_lblPose->SetText(strText);
	}
}

void  CGridUI::SetNo(BYTE no) {
	m_no = no;
	SetNo();
}

void  CGridUI::SetNo() {
	CDuiString  strText;
	if (m_lblNo) {
		strText.Format("%lu", (DWORD)m_no);
		m_lblNo->SetText(strText);
	}
}

BYTE  CGridUI::GetNo() const {
	return m_no;
}

void  CGridUI::SetFreq(BYTE freq) {
	m_freq = freq;
	SetFreq();
}

void  CGridUI::SetFreq() {
	CDuiString  strText;
	if (m_lblFreq) {
		strText.Format("%lu", (DWORD)m_freq);
		m_lblFreq->SetText(strText);
	}
}

BYTE  CGridUI::GetFreq() const {
	return m_freq;
}

void  CGridUI::SetGridBkColor(DWORD dwColor) {
	if (m_main) {
		m_main->SetBkColor(dwColor);
	}
}