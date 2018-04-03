#include <assert.h>
#include <afx.h>
#include "Business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() : m_Database(this), m_BindingReader(this) {
	sigInit.connect(this, &CBusiness::OnInit);
	sigDeinit.connect(this, &CBusiness::OnDeInit);
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


void CBusiness::OnInit(int * ret) {
	assert(ret);
	g_log = new FileLog();
	if (0 == g_log) {
		*ret = ZS_ERR_NO_MEMORY;
		return;
	}
	g_log->Init("NurseCenter.log");

	g_cfg = new FileConfig();
	if (0 == g_cfg) {
		*ret = ZS_ERR_NO_MEMORY;
		return;
	}
	g_cfg->Init("NurseCenter.cfg");

	g_thrd_db = new LmnToolkits::Thread();
	if (0 == g_thrd_db) {
		*ret = ZS_ERR_NO_MEMORY;
		return;
	}
	g_thrd_db->Start();

	g_thrd_reader = new LmnToolkits::Thread();
	if (0 == g_thrd_reader) {
		*ret = ZS_ERR_NO_MEMORY;
		return;
	}
	g_thrd_reader->Start();

	// timer线程
	g_thrd_timer = new LmnToolkits::Thread();
	if (0 == g_thrd_timer) {
		*ret = ZS_ERR_NO_MEMORY;
		return;
	}
	g_thrd_timer->Start();

	if (0 != m_BindingReader.Init()) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to load binding reader DLL!\n");
		*ret = ZS_ERR_FAILED_TO_LOAD_FUNCTION_DLL;
		return;
	}

	// 连接数据库
	ReconnectDatabaseAsyn(1000);

	// 盘点
	InventoryAsyn(1000);

	*ret = 0;
}

void CBusiness::OnDeInit(int * ret) {
	assert(ret);

	if (g_thrd_db) {
		g_thrd_db->Stop();
		delete g_thrd_db;
		g_thrd_db = 0;
	}


	if (g_thrd_reader) {
		g_thrd_reader->Stop();
		delete g_thrd_reader;
		g_thrd_reader = 0;
	}

	if (g_thrd_timer) {
		g_thrd_timer->Stop();
		delete g_thrd_timer;
		g_thrd_timer = 0;
	}

	Clear();

	*ret = 0;
}

// 重连数据库(异步)
int   CBusiness::ReconnectDatabaseAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_db->PostMessage(this, MSG_RECONNECT_DB);
	}
	else {
		g_thrd_db->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_DB);
	}
	return 0;
}

// 重连数据库
int   CBusiness::ReconnectDatabase() {
	int ret = m_Database.ReconnectDb();
	return ret;
}

int   CBusiness::NotifyUiDbStatus(CZsDatabase::DATABASE_STATUS eStatus) {
	::PostMessage(g_hWnd, UM_SHOW_DB_STATUS, eStatus, 0);
	return 0;
}

CZsDatabase::DATABASE_STATUS  CBusiness::GetDbStatus() {
	return  m_Database.GetStatus();
}


// 盘点
int   CBusiness::InventoryAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_reader->PostMessage(this, MSG_INVENTORY);
	}
	else {
		g_thrd_reader->PostDelayMessage(dwDelayTime, this, MSG_INVENTORY);
	}
	return 0;
}

int   CBusiness::Inventory() {
	TagItem  item;
	memset(&item, 0, sizeof(item));

	CZsBindingReader::BINDING_STATUS eOldStatus = m_BindingReader.GetStatus();
	int ret =  m_BindingReader.Inventory(&item);
	CZsBindingReader::BINDING_STATUS eNewStatus = m_BindingReader.GetStatus();

	// 通知UI界面
	if (eOldStatus != eNewStatus) {
		NotifyUiBindingReaderStatus(eNewStatus);
	}

	// 如果是成功的盘点，则在下次盘点时留下足够长的时间
	DWORD  dwRelayTime = 1000;
	if (0 == ret) {
		dwRelayTime = 2000;
	}

	// 再次盘点
	InventoryAsyn(dwRelayTime);

	return 0;
}

int   CBusiness::NotifyUiBindingReaderStatus(CZsBindingReader::BINDING_STATUS eStatus) {
	::PostMessage(g_hWnd, UM_SHOW_BINDING_READER_STATUS, eStatus, 0);
	return 0;
}

CZsBindingReader::BINDING_STATUS  CBusiness::GetBindingReaderStatus() {
	return m_BindingReader.GetStatus();
}

int  CBusiness::AddPatientAsyn(const PatientInfo * pPatient, HWND hWnd) {
	g_thrd_db->PostMessage( this, MSG_ADD_PATIENT, new CAddPatientParam(pPatient, hWnd) );
	return 0;
}

int  CBusiness::AddPatient(const CAddPatientParam * pParam) {
	DWORD  dwId = 0;
	int ret = m_Database.AddPatient(pParam, dwId);
	NotifyUiAddPatientRet(pParam->m_hWnd, ret, dwId );
	return 0;
}

// 通知添加结果
int   CBusiness::NotifyUiAddPatientRet(HWND hWnd, int ret, DWORD dwId) {
	::PostMessage( hWnd, UM_ADD_PATIENT_RET, ret, dwId );
	return 0;
}

// 获取所有的病人信息 
int   CBusiness::GetAllPatientsAsyn() {
	return 0;
}

int   CBusiness::GetAllPatients() {
	return 0;
}

int  CBusiness::NotifyUiGetAllPatientsRet() {
	return 0;
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	int ret = 0;

	switch (dwMessageId)
	{

	case MSG_RECONNECT_DB:
	{
		ReconnectDatabase();
	}
	break;

	case MSG_INVENTORY:
	{
		Inventory();
	}
	break;

	case MSG_ADD_PATIENT:
	{
		CAddPatientParam * pParam = (CAddPatientParam *)pMessageData;
		AddPatient(pParam);
	}
	break;

	default:
	{

	}
	break;

	}
}



