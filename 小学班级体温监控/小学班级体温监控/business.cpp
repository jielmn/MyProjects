#include "business.h"
#include <assert.h>
#include <time.h>

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

	std::vector<CMyComPort *>::iterator it_com;
	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
		CMyComPort * pItem = *it_com;
		pItem->m_com.CloseUartPort();
		delete pItem;
	}
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

	g_data.m_cfg->GetConfig("room rows", g_data.m_dwRoomRows, 6);
	g_data.m_cfg->GetConfig("room cols", g_data.m_dwRoomCols, 8);

	if (g_data.m_dwRoomRows == 0) {
		g_data.m_dwRoomRows = 6;
	}
	if (g_data.m_dwRoomCols == 0) {
		g_data.m_dwRoomCols = 8;
	}

	g_data.m_DeskSize.cx = 94;                           
	g_data.m_DeskSize.cy = 88;               
	         
	STRNCPY(g_data.m_aszChNo[0], "一", 6);
	STRNCPY(g_data.m_aszChNo[1], "二", 6);
	STRNCPY(g_data.m_aszChNo[2], "三", 6);
	STRNCPY(g_data.m_aszChNo[3], "四", 6);
	STRNCPY(g_data.m_aszChNo[4], "五", 6);
	STRNCPY(g_data.m_aszChNo[5], "六", 6);
	STRNCPY(g_data.m_aszChNo[6], "七", 6);
	STRNCPY(g_data.m_aszChNo[7], "八", 6);
	STRNCPY(g_data.m_aszChNo[8], "九", 6);
	STRNCPY(g_data.m_aszChNo[9], "十", 6);

	// 发射器端口
	char  szComPorts[256];
	g_data.m_cfg->GetConfig(CFG_LAUNCH_COM_PORT, szComPorts, sizeof(szComPorts), "");

	// 如果没有指定读取的串口
	if (szComPorts[0] == '\0') {
		g_data.m_bSpecifiedComports = FALSE;
		g_data.m_cfg->GetBooleanConfig("multiple com port", g_data.m_bMultipleComport, FALSE);
	}
	else {
		Str2Lower(szComPorts);
		SplitString s;
		s.Split(szComPorts, ',');
		if (s.Size() > 0) {
			int m = s.Size() > MAX_COM_PORTS_CNT ? MAX_COM_PORTS_CNT : s.Size();
			for (int i = 0; i < m; i++) {
				int nCom = 0;
				int ret = sscanf(s[i], " com%d", &nCom);
				if (1 == ret) {
					g_data.m_nComports[g_data.m_nComportsCnt] = nCom;
					g_data.m_nComportsCnt++;
				}
			}
		}

		// 还是没有指定读取的串口
		if (g_data.m_nComportsCnt == 0) {
			g_data.m_bSpecifiedComports = FALSE;
			g_data.m_cfg->GetBooleanConfig("multiple com port", g_data.m_bMultipleComport, FALSE);
		}
		else {
			g_data.m_bSpecifiedComports = TRUE;
		}
	}


	char szReaderRange[256];
	g_data.m_cfg->GetConfig("reader range", szReaderRange, sizeof(szReaderRange), "");
	SplitString split;
	split.Split(szReaderRange, ',');
	if (split.Size() > 0) {
		if (split.Size() > 1) {
			sscanf(split[0], " %d", &g_data.m_nReaderNoStart);
			sscanf(split[1], " %d", &g_data.m_nReaderNoEnd);
		}
		else {
			sscanf(split[0], " %d", &g_data.m_nReaderNoStart);
			g_data.m_nReaderNoEnd = g_data.m_nReaderNoStart;
		}
	}
	else {
		g_data.m_nReaderNoStart = DEFAULT_READER_NO_START;
		g_data.m_nReaderNoEnd = DEFAULT_READER_NO_END;
	}

	if (g_data.m_nReaderNoStart <= 0) {
		g_data.m_nReaderNoStart = DEFAULT_READER_NO_START;
	}

	if (g_data.m_nReaderNoStart > g_data.m_nReaderNoEnd) {
		g_data.m_nReaderNoEnd = g_data.m_nReaderNoStart;
	}

	g_data.m_cfg->GetConfig("high temperature", g_data.m_dwHighTemp, 3730);
	g_data.m_cfg->GetConfig("low temperature",  g_data.m_dwLowTemp,  3500);

	if ( g_data.m_dwLowTemp > g_data.m_dwHighTemp ) {
		g_data.m_dwHighTemp = 3730;
		g_data.m_dwLowTemp = 3500;
	}

	m_db.InitDb();

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	g_data.m_thrd_launch_cube = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_launch_cube) {
		return -1;
	}
	g_data.m_thrd_launch_cube->Start();


	// 不停的读取串口数据
	g_data.m_thrd_launch_cube->PostMessage(this, MSG_READ_COM_PORTS);

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	if (g_data.m_thrd_launch_cube) {
		g_data.m_thrd_launch_cube->Stop();
		delete g_data.m_thrd_launch_cube;
		g_data.m_thrd_launch_cube = 0;
	}

	Clear();

	m_db.DeinitDb();
	return 0;
}

