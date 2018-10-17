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

	BOOL bRet = OpenUartPort(szComPort);
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

	const DWORD  dwItemLen = 4;
	CLmnString strText;

	// 如果获取一条温度数据
	if ( m_recv_buf.GetDataLength() >= dwItemLen ) {
		m_recv_buf.Read( buf, dwItemLen );
		DWORD  dwTemp = (buf[0] - '0') * 1000 + (buf[1] - '0') * 100 + (buf[2] - '0') * 10 + (buf[3] - '0') ;

		int * pArg = new int[2];

		TempItem * pItem = new TempItem;
		memset(pItem, 0, sizeof(TempItem));
		pItem->dwTemp = dwTemp;
		pItem->tTime = time(0);

		pArg[0] = (int)TYPE_UPLOAD_TEMP;
		pArg[1] = (int)pItem;

		// 如果有服务器地址
		if (g_data.m_szServerAddr[0] != '\0') {
			std::string strUrl = g_data.m_szServerAddr;
			strUrl += "/main?type=upload&temp=";
			strUrl += strText.Format("%lu", pItem->dwTemp);
			strUrl += "&time=";
			strUrl += strText.Format("%lu", (DWORD)pItem->tTime);
			strUrl += "&bind=";
			strUrl += strText.Format("%d", g_data.m_bBindingReader) ;

			CHttp::GetInstance()->Get(strUrl, (void *)pArg);
		}
		else {
			JTelSvrPrint("server address is empty.");
		}
	}
	m_recv_buf.Reform();


	return 0;
}