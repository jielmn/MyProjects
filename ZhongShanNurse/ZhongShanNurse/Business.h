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


	// 重连数据库(异步，只发送消息到db线程)
	int   ReconnectDatabaseAsyn(DWORD dwDelayTime = 0);
	int   ReconnectDatabase();
	// 通知界面连接结果
	int   NotifyUiDbStatus(CZsDatabase::DATABASE_STATUS eStatus);
	// 获取状态
	CZsDatabase::DATABASE_STATUS  GetDbStatus();

	// 盘点
	int   InventoryAsyn(DWORD dwDelayTime = 0);
	int   Inventory();
	// 通知界面BindingReader连接
	int   NotifyUiBindingReaderStatus(CZsBindingReader::BINDING_STATUS eStatus);
	// 获取状态
	CZsBindingReader::BINDING_STATUS  GetBindingReaderStatus();

	// 添加病人
	int   AddPatientAsyn( const PatientInfo * pPatient, HWND hWnd );
	int   AddPatient(const CPatientParam * pParam);
	// 通知添加结果
	int   NotifyUiAddPatientRet(HWND hWnd, int ret, DWORD dwId);

	// 请求所有的病人信息
	int   GetAllPatientsAsyn();
	int   GetAllPatients();
	int   NotifyUiGetAllPatientsRet(int ret, std::vector<PatientInfo * > * pvRet);

	// 修改病人
	int   ModifyPatientAsyn(const PatientInfo * pPatient, HWND hWnd);
	int   ModifyPatient(const CPatientParam * pParam);
	// 通知修改结果
	int   NotifyUiModifyPatientRet(HWND hWnd, int ret);

	// 删除病人
	int   DeletePatientAsyn(DWORD dwId);
	int   DeletePatient(const CDeletePatientParam * pParam);
	// 通知删除结果
	int   NotifyUiDeletePatientRet(int ret, DWORD dwId);

	// 导入病人
	int   ImportPatientsAsyn( const char * szFilePath );
	int   ImportPatients(const CImportPatientsParam * pParam, std::vector<PatientInfo*> & vRet );
	// 通知导入结果
	int   NotifyUiImportPatientsRet(int ret, std::vector<PatientInfo*> * );

	// 添加护士
	int   AddNurseAsyn(const NurseInfo * pNurse, HWND hWnd);
	int   AddNurse(const CNurseParam * pParam);
	// 通知添加结果
	int   NotifyUiAddNurseRet(HWND hWnd, int ret, DWORD dwId);

	// 请求所有的护士信息
	int   GetAllNursesAsyn();
	int   GetAllNurses();
	int   NotifyUiGetAllNursesRet(int ret, std::vector<NurseInfo * > * pvRet);

	// 修改护士
	int   ModifyNurseAsyn(const NurseInfo * pNurse, HWND hWnd);
	int   ModifyNurse(const CNurseParam * pParam);
	// 通知修改结果
	int   NotifyUiModifyNurseRet(HWND hWnd, int ret);

	// 删除护士
	int   DeleteNurseAsyn(DWORD dwId);
	int   DeleteNurse(const CDeleteNurseParam * pParam);
	// 通知删除结果
	int   NotifyUiDeleteNurseRet(int ret, DWORD dwId);

	// 导入护士
	int   ImportNursesAsyn(const char * szFilePath);
	int   ImportNurses(const CImportNursesParam * pParam, std::vector<NurseInfo*> & vRet);
	// 通知导入结果
	int   NotifyUiImportNursesRet(int ret, std::vector<NurseInfo*> *);

	// 检查Tag有无绑定
	int   CheckTagBindingAsyn(const TagItem * pItem);
	int   CheckTagBinding( const CTagItemParam * pParam );
	int   NotifyUiCheckTagBindingRet(DWORD dwPatientId, const TagItem * pItem);

	// 绑定
	int   BindingPatientAsyn(DWORD dwPatientId, const TagItem * pItem);
	int   BindingPatient(const CBindingPatientParam * pParam);
	int   NotifyUiBindingPatientRet(int ret, const CBindingPatientParam * pParam);

	// 删除Tag
	int   DeleteTagAsyn(const TagItem * pItem, HWND hWnd );
	int   DeleteTag(const CDeleteTagParam * pParam);
	int   NotifyUiDeleteTagRet(int ret, const CDeleteTagParam * pParam);

	// 检查Tag有无绑定护士
	int   CheckCardBindingAsyn(const TagItem * pItem);
	int   CheckCardBinding(const CTagItemParam * pParam);
	int   NotifyUiCheckCardBindingRet(DWORD dwNurseId, const TagItem * pItem);

	// 绑定护士白卡
	int   BindingNurseAsyn(DWORD dwNurseId, const TagItem * pItem);
	int   BindingNurse(const CBindingNurseParam * pParam);
	int   NotifyUiBindingNurseRet(int ret, const CBindingNurseParam * pParam);

	// 重连同步读卡器
	int   ReconnectSyncReaderAsyn(DWORD dwDelayTime = 0);
	int   ReconnectSyncReader();
	// 通知界面连接结果
	int   NotifyUiSyncReaderStatus(CZsSyncReader::SYNC_READER_STATUS eStatus);

private:
	static CBusiness *  pInstance;

	void OnInit(int * ret);
	void OnDeInit(int * ret);

	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	CZsDatabase        m_Database;                           // 数据库
	CZsBindingReader   m_BindingReader;                      // 绑定读卡器
	CZsSyncReader      m_SyncReader;                         // 同步读卡器
	
};