void   CBusiness::AddClassAsyn( HWND hWnd, DWORD dwGrade, DWORD  dwClass) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADD_CLASS, new CAddClassParam(hWnd, dwGrade, dwClass));
}

void   CBusiness::AddClass(const CAddClassParam * pParam) {
	BOOL bRet = m_db.AddClass(pParam);
	::PostMessage(pParam->m_hWnd, UM_ADD_CLASS_RET, bRet, 0);
}

void   CBusiness::GetAllClassesAsyn() {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_ALL_CLASSES);
}

void   CBusiness::GetAllClasses() {
	std::vector<DWORD> * pvRet = new std::vector<DWORD>;
	m_db.GetAllClasses(*pvRet, m_BindingTags);
	::PostMessage(g_data.m_hWnd, UM_GET_ALL_CLASSES_RET, (WPARAM)pvRet, 0);
}

void  CBusiness::GetRoomDataAsyn(DWORD dwNo) {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_ROOM, new CGetRoomParam(dwNo));
}

void  CBusiness::GetRoomData(const CGetRoomParam * pParam) {
	std::vector<DeskItem*> * pvRet = new std::vector<DeskItem*>;
	m_db.GetRoomData(pParam, *pvRet);
	::PostMessage(g_data.m_hWnd, UM_GET_ROOM_RET, (WPARAM)pvRet, pParam->m_dwNo);
}

void  CBusiness::ModifyDeskAsyn(DWORD dwClassNo, DWORD dwPos, const char * szName, int nSex) {
	g_data.m_thrd_db->PostMessage(this, MSG_MODIFY_DESK, new CModifyDeskParam(dwClassNo, dwPos, szName,nSex));
}

void  CBusiness::ModifyDesk(const CModifyDeskParam * pParam) {
	m_db.ModifyDesk(pParam);
	DeskItem * pItem = new DeskItem;
	memset(pItem, 0, sizeof(DeskItem));
	STRNCPY(pItem->szName, pParam->m_szName, sizeof(pItem->szName));
	pItem->nSex   = pParam->m_nSex;
	pItem->nGrade = HIBYTE(pParam->m_dwNo);
	pItem->nClass = LOBYTE(pParam->m_dwNo);
	pItem->nRow   = HIBYTE(pParam->m_dwPos);
	pItem->nCol   = LOBYTE(pParam->m_dwPos);
	::PostMessage(g_data.m_hWnd, UM_MODIFY_DESK_RET, (WPARAM)pItem, 0);
}

void   CBusiness::EmptyDeskAsyn(DWORD dwClassNo, DWORD dwPos) {
	g_data.m_thrd_db->PostMessage(this, MSG_EMPTY_DESK, new CEmptyDeskParam(dwClassNo, dwPos));
}

void   CBusiness::EmptyDesk(const CEmptyDeskParam * pParam) {
	m_db.EmptyDesk(pParam);

	DWORD  dwGreat = MAKELONG((WORD)pParam->m_dwPos, (WORD)pParam->m_dwNo);
	std::map<std::string, DWORD>::iterator it;
	for (it = m_BindingTags.begin(); it != m_BindingTags.end(); ++it) {
		if (it->second == dwGreat) {
			m_BindingTags.erase(it);
			break;
		}
	}
	::PostMessage(g_data.m_hWnd, UM_EMPTY_DESK_RET, pParam->m_dwNo, pParam->m_dwPos);
}

void  CBusiness::DeleteClassAsyn(DWORD  dwClassNo) {
	g_data.m_thrd_db->PostMessage(this, MSG_DELETE_CLASS, new CDeleteClassParam(dwClassNo));
}

void  CBusiness::DeleteClass(const CDeleteClassParam * pParam) {
	m_db.DeleteClass(pParam);

	std::map<std::string, DWORD>::iterator it;
	for (it = m_BindingTags.begin(); it != m_BindingTags.end(); ) {
		WORD  wNo = HIWORD(it->second);
		if ( (DWORD)wNo == pParam->m_dwNo ) {
			it = m_BindingTags.erase(it);
		}
		else {
			++it;
		}
	}

	::PostMessage(g_data.m_hWnd, UM_DELETE_CLASS_RET, pParam->m_dwNo, 0);
}

