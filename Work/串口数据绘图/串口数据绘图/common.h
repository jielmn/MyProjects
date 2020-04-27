#pragma once

#include <vector>
#include <algorithm>
#include <map>
#include <iterator>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "compaint"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\" PROJ_NAME "_res")


#define   MSG_OPEN_COM               1
#define   MSG_READ_COM               2

#define   UM_OPEN_COM_RET            (WM_USER + 1)
#define   UM_WRONG_LUA               (WM_USER + 2)
#define   UM_COM_DATA                (WM_USER + 3)

#define   MAX_CHANNEL_COUNT          4


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	HWND                      m_hWnd;
	HCURSOR                   m_cursor_we;
	HCURSOR                   m_cursor_ns;
	LmnToolkits::Thread *     m_thrd_com;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_com = 0;
		m_hWnd = 0;
		m_cursor_we = 0;
		m_cursor_ns = 0;
	}
};


class  CChannel {
public:
	CChannel(DWORD  dwArgb = 0xFF000000 );
	~CChannel();

	std::vector<int>   m_vValues;
	Pen                m_pen;
	SolidBrush         m_brush;
};

class COpenComParam : public LmnToolkits::MessageData {
public:
	COpenComParam(int nCom, int nBaud) {
		m_nCom = nCom;
		m_nBaud = nBaud;
	}

	int       m_nCom;
	int       m_nBaud;
};

extern CGlobalData  g_data;
extern std::map<int, DWORD>        g_color;

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