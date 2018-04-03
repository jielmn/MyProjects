#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "sigslot.h"
#include "zsCommon.h"
#include "zsDatabase.h"
#include "zsBindingReader.h"

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
	int   AddPatient(const CAddPatientParam * pParam);
	// ֪ͨ��ӽ��
	int   NotifyUiAddPatientRet(HWND hWnd, int ret, DWORD dwId);

	// �������еĲ�����Ϣ
	int   GetAllPatientsAsyn();
	int   GetAllPatients();
	int   NotifyUiGetAllPatientsRet();

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
};



