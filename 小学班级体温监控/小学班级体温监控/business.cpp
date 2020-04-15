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

void   CBusiness::GetAllClassesAsyn() {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_ALL_CLASSES);
}

void   CBusiness::GetAllClasses() {
	std::vector<DWORD> * pvRet = new std::vector<DWORD>;
	m_db.GetAllClasses(*pvRet);
	::PostMessage(g_data.m_hWnd, UM_GET_ALL_CLASSES_RET, (WPARAM)pvRet, 0);
}

void  CBusiness::GetRoomDataAsyn(DWORD dwNo) {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_ROOM, new CGetRoomParam(dwNo));
}

void  CBusiness::GetRoomData(const CGetRoomParam * pParam) {
	std::vector<DeskItem*> * pvRet = new std::vector<DeskItem*>;
	m_db.GetRoomData(pParam, *pvRet);
	::PostMessage(g_data.m_hWnd, UM_GET_ROOM_RET, (WPARAM)pvRet, pParam->m_dwNo);
}

void  CBusiness::ModifyDeskAsyn(DWORD dwClassNo, DWORD dwPos, const char * szName, int nSex) {
	g_data.m_thrd_db->PostMessage(this, MSG_MODIFY_DESK, new CModifyDeskParam(dwClassNo, dwPos, szName,nSex));
}

void  CBusiness::ModifyDesk(const CModifyDeskParam * pParam) {
	m_db.ModifyDesk(pParam);
	DeskItem * pItem = new DeskItem;
	memset(pItem, 0, sizeof(DeskItem));
	STRNCPY(pItem->szName, pParam->m_szName, sizeof(pItem->szName));
	pItem->nSex   = pParam->m_nSex;
	pItem->nGrade = HIWORD(pParam->m_dwNo);
	pItem->nClass = LOWORD(pParam->m_dwNo);
	pItem->nRow   = HIWORD(pParam->m_dwPos);
	pItem->nCol   = LOWORD(pParam->m_dwPos);
	::PostMessage(g_data.m_hWnd, UM_MODIFY_DESK_RET, (WPARAM)pItem, 0);
}

void   CBusiness::EmptyDeskAsyn(DWORD dwClassNo, DWORD dwPos) {
	g_data.m_thrd_db->PostMessage(this, MSG_EMPTY_DESK, new CEmptyDeskParam(dwClassNo, dwPos));
}

void   CBusiness::EmptyDesk(const CEmptyDeskParam * pParam) {
	m_db.EmptyDesk(pParam);
	::PostMessage(g_data.m_hWnd, UM_EMPTY_DESK_RET, pParam->m_dwNo, pParam->m_dwPos);
}

void  CBusiness::DeleteClassAsyn(DWORD  dwClassNo) {
	g_data.m_thrd_db->PostMessage(this, MSG_DELETE_CLASS, new CDeleteClassParam(dwClassNo));
}

void  CBusiness::DeleteClass(const CDeleteClassParam * pParam) {
	m_db.DeleteClass(pParam);
	::PostMessage(g_data.m_hWnd, UM_DELETE_CLASS_RET, pParam->m_dwNo, 0);
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

	case MSG_GET_ALL_CLASSES:
	{
		GetAllClasses();
	}
	break;

	case MSG_GET_ROOM:
	{
		CGetRoomParam * pParam = (CGetRoomParam *)pMessageData;
		GetRoomData(pParam);
	}
	break;

	case MSG_MODIFY_DESK:
	{
		CModifyDeskParam * pParam = (CModifyDeskParam *)pMessageData;
		ModifyDesk(pParam);
	}
	break;

	case MSG_EMPTY_DESK:
	{
		CEmptyDeskParam * pParam = (CEmptyDeskParam *)pMessageData;
		EmptyDesk(pParam);
	}
	break;

	case MSG_DELETE_CLASS:
	{
		CDeleteClassParam * pParam = (CDeleteClassParam *)pMessageData;
		DeleteClass(pParam);
	}
	break;

	default:
		break;
	}
}