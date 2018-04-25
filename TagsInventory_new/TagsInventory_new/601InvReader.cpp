#include <afx.h>
#include <vector>
#include <string>
#include "LmnCommon.h"
#include "LmnString.h"
#include "610InvReader.h"

#define  RECONNECT_TIME                         10000

#define  MAX_READER_COMMAND_LENGTH              256
// Readerͨ��Э������
typedef struct tagReaderCmd {
	BYTE      abyCommand[MAX_READER_COMMAND_LENGTH];
	DWORD     dwCommandLength;
}ReaderCmd;


//����������ȡ��ɣ������ޱ�ǩ����ȡʧ��
#define  STATUS_NOISE                           0x58
#define  STATUS_COMPLETE                        0x59
#define  STATUS_NO_TAG                          0x94
#define  STATUS_FAIL                            0x95

static ReaderCmd  PREPARE_COMMAND;
static ReaderCmd  INVENTORY_COMMAND;

static BOOL GetAllSerialPortName(std::vector<std::string> & vCom);
static int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd);





C601InvReader::C601InvReader() {
	m_eStatus = C601InvReader::STATUS_CLOSE;
	TransferReaderCmd(PREPARE_COMMAND,   "dd 11 ef 09 00 01 00 00 00");
	TransferReaderCmd(INVENTORY_COMMAND, "dd 11 ef 09 00 01 00 00 00");
	
}

C601InvReader::~C601InvReader() {
	Clear();
}

C601InvReader::READER_STATUS C601InvReader::GetStatus() const {
	return m_eStatus;
}

void  C601InvReader::Clear() {
	CloseUartPort();
	m_received_data.Clear();
	m_eStatus = C601InvReader::STATUS_CLOSE;
}

void  C601InvReader::ReceiveAsPossible(DWORD  dwIntervalTime, DWORD dwMaxDataLength) {
	BYTE   buf[8192];
	DWORD  dwLeft = dwMaxDataLength;
	DWORD  dwReadLen = 0;

	do
	{
		Sleep(dwIntervalTime);
		if ( dwLeft > sizeof(buf) ) {
			dwReadLen = sizeof(buf);
		}
		else {
			dwReadLen = dwLeft;
		}

		Read( buf, dwReadLen );
		m_received_data.Append( buf, dwReadLen );
		dwLeft -= dwReadLen;

		// �������������
		if (0 == dwReadLen) {
			break;
		}

		// ����Ѿ������������(��ֹ��ѭ��)
		if (0 == dwLeft) {
			break;
		}
		
	} while (TRUE);
}

// ����
int  C601InvReader::Reconnect() {

	// ����Ѿ�������
	if ( m_eStatus == STATUS_OPEN ) {
		return 0;
	}

	std::vector<std::string>  vCom;
	// ��ȡ���еĴ�����Ϣ
	GetAllSerialPortName(vCom);

	// �ȹر�֮ǰ���ܴ򿪵Ĵ���
	Clear();


	BOOL  bPrepared = FALSE;
	// ���Դ򿪴�ϵͳ��õĴ����б�
	do
	{
		std::vector<std::string>::iterator it;
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string  sItem = *it;

			BOOL bRet = OpenUartPort(sItem.c_str(), 115200);
			if (!bRet) {
				Clear();
				continue;
			}

			RequestPrepare();
			int ret = ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
				break;
			}
			Clear();
		}
	} while (0);

	if (bPrepared) {
		m_eStatus = STATUS_OPEN;
		m_sigStatusChange.emit(m_eStatus);
		m_sigInventoryAsyn.emit();
		return 0;
	}
	else {
		m_sigReconnect.emit(RECONNECT_TIME);
		return ERROR_FAIL;
	}

	return ERROR_FAIL;
}


// �̵�
int  C601InvReader::Inventory() {

	if (m_eStatus == STATUS_CLOSE) {
		return ERROR_DISCONNECTED;
	}

	int ret = 0;
	do 
	{
		ret = RequestInventory();
		if (0 != ret) {
			break;
		}

		ret = ReadInventoryRet();
		if (0 != ret) {
			break;
		}

	} while ( 0 );

	if (m_eStatus == STATUS_CLOSE) {
		m_sigStatusChange.emit(m_eStatus);
		m_sigReconnect.emit(RECONNECT_TIME);
	}

	if (0 == ret) {
		m_sigInventoryAsyn.emit();
	}

	return 0;
}






