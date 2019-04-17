#include "business.h"
#include "main.h"

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
	if (g_data.m_log) {
		g_data.m_log->Deinit();
		delete g_data.m_log;
		g_data.m_log = 0;
	}

	if (g_data.m_cfg) {
		g_data.m_cfg->Deinit();
		delete g_data.m_cfg;
		g_data.m_cfg = 0;
	}
}

int CBusiness::Init() {

	g_data.m_log = new FileLog();
	if (0 == g_data.m_log) {
		return -1;
	}

#ifdef _DEBUG
	g_data.m_log->Init(LOG_FILE_NAME);
#else
	g_data.m_log->Init(LOG_FILE_NAME, 0, ILog::LOG_SEVERITY_INFO, TRUE);
#endif

	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	/*********************    ������    ***********************/
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, DEFAULT_MAIN_LAYOUT_COLUMNS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_ROWS,    g_data.m_CfgData.m_dwLayoutRows,    DEFAULT_MAIN_LAYOUT_ROWS);
	g_data.m_cfg->GetConfig(CFG_MAIN_LAYOUT_GRIDS_COUNT, g_data.m_CfgData.m_dwLayoutGridsCnt, DEFAULT_MAIN_LAYOUT_GRIDS_COUNT);

	// g_data.m_CfgData.m_dwLayoutGridsCnt��Χ��4~60
	if ( g_data.m_CfgData.m_dwLayoutGridsCnt > MAX_GRID_COUNT ) {
		g_data.m_CfgData.m_dwLayoutGridsCnt = MAX_GRID_COUNT;
	}
	else if (g_data.m_CfgData.m_dwLayoutGridsCnt < DEFAULT_MAIN_LAYOUT_GRIDS_COUNT) {
		g_data.m_CfgData.m_dwLayoutGridsCnt = DEFAULT_MAIN_LAYOUT_GRIDS_COUNT;
	}

	// g_data.m_CfgData.m_dwLayoutColumns������ڵ���2��С�ڵ���5
	// g_data.m_CfgData.m_dwLayoutRows������ڵ���2��С�ڵ���5
	if ( g_data.m_CfgData.m_dwLayoutColumns <= 1 || g_data.m_CfgData.m_dwLayoutRows <= 1 ) {
		g_data.m_CfgData.m_dwLayoutColumns = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_data.m_CfgData.m_dwLayoutRows = DEFAULT_MAIN_LAYOUT_ROWS;
	}
	if ( g_data.m_CfgData.m_dwLayoutColumns > MAX_COLUMNS_COUNT) {
		g_data.m_CfgData.m_dwLayoutColumns = MAX_COLUMNS_COUNT;
	}
	if (g_data.m_CfgData.m_dwLayoutRows > MAX_ROWS_COUNT) {
		g_data.m_CfgData.m_dwLayoutRows = MAX_ROWS_COUNT;
	}

	// ����˳��
	GetGridsOrderCfg();
	

	/******************** end ������ **********************/

	g_data.m_thrd_timer = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_timer) {
		return -1;
	}
	g_data.m_thrd_timer->Start(TRUE, 20);

	return 0;
}

void CBusiness::GetGridsOrderCfg() {
	char buf[8192];

	g_data.m_cfg->GetConfig(CFG_GRIDS_ORDER, buf, sizeof(buf), "");
	SplitString  split;
	split.Split(buf, ",");

	// �Ƿ���Ч��˳������
	BOOL  bValidOrder = TRUE;

	// ��������ΪMAX_GRID_COUNT��
	if ( split.Size() != MAX_GRID_COUNT ) {
		bValidOrder = FALSE;
	}
	else {
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			DWORD  dwTemp = 0;
			// ������
			if (1 == sscanf(split[i], "%lu", &dwTemp)) {
				if (dwTemp == 0 || dwTemp > MAX_GRID_COUNT) {
					bValidOrder = FALSE;
					break;
				}
				g_data.m_CfgData.m_GridOrder[i] = dwTemp - 1;
			}
			// ��������
			else {
				bValidOrder = FALSE;
				break;
			}
		}

		// �ټ��
		if (bValidOrder) {
			for (DWORD i = 0; i < MAX_GRID_COUNT - 1; i++) {
				for (DWORD j = i + 1; j < MAX_GRID_COUNT; j++) {
					// �ظ�
					if (g_data.m_CfgData.m_GridOrder[i] == g_data.m_CfgData.m_GridOrder[j]) {
						bValidOrder = FALSE;
						break;
					}
				}
				if (!bValidOrder) {
					break;
				}
			}
		}
	}

	// ���û����ȷ��˳�����ã�ȡĬ�ϵ�˳��
	if (!bValidOrder) {
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			g_data.m_CfgData.m_GridOrder[i] = i;
		}
	}
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_timer) {
		g_data.m_thrd_timer->Stop();
		delete g_data.m_thrd_timer;
		g_data.m_thrd_timer = 0;
	}

	Clear();
	return 0;
}

void CBusiness::InitSigslot(CDuiFrameWnd * pMainWnd) {
	return;
}


// ��Ϣ����
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case 0:
	{

	}
	break;

	default:
		break;
	}
}