void  CBusiness::ExchangeDeskAsyn(DWORD  dwClassNo, DWORD  dwPos1, DWORD  dwPos2) {
	g_data.m_thrd_db->PostMessage(this, MSG_EXCHANGE_DESK, new CExchangeDeskParam(dwClassNo, dwPos1, dwPos2));
}
 
void  CBusiness::ExchangeDesk(const CExchangeDeskParam * pParam) {
	DeskItem * pDesk1 = new DeskItem;
	DeskItem * pDesk2 = new DeskItem;
	m_db.ExchangeDesk(pParam, *pDesk1, *pDesk2, m_BindingTags); 
	::PostMessage(g_data.m_hWnd, UM_EXCHANGE_DESK_RET, (WPARAM)pDesk1, (LPARAM)pDesk2);
}

void  CBusiness::CheckLaunchAsyn() {
	g_data.m_thrd_launch_cube->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
}

void  CBusiness::CheckLaunch() {
	// 获取当前机器的所有串口(串口号，描述字符串)
	std::map<int, std::string>  all_ports;
	GetAllSerialPorts(all_ports);

	// 计算交集，多出来，缺少的部分
	std::map<int, std::string>   m1;
	std::map<int, std::string>   m2;
	std::vector<int>             v3;

	std::vector<CMyComPort *>::iterator it_com;
	std::map<int, std::string>::iterator it;

	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pComPort = &pItem->m_com;
		assert(pComPort->GetStatus() == CLmnSerialPort::OPEN);

		it = all_ports.find(pComPort->GetPort());
		if (it != all_ports.end()) {
			m1.insert(std::make_pair(it->first, it->second));
			all_ports.erase(it);
		}
		else {
			v3.push_back(pComPort->GetPort());
		}
	}

	// 把v3里的串口关闭掉
	std::vector<int>::iterator ix;
	for (ix = v3.begin(); ix != v3.end(); ++ix) {
		int nCom = *ix;
		for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
			CMyComPort * pItem = *it_com;
			CLmnSerialPort * pComPort = &pItem->m_com;
			if (pComPort->GetPort() == nCom) {
				pComPort->CloseUartPort();
				m_vSerialPorts.erase(it_com);
				break;
			}
		}
	}

	time_t now = time(0);

	// 如果是指定多个串口获取数据
	if (g_data.m_bSpecifiedComports) {
		for (it = all_ports.begin(); it != all_ports.end(); ++it) {
			int nCom = it->first;
			// 看看新的串口是否要打开
			for (int i = 0; i < g_data.m_nComportsCnt; i++) {
				if (nCom == g_data.m_nComports[i]) {
					CMyComPort * pItem = new CMyComPort;
					CLmnSerialPort * pPort = &pItem->m_com;
					BOOL bRet = pPort->OpenUartPort(nCom);
					if (bRet) {
						m_vSerialPorts.push_back(pItem);
						InitComPort(pPort);
						pItem->m_last_cmd_time = now;
					}
					else {
						pPort->CloseUartPort();
						delete pItem;
					}
					break;
				}
			}
		}
	}
	else {
		for (it = all_ports.begin(); it != all_ports.end(); ++it) {
			int nCom = it->first;
			std::string & s = it->second;

			char buf[256];
			Str2Lower(s.c_str(), buf, sizeof(buf));

			if (0 != strstr(buf, "usb-serial ch340")) {
				CMyComPort * pItem = new CMyComPort;
				CLmnSerialPort * pPort = &pItem->m_com;
				BOOL bRet = pPort->OpenUartPort(nCom);
				if (bRet) {
					m_vSerialPorts.push_back(pItem);
					InitComPort(pPort);
					pItem->m_last_cmd_time = now;
				}
				else {
					pPort->CloseUartPort();
					delete pItem;
				}
			}
		}
	}

	std::vector<int> * pvRet = new std::vector<int>;
	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ++it_com) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pCom = &pItem->m_com;
		pvRet->push_back(pCom->GetPort());
	}

	::PostMessage(g_data.m_hWnd, UM_CHECK_COM_PORTS_RET, (WPARAM)pvRet, 0);
}

