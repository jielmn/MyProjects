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

	// timer�߳�
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

	// �������ݿ�
	ReconnectDatabaseAsyn(200);

	// �̵�
	InventoryAsyn(200);

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

// �������ݿ�(�첽)
int   CBusiness::ReconnectDatabaseAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_db->PostMessage(this, MSG_RECONNECT_DB);
	}
	else {
		g_thrd_db->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_DB);
	}
	return 0;
}

// �������ݿ�
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


// �̵�
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

	// ֪ͨUI����
	if (eOldStatus != eNewStatus) {
		NotifyUiBindingReaderStatus(eNewStatus);
	}

	// ����ǳɹ����̵㣬�����´��̵�ʱ�����㹻����ʱ��
	DWORD  dwRelayTime = 1000;
	if (0 == ret) {
		dwRelayTime = 2000;
	}

	// �ٴ��̵�
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

// ֪ͨ��ӽ��
int   CBusiness::NotifyUiAddPatientRet(HWND hWnd, int ret, DWORD dwId) {
	::PostMessage( hWnd, UM_ADD_PATIENT_RET, ret, dwId );
	return 0;
}

// ��ȡ���еĲ�����Ϣ 
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

// �޸Ĳ���
int   CBusiness::ModifyPatientAsyn(const PatientInfo * pPatient, HWND hWnd) {
	g_thrd_db->PostMessage(this, MSG_MODIFY_PATIENT, new CPatientParam(pPatient, hWnd));
	return 0;
}

int   CBusiness::ModifyPatient(const CPatientParam * pParam) {
	int ret = m_Database.ModifyPatient(pParam);
	NotifyUiModifyPatientRet(pParam->m_hWnd, ret);
	return 0;
}

// ֪ͨ��ӽ��
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

// ֪ͨɾ�����
int   CBusiness::NotifyUiDeletePatientRet(int ret , DWORD dwId) {
	::PostMessage(g_hWnd, UM_DELETE_PATIENT_RET, ret, dwId);
	return 0;
}

// ���벡��
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
		sSql = "SELECT * from [������Ϣ$]";
		recset.Open(CRecordset::forwardOnly,sSql,CRecordset::readOnly);

		//int n = recset.GetODBCFieldCount();
		//get query results  
		//BOOL bFailed = FALSE;

		while (!recset.IsEOF())
		{
			PatientInfo tInfo;
			memset(&tInfo, 0, sizeof(PatientInfo));

			//read inside value  
			recset.GetFieldValue("���",     sItem);
			strncpy_s(tInfo.szId, sItem, sizeof(tInfo.szId));

			recset.GetFieldValue("����",     sItem);
			strncpy_s(tInfo.szName, sItem, sizeof(tInfo.szName));

			recset.GetFieldValue("�Ա�",     sItem);
			if (0 == strcmp(sItem, "Ů")) {
				tInfo.bFemale = TRUE;
			}
			else {
				tInfo.bFemale = FALSE;
			}

			recset.GetFieldValue("����",     sItem);
			strncpy_s(tInfo.szBedNo, sItem, sizeof(tInfo.szBedNo));

			recset.GetFieldValue("״̬", sItem);
			if (0 == strcmp(sItem, "��Ժ")) {
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

// ֪ͨ������
int   CBusiness::NotifyUiImportPatientsRet(int ret, std::vector<PatientInfo*> * pvRet ) {
	if ( (0 != ret) && (ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL != ret) ) {
		ClearVector(*pvRet);
		delete pvRet;
		pvRet = 0;
	}

	::PostMessage(g_hWnd, UM_NOTIFY_IMPORT_PATIENTS_RET, ret, (LPARAM)pvRet);
	return 0;
}


// ��Ϣ����
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

	default:
	{

	}
	break;

	}
}



