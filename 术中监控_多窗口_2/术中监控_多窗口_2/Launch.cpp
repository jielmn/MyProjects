#include "Launch.h"
#include "business.h"

CLaunch::CLaunch() {
}

CLaunch::~CLaunch() {

} 

int  CLaunch::Reconnect() {

	if ( GetStatus() == CLmnSerialPort::OPEN ) {
		CloseUartPort();
		CBusiness::GetInstance()->NotifyUiLaunchStatus(GetStatus());
	}

	char  szComPort[16] = { 0 };
	if ( g_szLaunchComPort[0] != 0 ) {
		STRNCPY(szComPort, g_szLaunchComPort, sizeof(szComPort));
	}
	else {
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

		// ̫���ch340
		if ( nFindCount > 1 ) {
			CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
		// û���ҵ�ch340
		else if (nFindCount == 0) {
			CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
	}

	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
		return 0;
	}

	CBusiness::GetInstance()->NotifyUiLaunchStatus(GetStatus());

	return 0;
}

// Ӳ���Ķ������״̬
int  CLaunch::CheckStatus() {

	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// ��������Ѿ�����
	if ( !CheckComPortExist( this->GetPort() ) ) {
		CloseUartPort();
		CBusiness::GetInstance()->NotifyUiLaunchStatus(GetStatus());
		CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
	}

	return 0;
}