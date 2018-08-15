#include "MyProgress.h"

// progress UI
CMyProgress::CMyProgress(DuiLib::CPaintManagerUI *p, DuiLib::CDuiString sForeImage) :m_pManager(p), m_nPos(0), m_sForeImage(sForeImage) {
}
CMyProgress::~CMyProgress() {}

void CMyProgress::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == DuiLib::UIEVENT_TIMER && event.wParam == 10)
	{
		const int MARGIN = 3;
		const int STEP = 4;
		DuiLib::CDuiString imageProp;
		const int PROGRESS_WIDTH = 36;
		const int PROGRESS_HEIGHT = 7;
		const int HORIZONTAL_MARGIN = 3;

		int width = this->GetWidth();
		int height = this->GetHeight();

		width -= 2 * HORIZONTAL_MARGIN;

		if (m_nPos < PROGRESS_WIDTH) {
			imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage,
				PROGRESS_WIDTH - m_nPos, 0, PROGRESS_WIDTH, PROGRESS_HEIGHT, HORIZONTAL_MARGIN,
				MARGIN, HORIZONTAL_MARGIN + m_nPos, height - MARGIN);
		}
		else if (m_nPos > width) {
			imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage,
				0, 0, PROGRESS_WIDTH + width - m_nPos, PROGRESS_HEIGHT,
				m_nPos - PROGRESS_WIDTH + HORIZONTAL_MARGIN, MARGIN, width + HORIZONTAL_MARGIN, height - MARGIN);
		}
		else {
			imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage, 0, 0, PROGRESS_WIDTH, PROGRESS_HEIGHT,
				m_nPos - PROGRESS_WIDTH + HORIZONTAL_MARGIN, MARGIN, m_nPos + HORIZONTAL_MARGIN, height - MARGIN);
		}

		this->SetForeImage(imageProp);

		m_nPos += STEP;
		if (m_nPos > width + PROGRESS_WIDTH) {
			m_nPos = 0;
		}
		return;
	}

	DuiLib::CProgressUI::DoEvent(event);
}

void CMyProgress::Stop() {
	if (m_pManager) {
		m_pManager->KillTimer(this, 10);
	}
}

void CMyProgress::Start() {
	if (m_pManager) {
		m_pManager->SetTimer(this, 10, 220);
	}
	m_nPos = 0;
}