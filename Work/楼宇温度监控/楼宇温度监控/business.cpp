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

	// �������˿�
	char  szComPorts[256];
	g_data.m_cfg->GetConfig(CFG_LAUNCH_COM_PORT, szComPorts, sizeof(szComPorts), "");

	// ���û��ָ����ȡ�Ĵ���
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

		// ����û��ָ����ȡ�Ĵ���
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


	// ��ͣ�Ķ�ȡ��������
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

// Ӳ���Ķ���������������״̬
void  CBusiness::CheckLaunchAsyn() {
	g_data.m_thrd_launch->PostMessage(this, MSG_CHECK_LAUNCH_STATUS, 0, TRUE);
}

void   CBusiness::CheckLaunch() {
	// ��ȡ��ǰ���������д���(���ںţ������ַ���)
	std::map<int, std::string>  all_ports;
	GetAllSerialPorts(all_ports);

	// ���㽻�����������ȱ�ٵĲ���
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

	// ��v3��Ĵ��ڹرյ�
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

	// �����ָ��������ڻ�ȡ����
	if (g_data.m_bSpecifiedComports) {
		for (it = all_ports.begin(); it != all_ports.end(); ++it) {
			int nCom = it->first;
			// �����µĴ����Ƿ�Ҫ��
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

// ��ȡ���еĴ�������
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
		BOOL   bHasData = FALSE;              // �Ƿ���͵�����

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

		// ����ر�
		if (bClosed) {
			delete pItem;
			it_com = m_vSerialPorts.erase(it_com);
			continue;
		}


		// ���û�йرգ���������
		if (pBuf->GetDataLength() < MIN_DATA_LENGTH) {
			++it_com;
			continue;
		}

		while (pBuf->Read(buf, MIN_DATA_LENGTH)) {

			if (buf[0] != 0x66) {
				DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
				g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������ͷ��\n%s\n", debug_buf);
				pCom->CloseUartPort();
				bClosed = TRUE;
				break;
			}

			// ������ֳ��¶�����
			if ( TRUE ) {
				DWORD  dwExtraLength = HAND_TEMP_DATA_LENGTH - MIN_DATA_LENGTH;
				DWORD  dwExpectedLength = HAND_TEMP_DATA_LENGTH;

				// ���û���㹻����
				if (pBuf->GetDataLength() < dwExtraLength) {
					pBuf->ResetReadPos();
					break;
				}

				// �������������� 
				pBuf->Read(buf + MIN_DATA_LENGTH, dwExtraLength);
				pBuf->Reform();

				TempItem item;
				BOOL bHandled = ProcHandeReader(pCom, buf, dwExpectedLength, item);
				// �����β���ܴ��ڵ�"dd aa"
				ProcTail(pBuf);

				// �õ�һ���¶�����
				if (bHandled) {
					g_data.m_thrd_db->PostMessage(this, MSG_TEMPERATURE_ITEM, new CTempParam(&item));
					bHasData = TRUE;
				}
			}
		}

		// ����ر�
		if (bClosed) {
			delete pItem;
			it_com = m_vSerialPorts.erase(it_com);
			continue;
		}

		// ���û�н��յ�����
		if (!bHasData) {
			// ���������һ��ʱ�䣬��Ҫ���³�ʼ������
			//if (now - pItem->m_last_cmd_time >= REINIT_COM_PORT_TIME) {
			//	InitComPort(&pItem->m_com);
			//	pItem->m_last_cmd_time = now;
			//}
		}

		++it_com;
	}

	g_data.m_thrd_launch->PostDelayMessage(1000, this, MSG_READ_COM_PORTS);
}

// �����ֳֶ���������
BOOL CBusiness::ProcHandeReader(CLmnSerialPort * pCom, const BYTE * pData, DWORD dwDataLen, TempItem & item) {
	char debug_buf[8192];

	// ������һ���ֽڲ���0xFF
	if (pData[dwDataLen - 1] != 0xAA || pData[dwDataLen - 2] != 0xDD ) {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(������������)��\n%s\n", debug_buf);
		return FALSE;
	}

	memset(&item, 0, sizeof(item));
	item.m_dwTemp = pData[6] * 100 + pData[7];
	item.m_time = time(0);
	item.m_nDeviceId = (pData[2] << 24) | (pData[3] << 16) | (pData[4] << 8) | pData[5];
	
	return TRUE;
}

// �����β���ܴ��ڵ�"dd aa"
void  CBusiness::ProcTail(CDataBuf * pBuf) {
	if (pBuf->GetDataLength() >= 2) {
		BYTE  buf[32];
		pBuf->Read(buf, 2);
		// ����� dd aa��β
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

// ��Ϣ����
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