#include "business.h"
#include "UIlib.h"
using namespace DuiLib;

#include <time.h>

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {

}

CBusiness::~CBusiness() {
	Clear();
}

void CBusiness::Clear() {
	if (g_log) {
		g_log->Deinit();
		delete g_log;
		g_log = 0;
	}

	if (g_cfg) {
		g_cfg->Deinit();
		delete g_cfg;
		g_cfg = 0;
	}
}

int CBusiness::Init() {
	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init(LOG_FILE_NAME);

	g_cfg = new FileConfigEx();
	if (0 == g_cfg) {
		return -1;
	}
	g_cfg->Init(CONFIG_FILE_NAME);

	g_thrd_db = new LmnToolkits::Thread();
	if (0 == g_thrd_db) {
		return -1;
	}
	g_thrd_db->Start();

	for (int i = 0; i < MAX_VPORT_COUNT; i++) {
		g_thrd_vport[i] = new LmnToolkits::Thread();
		if (0 == g_thrd_vport[i]) {
			return -1;
		}
		g_thrd_vport[i]->Start();
	}

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_db) {
		g_thrd_db->Stop();
		delete g_thrd_db;
		g_thrd_db = 0;
	}

	Clear();
	return 0;
}


int  CBusiness::OpenCloseVPortAsyn(int nIndex) {
	g_thrd_vport[nIndex]->PostMessage(this, MSG_OPEN_VPORT, new COpenVPortParam(nIndex));
	return 0;
}

int  CBusiness::OpenCloseVPort(const COpenVPortParam * pParam) {
	int nIndex = pParam->m_nIndex;
	BOOL b = FALSE;

	if (g_nInit[nIndex] == 0) {
		InitRand(TRUE, (DWORD)( time(0) % 255 + nIndex ) );
		g_nInit[nIndex] = 1;
	}
	
	CLmnSerialPort::PortStatus s = m_port[nIndex].GetStatus();

	if (s == CLmnSerialPort::CLOSE) {
		CDuiString strText;
		strText.Format("com%d", nIndex * 2 + 2);
		m_port[nIndex].m_recv.Clear();
		b = m_port[nIndex].OpenUartPort(strText);

		if (b) {
			g_thrd_vport[nIndex]->PostMessage(this, MSG_HANDLE_VPORT, new CHandleVPortParam(nIndex));
		}
	}
	else {
		b = m_port[nIndex].CloseUartPort();
		m_port[nIndex].m_recv.Clear();
	}
	
	::PostMessage(g_hWnd, UM_NOTIFY_OPEN_VPORT_RET, (nIndex << 8) | b, s );
	return 0;
}

void  CBusiness::HandleVPort(const CHandleVPortParam * pParam) {
	int nIndex = pParam->m_nIndex;
	int ret = m_port[nIndex].HandleData();
	if (0 != ret) {
		CLmnSerialPort::PortStatus s = m_port[nIndex].GetStatus();
		BOOL b = m_port[nIndex].CloseUartPort();
		m_port[nIndex].m_recv.Clear();
		::PostMessage(g_hWnd, UM_NOTIFY_OPEN_VPORT_RET, (nIndex << 8) | b, s);
	}
	else {
		g_thrd_vport[nIndex]->PostDelayMessage(100, this, MSG_HANDLE_VPORT, new CHandleVPortParam(nIndex));
	}	
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_OPEN_VPORT:
	{
		COpenVPortParam * pParam = (COpenVPortParam *)pMessageData;
		OpenCloseVPort(pParam);
	}
	break;

	case MSG_HANDLE_VPORT:
	{
		CHandleVPortParam * pParam = (CHandleVPortParam*)pMessageData;
		HandleVPort(pParam);
	}
	break;

	default:
		break;
	}
}