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
};





