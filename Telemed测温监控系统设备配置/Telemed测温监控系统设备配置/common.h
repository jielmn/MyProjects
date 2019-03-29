#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "TelemedDeviceSetting"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MAX_SURGENCY_BEDS_CNT  30
#define   MAX_READERS_PER_BED    6

#define   MSG_SETTING_HAND_READER      1101

#define   UM_SETTING_HAND_READER_RET   (WM_USER+1)

#define   RESTRICTED_FLAG       

class CSettingHandReaderParam : public LmnToolkits::MessageData {
public:
	CSettingHandReaderParam(int nChannel, int nAddr, int nCom) {
		m_nChannel = nChannel;
		m_nAddr = nAddr;
		m_nCom = nCom;
	}

	int   m_nChannel;
	int   m_nAddr;
	int   m_nCom;
};


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_com;
	HWND                      m_hWnd;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_com = 0;
	}
};

extern CGlobalData  g_data;

extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern void SetComboCom(DuiLib::CComboUI * pCombo, std::vector<std::string> & v);

// templates
template <class T>
void ClearVector(std::vector<T> & v) {
	typedef std::vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}