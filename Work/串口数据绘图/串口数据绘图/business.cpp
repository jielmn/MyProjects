#include "business.h"

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
	if (g_data.m_log) {
		g_data.m_log->Deinit();
		delete g_data.m_log;
		g_data.m_log = 0;
	}

	if (g_data.m_cfg) {
		g_data.m_cfg->Deinit();
		delete g_data.m_cfg;
		g_data.m_cfg = 0;
	}
}

int CBusiness::Init() {
	g_data.m_log = new FileLog();
	if (0 == g_data.m_log) {
		return -1;
	}

#ifdef _DEBUG
	g_data.m_log->Init(LOG_FILE_NAME);
#else
	g_data.m_log->Init(LOG_FILE_NAME, 0, ILog::LOG_SEVERITY_INFO, TRUE);
#endif
	
	g_color.insert(std::make_pair(1, 0xFF1b9375));
	g_color.insert(std::make_pair(2, 0xFFFF0000));
	g_color.insert(std::make_pair(3, 0xFF51786C));
	g_color.insert(std::make_pair(4, 0xFFCA5100));


	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_com) {
		g_data.m_thrd_com->Stop();
		delete g_data.m_thrd_com;
		g_data.m_thrd_com = 0;
	}

	Clear();
	return 0;
}

void  CBusiness::OpenComAsyn(int nCom, int nBaud) {
	g_data.m_thrd_com->PostMessage(this, MSG_OPEN_COM, new COpenComParam(nCom, nBaud) );
}

void  CBusiness::OpenCom(const COpenComParam * pParam) {
	BOOL bRet = m_com.OpenUartPort(pParam->m_nCom, pParam->m_nBaud);
	::PostMessage(g_data.m_hWnd, UM_OPEN_COM_RET, bRet, 0);
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_OPEN_COM:
	{
		COpenComParam * pParam = (COpenComParam *)pMessageData;
		OpenCom(pParam);
	}
	break;

	default:
		break;
	}
}