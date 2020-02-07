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

	// 打印状态(调试用)
	void   PrintStatusAsyn();
	void   PrintStatus();

	// 硬件改动，检查接收器串口状态
	void   CheckLaunchAsyn();
	void   CheckLaunch();

	// 重新连接接收器(因配置改动，grid count变化)
	void   RestartLaunchAsyn();
	void   RestartLaunch();

	// 获取温度
	void   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	void   GetGridTemperature(const CGetGridTempParam * pParam);
	void   GetGridTemperature(DWORD dwIndex, DWORD dwSubIndex, BYTE byArea, DWORD  dwOldMode );

	// launch 读串口数据
	void   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	void   ReadLaunch();

	// 发射器相关的回调
	void  OnStatus(CLmnSerialPort::PortStatus e);
	void  OnReaderTemp(WORD wBed, const TempItem & item);
	void  OnHandReaderTemp(const TempItem & item);

	// 保存温度数据
	void  SaveSurTempAsyn( WORD wBedNo, const TempItem & item);
	void  SaveSurTemp(CSaveSurTempParam * pParam);

	// 保存手持温度数据
	void  SaveHandeTempAsyn(const TempItem & item);
	void  SaveHandTemp(CSaveHandTempParam * pParam);

	// 请求历史温度数据
	void  QueryTempByTagAsyn(const char * szTagId, WORD wBedNo);
	void  QueryTempByTag(const CQueryTempByTagParam * pParam);

	// 保存注释
	void  SaveRemarkAsyn(DWORD  dwDbId, const char * szRemark);
	void  SaveRemark(const CSaveRemarkParam * pParam);

	// 定时检查tag patient name有没有过期(在sqlite线程里 )
	void  CheckSqliteAsyn();
	void  CheckSqlite();

	// 保存最后的术中TagID到床位号
	void  SaveLastSurTagIdAsyn(WORD wBedId, const char * szTagId);
	void  SaveLastSurTagId(const CSaveLastSurTagId * pParam);

	// 软件运行的时候，先从数据库获取上一次的有用信息
	void  PrepareAsyn();
	void  Prepare();	

	// Tag绑定窗格
	void  TagBindingGridAsyn(const char * szTagId, int nGridIndex);
	void  TagBindingGrid(const CBindingTagGrid * pParam);
	// 获取绑定窗格
	void  QueryBindingByTagIdAsyn(const char * szTagId);
	void  QueryBindingByTagId(const CQueryBindingByTag * pParam);

	// 请求Hand Tag历史温度数据
	void  QueryTempByHandTagAsyn(const char * szTagId, int nGridIndex);
	void  QueryTempByHandTag(const CQueryTempByHandTagParam * pParam);

	// 删除格子的手持Tag绑定
	void  RemoveGridBindingAsyn(int nGridIndex);
	void  RemoveGridBinding(const CRemoveGridBindingParam * pParam);

	// 保存tag的patient name
	void  SaveTagPNameAsyn(const char * szTagId, const char * szPName);
	void  SaveTagPName(const CSaveTagPNameParam * pParam);

	// 获取tag的patient id
	char *  GetTagPName(const char * szTagId, char * szPName, DWORD dwPNameLen);

	// 报警声
	void  AlarmAsyn();
	void  Alarm();

	// 获取病人的基础信息
	void  QueryPatientInfoAsyn(const char * szTagId);
	void  QueryPatientInfo(const CQueryPatientInfoParam * pParam);
	// 保存基本信息
	void  SavePatientInfoAsyn(const PatientInfo * pInfo);
	void  SavePatientInfo(const CSavePatientInfoParam * pParam);
	// 保存病人事件信息
	void  SavePatientEventsAsyn( const char * szTagId, const std::vector<PatientEvent*> & vEvents );
	void  SavePatientEvents(const CSavePatientEventsParam * pParam);

	// 获取某7天的病人的非体温数据信息
	void  QueryPatientDataAsyn(const char * szTagId, time_t tFirstDay);
	void  QueryPatientData(const CQueryPatientDataParam * pParam);
	// 保存病人非温度数据
	void  SavePatientDataAsyn(const PatientData * pData);
	void  SavePatientData(const CSavePatientDataParam * pParam);

	// 删除Tag数据
	void  DelTagAsyn(const char * szTagId);
	void  DelTag(const CDelTag * pParam);

	// 查询住院信息
	void  QueryInHospitalAsyn(const TQueryInHospital * pQuery);
	void  QueryInHospital(const CQueryInHospital * pParam);

	// 查询出院信息
	void  QueryOutHospitalAsyn(const TQueryOutHospital * pQuery);
	void  QueryOutHospital(const CQueryOutHospital * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();
	// 窗格顺序配置
	void GetGridsOrderCfg();
	// 病区配置
	void GetAreaCfg();
	// 床位的配置
	void GetGridsCfg();

	// 消息处理
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

	// 术中读卡器是否得到温度数据
	BOOL                          m_bSurReaderTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];	

	LmnLockType                                     m_lock;
	std::map<std::string, TagPName*>                m_tag_patient_name;        // tag对应的病人名称 

private:
	/********* 自动保存excel相关 *********/
	BOOL              m_bExcelSupport;
	CExcelEx *        m_excel;
	int               m_excel_row[MAX_GRID_COUNT][MAX_READERS_PER_GRID+1];
	char              m_excel_tag_id[MAX_GRID_COUNT][MAX_READERS_PER_GRID+1][MAX_TAG_ID_LENGTH];
	char              m_excel_patient_name[MAX_GRID_COUNT][MAX_READERS_PER_GRID + 1][MAX_TAG_PNAME_LENGTH];


public:
	// 保存excel
	void  SaveExcelAsyn();
	void  SaveExcel();

	// 写温度数据到excel( i: grid index, j: 0 hand,1~6 surgery )
	void  WriteTemp2ExcelAsyn(DWORD i, DWORD  j, const TempItem * pTemp, const char * szPName);
	void  WriteTemp2Excel(const CWriteTemp2ExcelParam * pParam);

	// 初始化excel线程(CoInitialize)
	void  InitThreadExcelAsyn();
	void  InitThreadExcel();

public:
	sigslot::signal2<PatientInfo *, const std::vector<PatientEvent * > & >   m_sigPatientInfo;
	sigslot::signal3<PatientData *, DWORD, const std::vector<TempItem *> &>  m_sigPatientData;
};





