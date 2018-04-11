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

CBusiness::CBusiness() : m_Database(this), m_BindingReader(this), m_SyncReader(this) {
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

	// 同步Reader线程
	g_thrd_sync_reader = new LmnToolkits::Thread();
	if (0 == g_thrd_sync_reader) {
		*ret = ZS_ERR_NO_MEMORY;
		return;
	}
	g_thrd_sync_reader->Start();
	

	if (0 != m_BindingReader.Init()) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to load binding reader DLL!\n");
		*ret = ZS_ERR_FAILED_TO_LOAD_FUNCTION_DLL;
		return;
	}

	char buf[8192];
	// 获取Reader协议命令
	g_cfg->GetConfig("sync command", buf, sizeof(buf), "55 01 03 dd aa");
	TransferReaderCmd(SYNC_COMMAND, buf);

	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "55 01 01 dd aa");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("clear command", buf, sizeof(buf), "55 01 04 dd aa");
	TransferReaderCmd(CLEAR_COMMAND, buf);


	// 连接数据库
	ReconnectDatabaseAsyn(200);

	// 盘点
	InventoryAsyn(200);

	// 同步读卡器
	ReconnectSyncReaderAsyn(200);

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

	if (g_thrd_sync_reader) {
		g_thrd_sync_reader->Stop();
		delete g_thrd_sync_reader;
		g_thrd_sync_reader = 0;
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

	// 通知UI界面状态
	if (eOldStatus != eNewStatus) {
		NotifyUiBindingReaderStatus(eNewStatus);
	}

	// 通知界面盘点结果
	if (0 == ret) {
		TagItem * pItem = new TagItem;
		if (pItem) {
			memcpy( pItem, &item, sizeof(TagItem) );
			::PostMessage(g_hWnd, UM_INVENTORY_RET, ret, (LPARAM)pItem );
		}		
	}
	else {
		::PostMessage(g_hWnd, UM_INVENTORY_RET, ret, 0);
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
	g_thrd_db->PostMessage( this, MSG_ADD_PATIENT, new CPatientParam(pPatient, hWnd) );
	return 0;
}

int  CBusiness::AddPatient(const CPatientParam * pParam) {
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
	g_thrd_db->PostMessage(this, MSG_GET_ALL_PATIENTS );
	return 0;
}

int   CBusiness::GetAllPatients() {
	std::vector<PatientInfo * > * pvRet = new std::vector<PatientInfo * >;
	int ret = 0;
	if (0 == pvRet) {
		ret = -1;
	} else {
		ret = m_Database.GetAllPatients(*pvRet);
	}

	NotifyUiGetAllPatientsRet(ret, pvRet);
	return 0;
}

int  CBusiness::NotifyUiGetAllPatientsRet( int ret, std::vector<PatientInfo * > * pvRet ) {
	::PostMessage(g_hWnd, UM_GET_ALL_PATIENTS_RET, ret, (LPARAM)pvRet);
	return 0;
}

// 修改病人
int   CBusiness::ModifyPatientAsyn(const PatientInfo * pPatient, HWND hWnd) {
	g_thrd_db->PostMessage(this, MSG_MODIFY_PATIENT, new CPatientParam(pPatient, hWnd));
	return 0;
}

int   CBusiness::ModifyPatient(const CPatientParam * pParam) {
	int ret = m_Database.ModifyPatient(pParam);
	NotifyUiModifyPatientRet(pParam->m_hWnd, ret);
	return 0;
}

// 通知修改结果
int   CBusiness::NotifyUiModifyPatientRet(HWND hWnd, int ret) {
	::PostMessage(hWnd, UM_MODIFY_PATIENT_RET, ret, 0);
	return 0;
}

int   CBusiness::DeletePatientAsyn(DWORD dwId) {
	g_thrd_db->PostMessage(this, MSG_DELETE_PATIENT, new CDeletePatientParam(dwId));
	return 0;
}

int   CBusiness::DeletePatient(const CDeletePatientParam * pParam) {
	int ret = m_Database.DeletePatient(pParam);
	NotifyUiDeletePatientRet(ret, pParam->m_dwId);
	return 0;
}

// 通知删除结果
int   CBusiness::NotifyUiDeletePatientRet(int ret , DWORD dwId) {
	::PostMessage(g_hWnd, UM_DELETE_PATIENT_RET, ret, dwId);
	return 0;
}

// 导入病人
int   CBusiness::ImportPatientsAsyn( const char * szFilePath ) {
	g_thrd_db->PostMessage(this, MSG_IMPORT_PATIENTS, new CImportPatientsParam(szFilePath) );
	return 0;
}

int   CBusiness::ImportPatients(const CImportPatientsParam * pParam, std::vector<PatientInfo*> & vRet ) {
	CString sDriver = GetExcelDriver();
	if (sDriver.GetLength() == 0) {
		return ZS_ERR_EXCEL_DRIVER_NOT_FOUND;
	}

	CString sDsn;
	CDatabase database;
	CString sSql;
	CString sItem;
	int ret = 0;
	int nTmp = 0;

	const char * szFilePath = pParam->m_szFilePath;
	sDsn.Format("ODBC;DRIVER={%s};DSN='''';DBQ=%s", sDriver, szFilePath);
	TRY
	{
		//open db  
		database.Open(NULL,false,false,sDsn);
		CRecordset recset(&database);
		//read sql   
		sSql = "SELECT * from [病人信息$]";
		recset.Open(CRecordset::forwardOnly,sSql,CRecordset::readOnly);

		//int n = recset.GetODBCFieldCount();
		//get query results  
		//BOOL bFailed = FALSE;

		while (!recset.IsEOF())
		{
			PatientInfo tInfo;
			memset(&tInfo, 0, sizeof(PatientInfo));

			//read inside value  
			recset.GetFieldValue("编号",     sItem);
			strncpy_s(tInfo.szId, sItem, sizeof(tInfo.szId));

			recset.GetFieldValue("姓名",     sItem);
			strncpy_s(tInfo.szName, sItem, sizeof(tInfo.szName));

			recset.GetFieldValue("性别",     sItem);
			if (0 == strcmp(sItem, "女")) {
				tInfo.bFemale = TRUE;
			}
			else {
				tInfo.bFemale = FALSE;
			}

			recset.GetFieldValue("床号",     sItem);
			strncpy_s(tInfo.szBedNo, sItem, sizeof(tInfo.szBedNo));

			recset.GetFieldValue("状态", sItem);
			if (0 == strcmp(sItem, "出院")) {
				tInfo.bOutHos = TRUE;
			}
			else {
				tInfo.bOutHos = FALSE;
			}

			nTmp = m_Database.ImportPatient(&tInfo);
			if (0 != nTmp) {
				ret = ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL;
			}
			else {
				PatientInfo * pNewInfo = new PatientInfo;
				if (pNewInfo) {
					memcpy(pNewInfo, &tInfo, sizeof(PatientInfo));
					vRet.push_back(pNewInfo);
				}
			}

			recset.MoveNext();
		}

		//close db  
		database.Close();

		return ret;
	}
	CATCH(CDBException, e)
	{
		//db exception occur  
		database.Close();
		return ZS_ERR_FAILED_TO_EXECUTE_EXCEL;
	}
	END_CATCH;
	return 0;
}

// 通知导入结果
int   CBusiness::NotifyUiImportPatientsRet(int ret, std::vector<PatientInfo*> * pvRet ) {
	if ( (0 != ret) && (ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL != ret) ) {
		ClearVector(*pvRet);
		delete pvRet;
		pvRet = 0;
	}

	::PostMessage(g_hWnd, UM_NOTIFY_IMPORT_PATIENTS_RET, ret, (LPARAM)pvRet);
	return 0;
}

// 添加护士
int   CBusiness::AddNurseAsyn(const NurseInfo * pNurse, HWND hWnd) {
	g_thrd_db->PostMessage(this, MSG_ADD_NURSE, new CNurseParam(pNurse, hWnd));
	return 0;
}

int   CBusiness::AddNurse(const CNurseParam * pParam) {
	DWORD  dwId = 0;
	int ret = m_Database.AddNurse(pParam, dwId);
	NotifyUiAddNurseRet(pParam->m_hWnd, ret, dwId);
	return 0;
}

// 通知添加结果
int   CBusiness::NotifyUiAddNurseRet(HWND hWnd, int ret, DWORD dwId) {
	::PostMessage(hWnd, UM_ADD_NURSE_RET, ret, dwId);
	return 0;
}

// 请求所有的护士信息
int   CBusiness::GetAllNursesAsyn() {
	g_thrd_db->PostMessage(this, MSG_GET_ALL_NURSES);
	return 0;
}

int   CBusiness::GetAllNurses() {
	std::vector<NurseInfo * > * pvRet = new std::vector<NurseInfo * >;
	int ret = 0;
	if (0 == pvRet) {
		ret = -1;
	}
	else {
		ret = m_Database.GetAllNurses(*pvRet);
	}

	NotifyUiGetAllNursesRet(ret, pvRet);
	return 0;
}

int   CBusiness::NotifyUiGetAllNursesRet(int ret, std::vector<NurseInfo * > * pvRet) {
	::PostMessage(g_hWnd, UM_GET_ALL_NURSES_RET, ret, (LPARAM)pvRet);
	return 0;
}

// 修改护士
int   CBusiness::ModifyNurseAsyn(const NurseInfo * pNurse, HWND hWnd) {
	g_thrd_db->PostMessage(this, MSG_MODIFY_NURSE, new CNurseParam(pNurse, hWnd));
	return 0;
}

int   CBusiness::ModifyNurse(const CNurseParam * pParam) {
	int ret = m_Database.ModifyNurse(pParam);
	NotifyUiModifyNurseRet(pParam->m_hWnd, ret);
	return 0;
}

// 通知修改结果
int   CBusiness::NotifyUiModifyNurseRet(HWND hWnd, int ret) {
	::PostMessage(hWnd, UM_MODIFY_NURSE_RET, ret, 0);
	return 0;
}

// 删除护士
int   CBusiness::DeleteNurseAsyn(DWORD dwId) {
	g_thrd_db->PostMessage(this, MSG_DELETE_NURSE, new CDeleteNurseParam(dwId));
	return 0;
}

int   CBusiness::DeleteNurse(const CDeleteNurseParam * pParam) {
	int ret = m_Database.DeleteNurse(pParam);
	NotifyUiDeleteNurseRet(ret, pParam->m_dwId);
	return 0;
}

// 通知删除结果
int   CBusiness::NotifyUiDeleteNurseRet(int ret, DWORD dwId) {
	::PostMessage(g_hWnd, UM_DELETE_NURSE_RET, ret, dwId);
	return 0;
}

// 导入护士
int   CBusiness::ImportNursesAsyn(const char * szFilePath) {
	g_thrd_db->PostMessage(this, MSG_IMPORT_NURSES, new CImportNursesParam(szFilePath));
	return 0;
}

int   CBusiness::ImportNurses(const CImportNursesParam * pParam, std::vector<NurseInfo*> & vRet) {
	CString sDriver = GetExcelDriver();
	if (sDriver.GetLength() == 0) {
		return ZS_ERR_EXCEL_DRIVER_NOT_FOUND;
	}

	CString sDsn;
	CDatabase database;
	CString sSql;
	CString sItem;
	int ret = 0;
	int nTmp = 0;

	const char * szFilePath = pParam->m_szFilePath;
	sDsn.Format("ODBC;DRIVER={%s};DSN='''';DBQ=%s", sDriver, szFilePath);
	TRY
	{
		//open db  
		database.Open(NULL,false,false,sDsn);
		CRecordset recset(&database);
		//read sql   
		sSql = "SELECT * from [护士信息$]";
		recset.Open(CRecordset::forwardOnly,sSql,CRecordset::readOnly);

		//int n = recset.GetODBCFieldCount();
		//get query results  
		//BOOL bFailed = FALSE;

		while (!recset.IsEOF())
		{
			NurseInfo tInfo;
			memset(&tInfo, 0, sizeof(NurseInfo));

			//read inside value  
			recset.GetFieldValue("编号",     sItem);
			strncpy_s(tInfo.szId, sItem, sizeof(tInfo.szId));

			recset.GetFieldValue("姓名",     sItem);
			strncpy_s(tInfo.szName, sItem, sizeof(tInfo.szName));			

			nTmp = m_Database.ImportNurse(&tInfo);
			if (0 != nTmp) {
				ret = ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL;
			}
			else {
				NurseInfo * pNewInfo = new NurseInfo;
				if (pNewInfo) {
					memcpy(pNewInfo, &tInfo, sizeof(NurseInfo));
					vRet.push_back(pNewInfo);
				}
			}

			recset.MoveNext();
		}

		//close db  
		database.Close();

		return ret;
	}
	CATCH(CDBException, e)
	{
		//db exception occur  
		database.Close();
		return ZS_ERR_FAILED_TO_EXECUTE_EXCEL;
	}
	END_CATCH;
	return 0;
}

// 通知导入结果
int   CBusiness::NotifyUiImportNursesRet(int ret, std::vector<NurseInfo*> * pvRet) {
	if ((0 != ret) && (ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL != ret)) {
		ClearVector(*pvRet);
		delete pvRet;
		pvRet = 0;
	}

	::PostMessage(g_hWnd, UM_NOTIFY_IMPORT_NURSES_RET, ret, (LPARAM)pvRet);
	return 0;
}

// 检查Tag有无绑定
int   CBusiness::CheckTagBindingAsyn(const TagItem * pItem) {
	g_thrd_db->PostMessage(this, MSG_CHECK_TAG_BINDING, new CTagItemParam(pItem) );
	return 0;
}

int   CBusiness::CheckTagBinding(const CTagItemParam * pParam) {
	DWORD dwPatientId = 0;
	int ret = m_Database.CheckTagBinding(pParam, dwPatientId);
	NotifyUiCheckTagBindingRet(dwPatientId, &pParam->m_tag);
	return 0;
}

int   CBusiness::NotifyUiCheckTagBindingRet( DWORD dwPatientId, const TagItem * pItem ) {
	TagItem * pNewItem = new TagItem;
	if (0 == pNewItem) {
		::PostMessage(g_hWnd, UM_CHECK_TAG_BINDING_RET, -1, 0 );
	}
	else {
		memcpy(pNewItem, pItem, sizeof(TagItem));
		::PostMessage(g_hWnd, UM_CHECK_TAG_BINDING_RET, dwPatientId, (LPARAM)pNewItem );
	}
	
	return 0;
}

// 绑定
int   CBusiness::BindingPatientAsyn(DWORD dwPatientId, const TagItem * pItem) {
	g_thrd_db->PostMessage(this, MSG_BINDING_PATIENT, new CBindingPatientParam( dwPatientId, pItem));
	return 0;
}

int   CBusiness::BindingPatient(const CBindingPatientParam * pParam) {
	int ret = m_Database.BindingPatient(pParam);
	NotifyUiBindingPatientRet(ret, pParam);
	return 0;
}

int   CBusiness::NotifyUiBindingPatientRet(int ret, const CBindingPatientParam * pParam) {
	CBindingPatientParam * p = new CBindingPatientParam(pParam->m_dwPatientId, &pParam->m_tag);
	::PostMessage(g_hWnd, UM_NOTIFY_BINDING_PATIENT_RET, ret, (LPARAM)p );
	return 0;
}

int   CBusiness::DeleteTagAsyn(const TagItem * pItem, HWND hWnd) {
	g_thrd_db->PostMessage(this, MSG_DELETE_TAG, new CDeleteTagParam(pItem, hWnd));
	return 0;
}

int   CBusiness::DeleteTag(const CDeleteTagParam * pParam) {
	int ret = m_Database.DeleteTag(pParam);
	NotifyUiDeleteTagRet(ret, pParam);
	return 0;
}

int   CBusiness::NotifyUiDeleteTagRet(int ret, const CDeleteTagParam * pParam) {
	CDeleteTagParam * p = new CDeleteTagParam(&pParam->m_tag, pParam->m_hWnd);
	::PostMessage(pParam->m_hWnd, UM_NOTIFY_DELETE_TAG_RET, ret, (LPARAM)p);
	return 0;
}

int   CBusiness::CheckCardBindingAsyn(const TagItem * pItem) {
	g_thrd_db->PostMessage(this, MSG_CHECK_CARD_BINDING, new CTagItemParam(pItem));
	return 0;
}

int   CBusiness::CheckCardBinding(const CTagItemParam * pParam) {
	DWORD dwNurseId = 0;
	int ret = m_Database.CheckCardBinding(pParam, dwNurseId);
	NotifyUiCheckCardBindingRet(dwNurseId, &pParam->m_tag);
	return 0;
}

int   CBusiness::NotifyUiCheckCardBindingRet(DWORD dwNurseId, const TagItem * pItem) {
	TagItem * pNewItem = new TagItem;
	if (0 == pNewItem) {
		::PostMessage(g_hWnd, UM_CHECK_CARD_BINDING_RET, -1, 0);
	}
	else {
		memcpy(pNewItem, pItem, sizeof(TagItem));
		::PostMessage(g_hWnd, UM_CHECK_CARD_BINDING_RET, dwNurseId, (LPARAM)pNewItem);
	}
	return 0;
}


int    CBusiness::BindingNurseAsyn(DWORD dwNurseId, const TagItem * pItem) {
	g_thrd_db->PostMessage(this, MSG_BINDING_NURSE, new CBindingNurseParam(dwNurseId, pItem));
	return 0;
}

int    CBusiness::BindingNurse(const CBindingNurseParam * pParam) {
	int ret = m_Database.BindingNurse(pParam);
	NotifyUiBindingNurseRet(ret, pParam);
	return 0;
}

int    CBusiness::NotifyUiBindingNurseRet(int ret, const CBindingNurseParam * pParam) {
	CBindingNurseParam * p = new CBindingNurseParam(pParam->m_dwNurseId, &pParam->m_tag);
	::PostMessage(g_hWnd, UM_NOTIFY_BINDING_NURSE_RET, ret, (LPARAM)p);
	return 0;
}

// 重连同步读卡器
int   CBusiness::ReconnectSyncReaderAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_sync_reader->PostMessage(this, MSG_RECONNECT_SYNC_READER);
	}
	else {
		g_thrd_sync_reader->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_SYNC_READER);
	}
	return 0;
}

