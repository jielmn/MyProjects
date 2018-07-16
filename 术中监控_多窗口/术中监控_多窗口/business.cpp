#include "business.h"
#include "UIlib.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {

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

int CBusiness::Init() {
	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init(LOG_FILE_NAME);

	g_cfg = new FileConfigEx();
	if (0 == g_cfg) {
		return -1;
	}
	g_cfg->Init(CONFIG_FILE_NAME);

	g_thrd_db = new LmnToolkits::Thread();
	if (0 == g_thrd_db) {
		return -1;
	}
	g_thrd_db->Start();

	DuiLib::CDuiString  strText;
	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("low alarm %d", i + 1);
		g_cfg->GetConfig(strText, g_dwLowTempAlarm[i],3500);

		strText.Format("high alarm %d", i + 1);
		g_cfg->GetConfig(strText, g_dwHighTempAlarm[i], 4200);

		strText.Format("collect interval %d", i + 1);
		g_cfg->GetConfig(strText, g_dwCollectInterval[i], 10);

		strText.Format("min temperature degree %d", i + 1);
		g_cfg->GetConfig(strText, g_dwMinTemp[i], 24);

		strText.Format("com port %d", i + 1);
		g_cfg->GetConfig(strText, g_szComPort[i], 32, "");
	}

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_db) {
		g_thrd_db->Stop();
		delete g_thrd_db;
		g_thrd_db = 0;
	}

	Clear();
	return 0;
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {

}