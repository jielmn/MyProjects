#include "WaitingBarUI.h"


CWaitingBarUI::CWaitingBarUI() : m_nPos(0) {
	m_sForeImage = "progress_fore_1.png";
	this->SetBkImage("file='progress_back.png' corner='5,5,5,5'");
}

CWaitingBarUI::~CWaitingBarUI() {}

void CWaitingBarUI::DoEvent(TEventUI& event) {
	if (event.Type == UIEVENT_TIMER && event.wParam == 10)
	{
		const int MARGIN = 3;
		const int STEP = 4;
		CDuiString imageProp;
		const int PROGRESS_WIDTH = 144;
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

	CProgressUI::DoEvent(event);
}

void CWaitingBarUI::Stop() {
	if (m_pManager) {
		m_pManager->KillTimer(this, 10);
	}
}

void CWaitingBarUI::Start() {
	if (m_pManager) {
		m_pManager->SetTimer(this, 10, 200);
	}
	m_nPos = 0;
}