#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "Launch.h"
#include "LmnExcel.h"
#include "MyDatabase.h"

class  CMyDb {
public:
	CMyDb();
	~CMyDb();

	int  Reconnect();
	int  GetStatus();
	int  QueryBinding(const CQueryBindingParam * pParam, TagBinding * tRet);
	int  DbHeartBeat();
	int  SaveTemp(const CSaveTempParam * pParam);
	int  GetAllPatients( std::vector<Patient *> & vRet, DWORD dwPatiendId = 0 );
	int  SetBinding(const CBindTagsParam * pParam);

	sigslot::signal1<int>     m_sigStatus;

private:
	typedef struct tagTempItem {
		DWORD   m_dwTemp;                      // �¶�
		time_t  m_Time;                        // ʱ��
		char    m_szTagId[20];                 // tag id
		DWORD   m_dwIndex;
		DWORD   m_dwSubIndex;
	}TempItem;

	MYSQL   m_mysql;
	int     m_nStatus;    //0: �ر�;    1: ��
	std::vector<TempItem *>   m_vTemps;

	int SaveTemp(const TempItem * pTemp);
	void BuffTemp(const CSaveTempParam * pParam);
};

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	// ����������
	int    UpdateScrollAsyn(int nIndex, DWORD dwDelay = 0);
	int    UpdateScroll(const CUpdateScrollParam * pParam);
	void   OnUpdateScroll(DWORD dwIndex);

	// ����
	int   AlarmAsyn();
	int   Alarm();
	void  OnAlarm();

	// ����������
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();
	void  OnReconnect(DWORD dwDelay);
	void  OnStatus(CLmnSerialPort::PortStatus e);

	// ��ӡ״̬
	int   PrintStatusAsyn();
	int   PrintStatus();

	// ��ȡ�¶�
	int   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	int   GetGridTemperature(const CGridTempParam * pParam);
	int   GetTemperatureAsyn(DWORD  dwIndex, DWORD dwSubIndex, DWORD dwDelay = 0);
	int   GetTemperature(CTemperatureParam * pParam);	

	// launch ����������
	int   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReadLaunch();
	void  OnReaderTemp(DWORD dwIndex, DWORD dwSubIndex, const LastTemp & t);
	void  OnCheckReader();

	// Ӳ���Ķ������״̬
	int   CheckLaunchStatusAsyn();
	int   CheckLaunchStatus();

	// �������ݿ�
	int   ReconnectDbAsyn(DWORD dwDelay = 0);
	int   ReconnectDb();
	// 0: �ر�;  1: ��
	void  OnDbStatus(int nDbStatus);
	int   GetDbStatus();

	// �������������״̬
	int   DbHeartBeatAsyn(DWORD dwDelay = 0);
	int   DbHeartBeat();

	// ��ѯ�󶨹�ϵ
	int   QueryBindingAsyn(DWORD dwIndex, DWORD dwSubIndex, const char * szTagId);
	int   QueryBinding(const CQueryBindingParam * pParam);

	// �����¶����ݵ����ݿ�
	int   SaveTempAsyn(DWORD dwIndex, DWORD dwSubIndex, const LastTemp * pTemp );
	int   SaveTemp(const CSaveTempParam * pParam);

	// ��ѯ���в�����Ϣ
	int   GetAllPatientsAsyn(HWND  hWnd, DWORD  dwPatientId = 0);
	int   GetAllPatients(const CGetPatientsParam * pParam);

	// ��tag
	int   SetBindingAsyn(HWND hWnd, DWORD dwPatientId, 
		                const TagBinding_1 * pTags, DWORD dwTagsCnt );
	int   SetBinding(const CBindTagsParam * pParam);

	// ����excel
	int  SaveExcelAsyn();

	// 
	int  ExcelTempAsyn(DWORD dwIndex, DWORD  dwSubIndex, const LastTemp * pTemp );
	//
	int  ExcelPatientNameChangedAsyn(DWORD dwIndex);

	//   �����¶����ݵ�sqlite
	int  SaveTemp2SqliteAsyn(const char * szTagId, DWORD dwTemp);
	int  SaveTemp2Sqlite(const CSaveTempSqliteParam * pParam);

	//  ��ѯsqlite��¼��ĳ��tag���¶ȼ�¼
	int   QueryTempFromSqliteByTagAsyn(const char * szTagId, DWORD  dwIndex, DWORD  dwSubIndex);
	int   QueryTempFromSqliteByTag(const CQueryTempSqliteParam * pParam);
	
	// ����ע��
	int   SetRemarkAsyn(const char * szTagId, time_t tTime, const char * szRemark);
	int   SetRemark(const CSetRemarkSqliteParam * pParam);

	// �����ֳ�Reader����ʷ�¶�����
	int   QueryHandReaderTempFromSqliteAsyn();
	int   QueryHandReaderTempFromSqlite();

private:
	static CBusiness *  pInstance;
	void Clear();
	void  CheckGrid(DWORD dwIndex);

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char              m_szAlarmFile[256];
	CLaunch           m_launch;
	ReaderStatus      m_reader_status[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CMyDb             m_db;

	BOOL              m_bExcelSupport;
	CExcelEx *        m_excel;
	int               m_excel_row[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	char              m_excel_tag_id[MAX_GRID_COUNT][MAX_READERS_PER_GRID][20];

	CMySqliteDatabase   m_sqlite;
};





