#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "sigslot.h"
#include "zsCommon.h"
#include "zsDatabase.h"
#include "zsBindingReader.h"
#include "zsSyncReader.h"

class CBusiness : public sigslot::has_slots<>, public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	sigslot::signal1< int *>                               sigInit;
	sigslot::signal1< int *>                               sigDeinit;

	static CBusiness *  GetInstance();


	// �������ݿ�(�첽��ֻ������Ϣ��db�߳�)
	int   ReconnectDatabaseAsyn(DWORD dwDelayTime = 0);
	int   ReconnectDatabase();
	// ֪ͨ�������ӽ��
	int   NotifyUiDbStatus(CZsDatabase::DATABASE_STATUS eStatus);
	// ��ȡ״̬
	CZsDatabase::DATABASE_STATUS  GetDbStatus();

	// �̵�
	int   InventoryAsyn(DWORD dwDelayTime = 0);
	int   Inventory();
	// ֪ͨ����BindingReader����
	int   NotifyUiBindingReaderStatus(CZsBindingReader::BINDING_STATUS eStatus);
	// ��ȡ״̬
	CZsBindingReader::BINDING_STATUS  GetBindingReaderStatus();

	// ��Ӳ���
	int   AddPatientAsyn( const PatientInfo * pPatient, HWND hWnd );
	int   AddPatient(const CPatientParam * pParam);
	// ֪ͨ��ӽ��
	int   NotifyUiAddPatientRet(HWND hWnd, int ret, DWORD dwId);

	// �������еĲ�����Ϣ
	int   GetAllPatientsAsyn();
	int   GetAllPatients();
	int   NotifyUiGetAllPatientsRet(int ret, std::vector<PatientInfo * > * pvRet);

	// �޸Ĳ���
	int   ModifyPatientAsyn(const PatientInfo * pPatient, HWND hWnd);
	int   ModifyPatient(const CPatientParam * pParam);
	// ֪ͨ�޸Ľ��
	int   NotifyUiModifyPatientRet(HWND hWnd, int ret);

	// ɾ������
	int   DeletePatientAsyn(DWORD dwId);
	int   DeletePatient(const CDeletePatientParam * pParam);
	// ֪ͨɾ�����
	int   NotifyUiDeletePatientRet(int ret, DWORD dwId);

	// ���벡��
	int   ImportPatientsAsyn( const char * szFilePath );
	int   ImportPatients(const CImportPatientsParam * pParam, std::vector<PatientInfo*> & vRet );
	// ֪ͨ������
	int   NotifyUiImportPatientsRet(int ret, std::vector<PatientInfo*> * );

	// ��ӻ�ʿ
	int   AddNurseAsyn(const NurseInfo * pNurse, HWND hWnd);
	int   AddNurse(const CNurseParam * pParam);
	// ֪ͨ��ӽ��
	int   NotifyUiAddNurseRet(HWND hWnd, int ret, DWORD dwId);

	// �������еĻ�ʿ��Ϣ
	int   GetAllNursesAsyn();
	int   GetAllNurses();
	int   NotifyUiGetAllNursesRet(int ret, std::vector<NurseInfo * > * pvRet);

	// �޸Ļ�ʿ
	int   ModifyNurseAsyn(const NurseInfo * pNurse, HWND hWnd);
	int   ModifyNurse(const CNurseParam * pParam);
	// ֪ͨ�޸Ľ��
	int   NotifyUiModifyNurseRet(HWND hWnd, int ret);

	// ɾ����ʿ
	int   DeleteNurseAsyn(DWORD dwId);
	int   DeleteNurse(const CDeleteNurseParam * pParam);
	// ֪ͨɾ�����
	int   NotifyUiDeleteNurseRet(int ret, DWORD dwId);

	// ���뻤ʿ
	int   ImportNursesAsyn(const char * szFilePath);
	int   ImportNurses(const CImportNursesParam * pParam, std::vector<NurseInfo*> & vRet);
	// ֪ͨ������
	int   NotifyUiImportNursesRet(int ret, std::vector<NurseInfo*> *);

	// ���Tag���ް�
	int   CheckTagBindingAsyn(const TagItem * pItem);
	int   CheckTagBinding( const CTagItemParam * pParam );
	int   NotifyUiCheckTagBindingRet(DWORD dwPatientId, const TagItem * pItem);

	// ��
	int   BindingPatientAsyn(DWORD dwPatientId, const TagItem * pItem);
	int   BindingPatient(const CBindingPatientParam * pParam);
	int   NotifyUiBindingPatientRet(int ret, const CBindingPatientParam * pParam);

	// ɾ��Tag
	int   DeleteTagAsyn(const TagItem * pItem, HWND hWnd );
	int   DeleteTag(const CDeleteTagParam * pParam);
	int   NotifyUiDeleteTagRet(int ret, const CDeleteTagParam * pParam);

	// ���Tag���ް󶨻�ʿ
	int   CheckCardBindingAsyn(const TagItem * pItem);
	int   CheckCardBinding(const CTagItemParam * pParam);
	int   NotifyUiCheckCardBindingRet(DWORD dwNurseId, const TagItem * pItem);

	// �󶨻�ʿ�׿�
	int   BindingNurseAsyn(DWORD dwNurseId, const TagItem * pItem);
	int   BindingNurse(const CBindingNurseParam * pParam);
	int   NotifyUiBindingNurseRet(int ret, const CBindingNurseParam * pParam);

	// ����ͬ��������
	int   ReconnectSyncReaderAsyn(DWORD dwDelayTime = 0);
	int   ReconnectSyncReader();
	// ֪ͨ�������ӽ��
	int   NotifyUiSyncReaderStatus(CZsSyncReader::SYNC_READER_STATUS eStatus);

private:
	static CBusiness *  pInstance;

	void OnInit(int * ret);
	void OnDeInit(int * ret);

	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	CZsDatabase        m_Database;                           // ���ݿ�
	CZsBindingReader   m_BindingReader;                      // �󶨶�����
	CZsSyncReader      m_SyncReader;                         // ͬ��������
	
};



