#include "VPort.h"

int  CVPort::HandleData() {
	char buf[8192];
	DWORD  rcvDataLen = sizeof(buf);
	BOOL bRet = this->Read( buf, rcvDataLen );
	if (bRet) {
		if (rcvDataLen > 0) {
			m_recv.Append(buf, rcvDataLen);
			//g_log->Output(ILog::LOG_SEVERITY_INFO, "received length = %lu \n", rcvDataLen);
		}		
	}

	DWORD w = 0;
	//DWORD a = m_recv.GetDataLength();
	//const BYTE * b = (const BYTE *)m_recv.GetData();
	if ( m_recv.GetDataLength() >= 5 ) {
		BYTE tmp[8192] = { 0 };
		m_recv.Read(tmp, 5);

		if ( 0 == memcmp(tmp, "\x55\x01\x01\xDD\xAA", 5) ) {
			w = 4;
			this->Write("\xFF\x55\x55\xFF", w);
		}
		 else if (0 == memcmp(tmp, "\x55\x01\x02\xDD\xAA", 5)) {
			w = 14;
			DWORD dwTemp = GetRand(3000, 4150);
			tmp[0] = (char)0xFF;
			tmp[1] = (char)(dwTemp / 1000);
			tmp[2] = (char)( ( dwTemp / 100 ) % 10 );
			tmp[3] = (char)( ( dwTemp / 10 ) % 10 );
			tmp[4] = (char) ( dwTemp % 10 );
			tmp[13] = (char)0xFF;
			this->Write(tmp, w);
		}
		else {
			return -1;
		}
	}

	return 0;
}