int   CBusiness::ReconnectSyncReader() {
	int ret = m_SyncReader.Reconnect();
	return ret;
}

// 通知界面连接结果
int   CBusiness::NotifyUiSyncReaderStatus(CZsSyncReader::SYNC_READER_STATUS eStatus) {
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
		CPatientParam * pParam = (CPatientParam *)pMessageData;
		AddPatient(pParam);
	}
	break;

	case MSG_GET_ALL_PATIENTS:
	{
		GetAllPatients();
	}
	break;

	case MSG_MODIFY_PATIENT:
	{
		CPatientParam * pParam = (CPatientParam *)pMessageData;
		ModifyPatient(pParam);
	}
	break;

	case MSG_DELETE_PATIENT:
	{
		CDeletePatientParam * pParam = (CDeletePatientParam *)pMessageData;
		DeletePatient(pParam);
	}
	break;

	case MSG_IMPORT_PATIENTS:
	{
		CImportPatientsParam * pParam = (CImportPatientsParam *)pMessageData;
		std::vector<PatientInfo*> * pvRet = new std::vector<PatientInfo*>;
		int ret = 0;
		if (pvRet) {
			ret = ImportPatients(pParam, *pvRet);
		}
		else {
			ret = -1;
		}

		NotifyUiImportPatientsRet(ret, pvRet);
		
	}
	break;

	case MSG_ADD_NURSE:
	{
		CNurseParam * pParam = (CNurseParam *)pMessageData;
		AddNurse(pParam);
	}
	break;

	case MSG_GET_ALL_NURSES:
	{
		GetAllNurses();
	}
	break;

	case MSG_MODIFY_NURSE:
	{
		CNurseParam * pParam = (CNurseParam *)pMessageData;
		ModifyNurse( pParam );
	}
	break;

	case MSG_DELETE_NURSE:
	{
		CDeleteNurseParam * pParam = (CDeleteNurseParam *)pMessageData;
		DeleteNurse(pParam);
	}
	break;

	case MSG_IMPORT_NURSES:
	{
		CImportNursesParam * pParam = (CImportNursesParam *)pMessageData;
		std::vector<NurseInfo*> * pvRet = new std::vector<NurseInfo*>;
		int ret = 0;
		if (pvRet) {
			ret = ImportNurses(pParam, *pvRet);
		}
		else {
			ret = -1;
		}

		NotifyUiImportNursesRet(ret, pvRet);
	}
	break;

	case MSG_CHECK_TAG_BINDING:
	{
		CTagItemParam * pParam = (CTagItemParam *)pMessageData;
		CheckTagBinding( pParam );
	}
	break;

	case MSG_BINDING_PATIENT:
	{
		CBindingPatientParam * pParam = (CBindingPatientParam *)pMessageData;
		BindingPatient(pParam );
	}
	break;

	case MSG_DELETE_TAG:
	{
		CDeleteTagParam * pParam = (CDeleteTagParam *)pMessageData;
		DeleteTag(pParam);
	}
	break;

	case MSG_CHECK_CARD_BINDING:
	{
		CTagItemParam * pParam = (CTagItemParam *)pMessageData;
		CheckCardBinding(pParam);
	}
	break;

	case MSG_BINDING_NURSE:
	{
		CBindingNurseParam * pParam = (CBindingNurseParam *)pMessageData;
		BindingNurse(pParam);
	}
	break;

	case MSG_RECONNECT_SYNC_READER:
	{
		ReconnectSyncReader();
	}
	break;

	default:
	{

	}
	break;

	}
}



