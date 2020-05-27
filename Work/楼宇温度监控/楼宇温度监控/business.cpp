#include "business.h"
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

	m_db.InitDb();

	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);


	g_data.m_cfg->GetIntConfig("high temperature", g_data.m_nHighTemp, 4000);

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



	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	g_data.m_thrd_launch = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_launch) {
		return -1;
	}
	g_data.m_thrd_launch->Start();


	// 不停的读取串口数据
	g_data.m_thrd_launch->PostMessage(this, MSG_READ_COM_PORTS);

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	if (g_data.m_thrd_launch) {
		g_data.m_thrd_launch->Stop();
		delete g_data.m_thrd_launch;
		g_data.m_thrd_launch = 0;
	}

	Clear();

	m_db.DeinitDb();

	return 0;
}

void  CBusiness::AddFloorAsyn(HWND hWnd, int nFloor) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADD_FLOOR, new CAddFloorParam(hWnd, nFloor));
}

void  CBusiness::AddFloor(const CAddFloorParam * pParam) {
	BOOL bRet = m_db.AddFloor(pParam);
	::PostMessage(pParam->m_hWnd, UM_ADD_FLOOR_RET, bRet, pParam->m_nFloor);
}

void  CBusiness::GetAllFloorsAsyn() {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_ALL_CLASSES);
}

void  CBusiness::GetAllFloors() {
	std::vector<int> * pvRet = new std::vector<int>;
	m_db.GetAllFloors(*pvRet);
	::PostMessage(g_data.m_hWnd, UM_GET_ALL_FLOORS_RET, (WPARAM)pvRet, 0);
}

void  CBusiness::GetDevicesByFloorAsyn(int nFloor) {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_DEVICES_BY_FLOOR, new CGetDevicesParam(nFloor));
}

void  CBusiness::GetDevicesByFloor(const CGetDevicesParam * pParam) {
	std::vector<DeviceItem*> * pvRet = new std::vector<DeviceItem*>;
	m_db.GetDevicesByFloor(pParam, *pvRet);
	::PostMessage(g_data.m_hWnd, UM_GET_DEVICES_BY_FLOOR_RET, (WPARAM)pvRet, pParam->m_nFloor);
}

void  CBusiness::AddDeviceAsyn(HWND hWnd, int nFloor, int nDeviceId, const char * szDeviceAddr) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADD_DEVICE, new CAddDeviceParam(hWnd, nFloor, nDeviceId, szDeviceAddr));
}

void  CBusiness::AddDevice(const CAddDeviceParam * pParam) {
	BOOL bRet = m_db.AddDevice(pParam);
	
	DeviceItem * pDevice = 0;
	if (bRet) {
		pDevice = new DeviceItem;
		memset(pDevice, 0, sizeof(DeviceItem));
		pDevice->nFloor = pParam->m_nFloor;
		pDevice->nDeviceNo = pParam->m_nDeviceId;
		STRNCPY(pDevice->szAddr, pParam->m_szDeviceAddr, sizeof(pDevice->szAddr));
	}	

	::PostMessage( pParam->m_hWnd, UM_ADD_DEVICE_RET, bRet, (LPARAM)pDevice );
}

void  CBusiness::DeleteDeviceAsyn(int nDeviceId) {
	g_data.m_thrd_db->PostMessage(this, MSG_DEL_DEVICE, new CDelDeviceParam(nDeviceId));
}

void  CBusiness::DeleteDevice(const CDelDeviceParam * pParam) {
	BOOL bRet = m_db.DeleteDevice(pParam);
	::PostMessage(g_data.m_hWnd, UM_DEL_DEVICE_RET, bRet, pParam->m_nDeviceId);
}

void CBusiness::DeleteFloorAsyn(int nFloor) {
	g_data.m_thrd_db->PostMessage(this, MSG_DEL_FLOOR, new CDelFloorParam(nFloor));
}

void CBusiness::DeleteFloor(const CDelFloorParam * pParam) {
	BOOL bRet = m_db.DeleteFloor(pParam);
	::PostMessage(g_data.m_hWnd, UM_DEL_FLOOR_RET, bRet, pParam->m_nFloor);
}

// 硬件改动，检查接收器串口状态
void  CBusiness::CheckLaunchAsyn() {
	g_data.m_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
}

void   CBusiness::CheckLaunch() {
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
				delete pItem;
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

// 读取所有的串口数据
void CBusiness::ReadAllComPorts() {
	BYTE   buf[8192];
	DWORD  dwBufLen = 0;
	std::vector<CMyComPort *>::iterator  it_com;
	char debug_buf[8192];

	const int MIN_DATA_LENGTH = 10;
	const int HAND_TEMP_DATA_LENGTH = 10;

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

			if (buf[0] != 0x66) {
				DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
				g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据头：\n%s\n", debug_buf);
				pCom->CloseUartPort();
				bClosed = TRUE;
				break;
			}

			// 如果是手持温度数据
			if ( TRUE ) {
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
			//if (now - pItem->m_last_cmd_time >= REINIT_COM_PORT_TIME) {
			//	InitComPort(&pItem->m_com);
			//	pItem->m_last_cmd_time = now;
			//}
		}

		++it_com;
	}

	g_data.m_thrd_launch->PostDelayMessage(1000, this, MSG_READ_COM_PORTS);
}

// 处理手持读卡器数据
BOOL CBusiness::ProcHandeReader(CLmnSerialPort * pCom, const BYTE * pData, DWORD dwDataLen, TempItem & item) {
	char debug_buf[8192];

	// 如果最后一个字节不是0xFF
	if (pData[dwDataLen - 1] != 0xAA || pData[dwDataLen - 2] != 0xDD ) {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾(跳过，不处理)：\n%s\n", debug_buf);
		return FALSE;
	}

	memset(&item, 0, sizeof(item));
	item.m_dwTemp = pData[6] * 100 + pData[7];
	item.m_time = time(0);
	item.m_nDeviceId = (pData[2] << 24) | (pData[3] << 16) | (pData[4] << 8) | pData[5];
	
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
	m_db.SaveTemp(&pParam->m_item);
	TempItem * pItem = new TempItem;
	memcpy(pItem, &pParam->m_item, sizeof(TempItem));
	::PostMessage(g_data.m_hWnd, UM_TEMPERATURE, (WPARAM)pItem, 0);
}

// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_ADD_FLOOR:
	{
		CAddFloorParam * pParam = (CAddFloorParam *)pMessageData;
		AddFloor(pParam);
	}
	break;

	case MSG_GET_ALL_CLASSES:
	{
		GetAllFloors();
	}
	break;

	case MSG_GET_DEVICES_BY_FLOOR:
	{
		CGetDevicesParam * pParam = (CGetDevicesParam *)pMessageData;
		GetDevicesByFloor(pParam);
	}
	break;

	case MSG_ADD_DEVICE:
	{
		CAddDeviceParam * pParam = (CAddDeviceParam *)pMessageData;
		AddDevice(pParam);
	}
	break;

	case MSG_DEL_DEVICE:
	{
		CDelDeviceParam * pParam = (CDelDeviceParam *)pMessageData;
		DeleteDevice(pParam);
	}
	break;

	case MSG_DEL_FLOOR:
	{
		CDelFloorParam * pParam = (CDelFloorParam *)pMessageData;
		DeleteFloor(pParam);
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

	default:
		break;
	}
}