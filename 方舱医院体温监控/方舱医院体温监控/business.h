#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "MyDatabase.h"
#include "Launch.h"
#include "LmnExcel.h"


class CDuiFrameWnd;
class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();
	void InitSigslot(CDuiFrameWnd * pMainWnd);

	// ��ӡ״̬(������)
	void   PrintStatusAsyn();
	void   PrintStatus();

	// Ӳ���Ķ���������������״̬
	void   CheckLaunchAsyn();
	void   CheckLaunch();

	// �������ӽ�����(�����øĶ���grid count�仯)
	void   RestartLaunchAsyn();
	void   RestartLaunch();

	// ��ȡ�¶�
	void   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	void   GetGridTemperature(const CGetGridTempParam * pParam);
	void   GetGridTemperature(DWORD dwIndex, DWORD dwSubIndex, BYTE byArea, DWORD  dwOldMode );

	// launch ����������
	void   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	void   ReadLaunch();

	// ��������صĻص�
	void  OnStatus(CLmnSerialPort::PortStatus e);
	void  OnReaderTemp(WORD wBed, const TempItem & item);
	void  OnHandReaderTemp(const TempItem & item);

	// �����¶�����
	void  SaveSurTempAsyn( WORD wBedNo, const TempItem & item);
	void  SaveSurTemp(CSaveSurTempParam * pParam);

	// �����ֳ��¶�����
	void  SaveHandeTempAsyn(const TempItem & item);
	void  SaveHandTemp(CSaveHandTempParam * pParam);

	// ������ʷ�¶�����
	void  QueryTempByTagAsyn(const char * szTagId, WORD wBedNo);
	void  QueryTempByTag(const CQueryTempByTagParam * pParam);

	// ����ע��
	void  SaveRemarkAsyn(DWORD  dwDbId, const char * szRemark);
	void  SaveRemark(const CSaveRemarkParam * pParam);

	// ��ʱ���tag patient name��û�й���(��sqlite�߳��� )
	void  CheckSqliteAsyn();
	void  CheckSqlite();

	// ������������TagID����λ��
	void  SaveLastSurTagIdAsyn(WORD wBedId, const char * szTagId);
	void  SaveLastSurTagId(const CSaveLastSurTagId * pParam);

	// ������е�ʱ���ȴ����ݿ��ȡ��һ�ε�������Ϣ
	void  PrepareAsyn();
	void  Prepare();	

	// Tag�󶨴���
	void  TagBindingGridAsyn(const char * szTagId, int nGridIndex);
	void  TagBindingGrid(const CBindingTagGrid * pParam);
	// ��ȡ�󶨴���
	void  QueryBindingByTagIdAsyn(const char * szTagId);
	void  QueryBindingByTagId(const CQueryBindingByTag * pParam);

	// ����Hand Tag��ʷ�¶�����
	void  QueryTempByHandTagAsyn(const char * szTagId, int nGridIndex);
	void  QueryTempByHandTag(const CQueryTempByHandTagParam * pParam);

	// ɾ�����ӵ��ֳ�Tag��
	void  RemoveGridBindingAsyn(int nGridIndex);
	void  RemoveGridBinding(const CRemoveGridBindingParam * pParam);

	// ����tag��patient name
	void  SaveTagPNameAsyn(const char * szTagId, const char * szPName);
	void  SaveTagPName(const CSaveTagPNameParam * pParam);

	// ��ȡtag��patient id
	char *  GetTagPName(const char * szTagId, char * szPName, DWORD dwPNameLen);

	// ������
	void  AlarmAsyn();
	void  Alarm();

	// ��ȡ���˵Ļ�����Ϣ
	void  QueryPatientInfoAsyn(const char * szTagId);
	void  QueryPatientInfo(const CQueryPatientInfoParam * pParam);
	// ���������Ϣ
	void  SavePatientInfoAsyn(const PatientInfo * pInfo);
	void  SavePatientInfo(const CSavePatientInfoParam * pParam);
	// ���没���¼���Ϣ
	void  SavePatientEventsAsyn( const char * szTagId, const std::vector<PatientEvent*> & vEvents );
	void  SavePatientEvents(const CSavePatientEventsParam * pParam);

	// ��ȡĳ7��Ĳ��˵ķ�����������Ϣ
	void  QueryPatientDataAsyn(const char * szTagId, time_t tFirstDay);
	void  QueryPatientData(const CQueryPatientDataParam * pParam);
	// ���没�˷��¶�����
	void  SavePatientDataAsyn(const PatientData * pData);
	void  SavePatientData(const CSavePatientDataParam * pParam);

	// ɾ��Tag����
	void  DelTagAsyn(const char * szTagId);
	void  DelTag(const CDelTag * pParam);

	// ��ѯסԺ��Ϣ
	void  QueryInHospitalAsyn(const TQueryInHospital * pQuery);
	void  QueryInHospital(const CQueryInHospital * pParam);

	// ��ѯ��Ժ��Ϣ
	void  QueryOutHospitalAsyn(const TQueryOutHospital * pQuery);
	void  QueryOutHospital(const CQueryOutHospital * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();
	// ����˳������
	void GetGridsOrderCfg();
	// ��������
	void GetAreaCfg();
	// ��λ������
	void GetGridsCfg();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase             m_sqlite;
	CLaunch                       m_launch;
	sigslot::signal1<CLmnSerialPort::PortStatus>    m_sigLanchStatus;
	sigslot::signal2<WORD,BOOL>                     m_sigTrySurReader;         // param: bed no, is try read
	sigslot::signal2<WORD,BOOL>                     m_sigSurReaderStatus;      // param: bed no, connected
	sigslot::signal2<WORD, const TempItem &>        m_sigSurReaderTemp;        // param: bed no, temperature
	sigslot::signal3<const char *, WORD, std::vector<TempItem*> *> 
		                                            m_sigQueyTemp;             // param: tag id, bed no, vector result
	sigslot::signal2<const TempItem &, const char *>   m_sigHandReaderTemp;    // param: temperature, tag patient name
	sigslot::signal1<std::vector<HandTagResult *> *>   m_sigAllHandTagTempData;  
	sigslot::signal0<>                              m_prepared;
	sigslot::signal1<const TagBindingGridRet &>     m_sigBindingRet;
	sigslot::signal3<const char *, int, std::vector<TempItem*> *>
		                                            m_sigQueyHandTemp;         // param: tag id, grid index, vector result
	sigslot::signal1<const TagBindingGridRet &>     m_sigQueryBindingByTag;
	sigslot::signal1<const char *>                  m_sigDelTag;
	sigslot::signal1<const std::vector<InHospitalItem * > & >   m_sigInHospitalRet;
	sigslot::signal1<const std::vector<OutHospitalItem * > & >  m_sigOutHospitalRet;

	// ���ж������Ƿ�õ��¶�����
	BOOL                          m_bSurReaderTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];	

	LmnLockType                                     m_lock;
	std::map<std::string, TagPName*>                m_tag_patient_name;        // tag��Ӧ�Ĳ������� 

