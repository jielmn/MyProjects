#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "MyDatabase.h"
#include "Launch.h"



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

	// ���ж������Ƿ�õ��¶�����
	BOOL                          m_bSurReaderTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];	

	std::map<std::string, TagPName*>                m_tag_patient_name;        // tag��Ӧ�Ĳ������� 
};





