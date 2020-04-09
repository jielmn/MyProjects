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

	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	g_data.m_cfg->GetConfig("room rows", g_data.m_dwRoomRows, 6);
	g_data.m_cfg->GetConfig("room cols", g_data.m_dwRoomCols, 8);

	if (g_data.m_dwRoomRows == 0) {
		g_data.m_dwRoomRows = 6;
	}
	if (g_data.m_dwRoomCols == 0) {
		g_data.m_dwRoomCols = 8;
	}

	g_data.m_DeskSize.cx = 94;                           
	g_data.m_DeskSize.cy = 88;               
	         
	STRNCPY(g_data.m_aszChNo[0], "一", 6);
	STRNCPY(g_data.m_aszChNo[1], "二", 6);
	STRNCPY(g_data.m_aszChNo[2], "三", 6);
	STRNCPY(g_data.m_aszChNo[3], "四", 6);
	STRNCPY(g_data.m_aszChNo[4], "五", 6);
	STRNCPY(g_data.m_aszChNo[5], "六", 6);
	STRNCPY(g_data.m_aszChNo[6], "七", 6);
	STRNCPY(g_data.m_aszChNo[7], "八", 6);
	STRNCPY(g_data.m_aszChNo[8], "九", 6);
	STRNCPY(g_data.m_aszChNo[9], "十", 6);

	m_db.InitDb();

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

void   CBusiness::AddClassAsyn( HWND hWnd, DWORD dwGrade, DWORD  dwClass) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADD_CLASS, new CAddClassParam(hWnd, dwGrade, dwClass));
}

void   CBusiness::AddClass(const CAddClassParam * pParam) {
	BOOL bRet = m_db.AddClass(pParam);
	::PostMessage(pParam->m_hWnd, UM_ADD_CLASS_RET, bRet, 0);
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_ADD_CLASS:
	{
		CAddClassParam * pParam = (CAddClassParam *)pMessageData;
		AddClass(pParam);
	}
	break;

	default:
		break;
	}
}