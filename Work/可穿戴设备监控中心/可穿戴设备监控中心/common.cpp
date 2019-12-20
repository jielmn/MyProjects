#include <time.h>
#include "common.h"
#include "EditableButtonUI.h"

CGlobalData  g_data;

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "EditableButton")) {
		return new CEditableButtonUI;
	}
	return NULL;
}

CWearItem::CWearItem() {
	memset(m_szDeviceId, 0, sizeof(m_szDeviceId));
	memset(m_szName, 0, sizeof(m_szName));
	m_nPose = 0;
}

CWearItem::~CWearItem() {
	ClearVector(m_vHearbeat);
	ClearVector(m_vOxy);
	ClearVector(m_vTemp);
}