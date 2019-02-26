#include <time.h>
#include "common.h"
#include "launcher.h"
#include "httpstack.h"
#include "LmnTelSvr.h"

CLaunch::CLaunch() {

}

CLaunch::~CLaunch() {

}

void  CLaunch::CloseLaunch() {
	CloseUartPort();
	m_recv_buf.Clear();
	m_sigStatus.emit(GetStatus());
}

// 重连
int  CLaunch::Reconnect() {
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "launch reconnect \n");

	if (GetStatus() == CLmnSerialPort::OPEN) {
		CloseLaunch();
	}

	DWORD  dwCfgLaunchPort = 0;
	g_data.m_cfg->GetConfig("launch port", dwCfgLaunchPort, 0);

	char  szComPort[256] = { 0 };
	if (dwCfgLaunchPort != 0) {
		SNPRINTF(szComPort, sizeof(szComPort), "com%lu", dwCfgLaunchPort);
	}
	else {
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

		// 太多的ch340
		if (nFindCount > 1) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
		// 没有找到ch340
		else if (nFindCount == 0) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
	}

	BOOL bRet = OpenUartPort(szComPort,9600);
	if (!bRet) {
		m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		return 0;
	}

	// 通知成功
	m_sigStatus.emit(GetStatus());
	m_recv_buf.Clear();

	return 0;
}

// 获取数据
int  CLaunch::GetData() {

	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	BYTE buf[8192];

	DWORD  dwBufLen = sizeof(buf);
	if (Read(buf, dwBufLen)) {
		if (dwBufLen > 0) {
			m_recv_buf.Append(buf, dwBufLen);
		}
	}

	// 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
	// 55 18 00 02 04 45 52 00 00 04 00 00 00 00 00 01 38 37 11 37 D8 59 02 E0 16 40 FF 0D 0A
	// new
	//       信道  地址      SN                                  温度
	//       ____  ____  ___________                             _____
	//  1  2  3      4    5  6  7  8    9 10 11 12 13 14 15 16   17 18   19 20 21 22 23
	// 55 15 02     00   00 00 00 0B   8F 63 11 37 D8 59 02 E0   14 39   00 00 00 00 FF 
	const DWORD  dwItemLen = 23;
	CLmnString strText;

	// 如果获取一条温度数据
	if ( m_recv_buf.GetDataLength() >= dwItemLen ) {
		m_recv_buf.Read( buf, dwItemLen );
		//DWORD  dwTemp = (buf[0] - '0') * 1000 + (buf[1] - '0') * 100 + (buf[2] - '0') * 10 + (buf[3] - '0') ;
		if (buf[0] == 0x55 && buf[1] == 0x15 ) {
			DWORD  dwTemp = buf[16] * 100 + buf[17];
			char   szReaderId[256] = {0};
			char   szTagId[256] = { 0 };
			DWORD  dwNum = buf[4] * 256 * 256 * 256 + buf[5] * 256 * 256 + buf[6] * 256 + buf[7];
			SNPRINTF(szReaderId, sizeof(szReaderId), "ER003%06lu", dwNum );
			SNPRINTF(szTagId,    sizeof(szTagId),    "%02x%02x%02x%02x%02x%02x%02x%02x", 
				buf[15], buf[14], buf[13], buf[12], buf[11], buf[10], buf[9], buf[8]);			

			// 如果有服务器地址
			if (g_data.m_szServerAddr[0] != '\0') {
				JTelSvrPrint("try to send a temp record to server!");

				int * pArg = new int[2];
				TempItem * pItem = new TempItem;
				memset(pItem, 0, sizeof(TempItem));
				pItem->dwTemp = dwTemp;
				pItem->tTime = time(0);
				pArg[0] = (int)TYPE_UPLOAD_TEMP;
				pArg[1] = (int)pItem;

				std::string strUrl = g_data.m_szServerAddr;
				strUrl += "/main?type=upload&temp=";
				strUrl += strText.Format("%lu", pItem->dwTemp);
				strUrl += "&time=";
				strUrl += strText.Format("%lu", (DWORD)pItem->tTime);
				strUrl += "&bind=";
				strUrl += strText.Format("%d", g_data.m_bBindingReader);
				strUrl += "&readerid=";
				strUrl += szReaderId;
				strUrl += "&tagid=";
				strUrl += szTagId;

				CHttp::GetInstance()->Get(strUrl, (void *)pArg);
			}
			else {
				JTelSvrPrint("server address is empty.");
			}
		}
		else {
			JTelSvrPrint("com port data is nor correct!");
		}
		
	}
	m_recv_buf.Reform();


	return 0;
}

int  CLaunch::CheckStatus() {

	// 关闭状态
	if (GetStatus() == CLmnSerialPort::CLOSE) {

		DWORD  dwCfgLaunchPort = 0;
		g_data.m_cfg->GetConfig("launch port", dwCfgLaunchPort, 0);

		// 如果串口在
		if ( dwCfgLaunchPort != 0 ) {
			if ( CheckComPortExist((int)dwCfgLaunchPort) ) {
				// m_sigReconnect.emit(0);
				Reconnect();
			}
		}
		else {
			char szComPort[16];
			int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));
			// 如果找到1个ch340串口，立即连接发射器
			if (1 == nFindCount) {
				//m_sigReconnect.emit(0);
				Reconnect();
			}
		}
	}
	// 打开状态
	else {
		// 如果串口已经不在
		if (!CheckComPortExist(this->GetPort())) {
			CloseLaunch();
		}
	}
	return 0;
}