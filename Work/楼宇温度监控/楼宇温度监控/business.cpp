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

	m_db.InitDb();

	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	Clear();

	m_db.DeinitDb();

	return 0;
}

void  CBusiness::AddFloorAsyn(HWND hWnd, int nFloor) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADD_FLOOR, new CAddFloorParam(hWnd, nFloor));
}

void  CBusiness::AddFloor(const CAddFloorParam * pParam) {
	BOOL bRet = m_db.AddFloor(pParam);
	::PostMessage(pParam->m_hWnd, UM_ADD_FLOOR_RET, bRet, pParam->m_nFloor);
}

void  CBusiness::GetAllFloorsAsyn() {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_ALL_CLASSES);
}

void  CBusiness::GetAllFloors() {
	std::vector<int> * pvRet = new std::vector<int>;
	m_db.GetAllFloors(*pvRet);
	::PostMessage(g_data.m_hWnd, UM_GET_ALL_FLOORS_RET, (WPARAM)pvRet, 0);
}

void  CBusiness::GetDevicesByFloorAsyn(int nFloor) {

}

void  CBusiness::GetDevicesByFloor() {

}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_ADD_FLOOR:
	{
		CAddFloorParam * pParam = (CAddFloorParam *)pMessageData;
		AddFloor(pParam);
	}
	break;

	case MSG_GET_ALL_CLASSES:
	{
		GetAllFloors();
	}
	break;

	default:
		break;
	}
}