void  CBusiness::ReadAllComPorts() {
	BYTE   buf[8192];
	DWORD  dwBufLen = 0;
	std::vector<CMyComPort *>::iterator  it_com;
	char debug_buf[8192];

	const int MIN_DATA_LENGTH = 16;
	const int BATERRY_TEMP_DATAl_LENGTH = 16;       // 有源tag
	const int HAND_TEMP_DATA_LENGTH = 19;

	time_t now = time(0);

	for (it_com = m_vSerialPorts.begin(); it_com != m_vSerialPorts.end(); ) {
		CMyComPort * pItem = *it_com;
		CLmnSerialPort * pCom = &pItem->m_com;
		CDataBuf * pBuf = &pItem->m_buf;
		BOOL   bHasData = FALSE;              // 是否接送到数据

		BOOL  bClosed = FALSE;
		while (TRUE) {
			dwBufLen = sizeof(buf);
			if (pCom->Read(buf, dwBufLen)) {
				if (dwBufLen > 0) {
					pBuf->Append(buf, dwBufLen);
				}
				else {
					break;
				}
			}
			else {
				pCom->CloseUartPort();
				bClosed = TRUE;
				break;
			}
		}

		// 如果关闭
		if (bClosed) {
			delete pItem;
			it_com = m_vSerialPorts.erase(it_com);
			continue;
		}


		// 如果没有关闭，处理数据
		if (pBuf->GetDataLength() < MIN_DATA_LENGTH) {
			++it_com;
			continue;
		}

		while (pBuf->Read(buf, MIN_DATA_LENGTH)) {

			if (buf[0] != 0x55) {
				DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
				g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据头：\n%s\n", debug_buf);
				pCom->CloseUartPort();
				bClosed = TRUE;
				break;
			}

			// 如果是手持温度数据
			if (buf[1] == 0x11) {
				DWORD  dwExtraLength = HAND_TEMP_DATA_LENGTH - MIN_DATA_LENGTH;
				DWORD  dwExpectedLength = HAND_TEMP_DATA_LENGTH;

				// 如果没有足够数据
				if (pBuf->GetDataLength() < dwExtraLength) {
					pBuf->ResetReadPos();
					break;
				}

				// 读出完整的数据 
				pBuf->Read(buf + MIN_DATA_LENGTH, dwExtraLength);
				pBuf->Reform();

				TempItem item;
				BOOL bHandled = ProcHandeReader(pCom, buf, dwExpectedLength, item);
				// 清除结尾可能存在的"dd aa"
				ProcTail(pBuf);

				// 得到一条温度数据
				if (bHandled) {
					g_data.m_thrd_db->PostMessage(this, MSG_TEMPERATURE_ITEM, new CTempParam(&item));
					bHasData = TRUE;
				}
			}
		}

		// 如果关闭
		if (bClosed) {
			delete pItem;
			it_com = m_vSerialPorts.erase(it_com);
			continue;
		}

		// 如果没有接收到数据
		if (!bHasData) {
			// 如果经过了一定时间，需要重新初始化串口
			if (now - pItem->m_last_cmd_time >= REINIT_COM_PORT_TIME) {
				InitComPort(&pItem->m_com);
				pItem->m_last_cmd_time = now;
			}
		}

		++it_com;
	}

	g_data.m_thrd_launch_cube->PostDelayMessage(1000, this, MSG_READ_COM_PORTS);
}

// 处理手持读卡器数据
BOOL   CBusiness::ProcHandeReader(CLmnSerialPort * pCom, const BYTE * pData, DWORD dwDataLen, TempItem & item) {
	char debug_buf[8192];

	// 如果最后一个字节不是0xFF
	if (pData[dwDataLen - 1] != 0xFF) {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾(跳过，不处理)：\n%s\n", debug_buf);
		return FALSE;
	}

	memset(&item, 0, sizeof(item));
	item.m_dwTemp = pData[16] * 100 + pData[17];
	item.m_time = time(0);
	GetTagId(item.m_szTagId, sizeof(item.m_szTagId), pData + 8, 8);
	GetHandReaderId(item.m_szReaderId, sizeof(item.m_szReaderId), pData + 4);

	return TRUE;
}

// 清除结尾可能存在的"dd aa"
void  CBusiness::ProcTail(CDataBuf * pBuf) {
	if (pBuf->GetDataLength() >= 2) {
		BYTE  buf[32];
		pBuf->Read(buf, 2);
		// 如果是 dd aa结尾
		if (buf[0] == 0xDD && buf[1] == 0xAA) {
			pBuf->Reform();
		}
		else {
			pBuf->ResetReadPos();
		}
	}
}