// Э�鲿��
int   C601InvReader::ReadToken(Token601 & t) {
	if (m_received_data.GetDataLength() < 3) {
		return -1;
	}

	BYTE buf[8192];
	m_received_data.Read(buf, 3);
	// ���ǰ�����ֽڲ��� dd 11 ef
	if (!(buf[0] == 0xdd && buf[1] == 0x11 && buf[2] == 0xef)) {
		return -1;
	}

	// ���ݳ���
	m_received_data.Read(&t.byDataLen, 1);

	// ������ݳ��Ȳ��ԣ����߳���Token�������󳤶�
	if (t.byDataLen < 8) {
		return -1;
	}

	if ((t.byDataLen - 8) > MAX_TOKEN_LENGTH) {
		return -2;
	}
	t.byDataLen -= 8;

	// ͨ����ַ
	if (m_received_data.GetDataLength() == 0) {
		return -1;
	}
	m_received_data.Read(&t.byChannel, 1);

	// ������
	if (m_received_data.GetDataLength() == 0) {
		return -1;
	}
	m_received_data.Read(&t.byOperation, 1);

	// ����
	if (m_received_data.GetDataLength() < t.byDataLen) {
		return -1;
	}
	m_received_data.Read(t.abyData, (DWORD)t.byDataLen);

	// ���2�ֽ�У����
	if (m_received_data.GetDataLength() < 2) {
		return -1;
	}
	m_received_data.Read(buf, 2);
	m_received_data.Reform();

	return 0;
}

// prepare
int   C601InvReader::RequestPrepare() {
	DWORD dwWrited = PREPARE_COMMAND.dwCommandLength;
	BOOL bRet = Write( PREPARE_COMMAND.abyCommand, dwWrited );
	if ( !bRet ) {
		return -1;
	}	
	return 0;
}

int   C601InvReader::ReadPrepareRet() {

	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 8192);

	Token601 t;
	int ret = ReadToken(t);
	while (0 == ret) {
		// ������������
		if (t.byOperation == 0x01) {
			// �����4�ֽ�����
			if (t.byDataLen == 4) {
				// ���״̬���ǻ�����������ȡ��ɣ������ޱ�ǩ����ȡʧ��
				if (t.abyData[0] == STATUS_NOISE || t.abyData[0] == STATUS_COMPLETE || t.abyData[0] == STATUS_NO_TAG || t.abyData[0] == STATUS_FAIL) {
					return 0;
				}
			}
		}
		ret = ReadToken(t);
	}

	return -1;
}

int   C601InvReader::RequestInventory() {

	DWORD dwWrited = INVENTORY_COMMAND.dwCommandLength;
	BOOL bRet = Write(INVENTORY_COMMAND.abyCommand, dwWrited);
	if (!bRet) {
		m_eStatus = STATUS_CLOSE;
		return -1;
	}
	return 0;
}

int   C601InvReader::ReadInventoryRet() {
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 8192);

#ifdef _DEBUG
	char buf[8192*8];
	DebugStream(buf, sizeof(buf), m_received_data.GetData(), m_received_data.GetDataLength());
	g_log->Output(ILog::LOG_SEVERITY_INFO, "RECEIVED COM DATA:\n%s\n", buf);
#endif

	Token601 t;
	int ret = ReadToken(t);
	while (0 == ret) {
		// ������������
		if (t.byOperation == 0x01) {
			// �����4�ֽ�����
			if (t.byDataLen == 4) {
				// ���״̬���ǻ�����������ȡ��ɣ������ޱ�ǩ����ȡʧ��
				if (t.abyData[0] == STATUS_NOISE || t.abyData[0] == STATUS_COMPLETE || t.abyData[0] == STATUS_NO_TAG || t.abyData[0] == STATUS_FAIL) {
					return 0;
				}
			}
			//  Tag���ݣ�ÿ�α������һ����
			else if (t.byDataLen == 11) {
				// �ų���βΪ"F0"�ı�ǩ
				if ( *(t.abyData + 3 + (8 - 1)) != 0xF0 ) {
					m_sigInvTagIetm(t.abyData + 3, 8);
				}
			}
		}
		ret = ReadToken(t);
	}

	// ��ʽ����
	m_eStatus = STATUS_CLOSE;
	return -1;
}







// ����ϵͳ������д���
static BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HKEY hKey = NULL;
	vCom.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	char valuename[200], databuffer[200];
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename, &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer, &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		std::string com_name = databuffer;
		vCom.push_back(com_name);

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);
	return TRUE;
}

// ���ַ���תΪ�ֽ�
static int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd) {
	if (0 == szCmd) {
		return -1;
	}

	SplitString split;
	split.SplitByBlankChars(szCmd);
	DWORD dwSize = split.Size();
	if (dwSize >= sizeof(cmd.abyCommand)) {
		return -1;
	}

	for (DWORD i = 0; i < dwSize; i++) {
		int  tmp = 0;
		sscanf_s(split[i], "%x", &tmp);
		cmd.abyCommand[i] = (BYTE)tmp;
	}
	cmd.dwCommandLength = dwSize;

	return 0;
}