private:
	/********* �Զ�����excel��� *********/
	BOOL              m_bExcelSupport;
	CExcelEx *        m_excel;
	int               m_excel_row[MAX_GRID_COUNT][MAX_READERS_PER_GRID+1];
	char              m_excel_tag_id[MAX_GRID_COUNT][MAX_READERS_PER_GRID+1][MAX_TAG_ID_LENGTH];
	char              m_excel_patient_name[MAX_GRID_COUNT][MAX_READERS_PER_GRID + 1][MAX_TAG_PNAME_LENGTH];


public:
	// ����excel
	void  SaveExcelAsyn();
	void  SaveExcel();

	// д�¶����ݵ�excel( i: grid index, j: 0 hand,1~6 surgery )
	void  WriteTemp2ExcelAsyn(DWORD i, DWORD  j, const TempItem * pTemp, const char * szPName);
	void  WriteTemp2Excel(const CWriteTemp2ExcelParam * pParam);

	// ��ʼ��excel�߳�(CoInitialize)
	void  InitThreadExcelAsyn();
	void  InitThreadExcel();

public:
	sigslot::signal2<PatientInfo *, const std::vector<PatientEvent * > & >   m_sigPatientInfo;
	sigslot::signal3<PatientData *, DWORD, const std::vector<TempItem *> &>  m_sigPatientData;
};





