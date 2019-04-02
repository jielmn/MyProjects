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

#define   MAX_AREA_COUNT         20

#define   MSG_SETTING_HAND_READER      1101
#define   MSG_SET_HAND_READER_SN       1102
#define   MSG_SET_RECIEVER_CHANNEL     1103
#define   MSG_SET_SURGENCY_READER      1104

#define   UM_SETTING_HAND_READER_RET   (WM_USER+1)
#define   UM_SET_HAND_READER_SN_RET    (WM_USER+2)
#define   UM_SET_RECIEVER_CHANNEL_RET  (WM_USER+3)
#define   UM_SET_SURGENCY_READER_RET   (WM_USER+4)

#define   RESTRICTED_FLAG       

class CSettingHandReaderParam : public LmnToolkits::MessageData {
public:
	CSettingHandReaderParam(BYTE byChannel, BYTE byAddr, int nCom) {
		m_byChannel = byChannel;
		m_byAddr = byAddr;
		m_nCom = nCom;
	}

	int    m_nCom;
	BYTE   m_byChannel;
	BYTE   m_byAddr;	
	char   m_reserved[2];
};

class CSetHandReaderSnParam : public LmnToolkits::MessageData {
public:
	CSetHandReaderSnParam(DWORD dwSn, int nCom) {
		m_dwSn = dwSn;
		m_nCom = nCom;
	}

	int    m_nCom;
	DWORD  m_dwSn;
};

class CSetReceiverChannelParam : public LmnToolkits::MessageData {
public:
	CSetReceiverChannelParam(BYTE a, BYTE b, BYTE c, int nCom) {
		m_byChannelA = a;
		m_byChannelB = b;
		m_byChannelC = c;
		m_nCom = nCom;
	}

	int     m_nCom;
	BYTE    m_byChannelA;
	BYTE    m_byChannelB;
	BYTE    m_byChannelC;
	BYTE    m_reserved[1];
};

class CSetSurgencyReaderParam : public LmnToolkits::MessageData {
public:
	CSetSurgencyReaderParam(WORD wAddr, BYTE byChannel, int nCom) {
		m_wAddr = wAddr;
		m_byChannel = byChannel;
		m_nCom = nCom;
	}

	int     m_nCom;
	WORD    m_wAddr;
	BYTE    m_byChannel;
	BYTE    m_reserved[1];
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

typedef struct tagArea {
	char   szAreaName[64];
	DWORD  dwAreaNo;
}TArea;

extern CGlobalData  g_data;
extern std::vector<TArea *>  g_vArea;
extern IConfig * g_cfg_area;

extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern void SetComboCom(DuiLib::CComboUI * pCombo, std::vector<std::string> & v);
extern DWORD  FindNewAreaId(const std::vector<TArea *> & v);
extern void  SaveAreas();

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