void  CBusiness::OnTempItem(const CTempParam * pParam) {
	std::map<std::string, DWORD>::iterator it = m_BindingTags.find(pParam->m_item.m_szTagId);
	// 如果是新tag
	if (it == m_BindingTags.end()) {
		TempItem * pItem = new TempItem;
		memcpy(pItem, &pParam->m_item, sizeof(TempItem));
		::PostMessage(g_data.m_hWnd, UM_NEW_TAG_TEMPERATURE, (WPARAM)pItem, 0);
	}
	// 不是新Tag
	else {
		m_db.SaveTemp(it->second, &pParam->m_item);

		TempItem * pItem = new TempItem;
		memcpy(pItem, &pParam->m_item, sizeof(TempItem));
		::PostMessage(g_data.m_hWnd, UM_DESK_TEMPERATURE, (WPARAM)pItem, it->second);
	}
}

void  CBusiness::BindingTag2DeskAsyn(const char * szTagId, DWORD  dwClassNo, DWORD  dwPos) {
	g_data.m_thrd_db->PostMessage(this, MSG_BINDING_TAG, new CBindingTagParam( szTagId, dwClassNo, dwPos) );
}

void  CBusiness::BindingTag2Desk(const CBindingTagParam * pParam) {
	m_db.BindingTag2Desk(pParam, m_BindingTags);
	DWORD  dwGreat = MAKELONG((WORD)pParam->m_dwPos, (WORD)pParam->m_dwNo);
	char * szTagId = new char[20];
	STRNCPY(szTagId, pParam->m_szTagId, 20);
	::PostMessage(g_data.m_hWnd, UM_BINDING_TAG_RET, dwGreat, (LPARAM)szTagId);
}

void  CBusiness::DisableBindingAsyn(WORD wClassNo, WORD  wPos) {
	g_data.m_thrd_db->PostMessage(this, MSG_DISABLE_BINDING_TAG, new CDisableBindingTagParam(wClassNo, wPos));
}

void  CBusiness::DisableBinding(const CDisableBindingTagParam * pParam) {
	m_db.DisableBinding(pParam);

	DWORD  dwGreat = MAKELONG(pParam->m_wPos, pParam->m_wNo);
	std::map<std::string, DWORD>::iterator it;
	for (it = m_BindingTags.begin(); it != m_BindingTags.end(); ++it) {
		if (it->second == dwGreat) {
			m_BindingTags.erase(it);
			break;
		}
	}

	::PostMessage(g_data.m_hWnd, UM_DISABLE_BINDING_RET, pParam->m_wNo, pParam->m_wPos);
}

// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_ADD_CLASS:
	{
		CAddClassParam * pParam = (CAddClassParam *)pMessageData;
		AddClass(pParam);
	}
	break;

	case MSG_GET_ALL_CLASSES:
	{
		GetAllClasses();
	}
	break;

	case MSG_GET_ROOM:
	{
		CGetRoomParam * pParam = (CGetRoomParam *)pMessageData;
		GetRoomData(pParam);
	}
	break;

	case MSG_MODIFY_DESK:
	{
		CModifyDeskParam * pParam = (CModifyDeskParam *)pMessageData;
		ModifyDesk(pParam);
	}
	break;

	case MSG_EMPTY_DESK:
	{
		CEmptyDeskParam * pParam = (CEmptyDeskParam *)pMessageData;
		EmptyDesk(pParam);
	}
	break;

	case MSG_DELETE_CLASS:
	{
		CDeleteClassParam * pParam = (CDeleteClassParam *)pMessageData;
		DeleteClass(pParam);
	}
	break;

	case MSG_EXCHANGE_DESK:
	{
		CExchangeDeskParam * pParam = (CExchangeDeskParam *)pMessageData;
		ExchangeDesk(pParam);
	}
	break;

	case MSG_CHECK_LAUNCH_STATUS:
	{
		CheckLaunch();
	}
	break;

	case MSG_READ_COM_PORTS:
	{
		ReadAllComPorts();
	}
	break;

	case MSG_TEMPERATURE_ITEM:
	{
		CTempParam * pParam = (CTempParam *)pMessageData;
		OnTempItem(pParam);
	}
	break;

	case MSG_BINDING_TAG:
	{
		CBindingTagParam * pParam = (CBindingTagParam *)pMessageData;
		BindingTag2Desk(pParam);
	}
	break;

	case MSG_DISABLE_BINDING_TAG:
	{
		CDisableBindingTagParam * pParam = (CDisableBindingTagParam *)pMessageData;
		DisableBinding(pParam);
	}
	break;

	default:
		break;
	}
}