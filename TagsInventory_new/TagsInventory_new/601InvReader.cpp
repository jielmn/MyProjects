#include <afx.h>
#include <vector>
#include <string>
#include "LmnCommon.h"
#include "LmnString.h"
#include "610InvReader.h"

#define  RECONNECT_TIME                         10000

#define  MAX_READER_COMMAND_LENGTH              256
// Reader通信协议命令
typedef struct tagReaderCmd {
	BYTE      abyCommand[MAX_READER_COMMAND_LENGTH];
	DWORD     dwCommandLength;
}ReaderCmd;


//境噪音，读取完成，场内无标签，读取失败
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

		// 如果再无数据了
		if (0 == dwReadLen) {
			break;
		}

		// 如果已经读到最大数据(防止死循环)
		if (0 == dwLeft) {
			break;
		}
		
	} while (TRUE);
}

// 重连
int  C601InvReader::Reconnect() {

	// 如果已经连接上
	if ( m_eStatus == STATUS_OPEN ) {
		return 0;
	}

	std::vector<std::string>  vCom;
	// 获取所有的串口信息
	GetAllSerialPortName(vCom);

	// 先关闭之前可能打开的串口
	Clear();


	BOOL  bPrepared = FALSE;
	// 尝试打开从系统获得的串口列表
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


// 盘点
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






// 协议部分
int   C601InvReader::ReadToken(Token601 & t) {
	if (m_received_data.GetDataLength() < 3) {
		return -1;
	}

	BYTE buf[8192];
	m_received_data.Read(buf, 3);
	// 如果前三个字节不是 dd 11 ef
	if (!(buf[0] == 0xdd && buf[1] == 0x11 && buf[2] == 0xef)) {
		return -1;
	}

	// 数据长度
	m_received_data.Read(&t.byDataLen, 1);

	// 如果数据长度不对，或者超过Token定义的最大长度
	if (t.byDataLen < 8) {
		return -1;
	}

	if ((t.byDataLen - 8) > MAX_TOKEN_LENGTH) {
		return -2;
	}
	t.byDataLen -= 8;

	// 通道地址
	if (m_received_data.GetDataLength() == 0) {
		return -1;
	}
	m_received_data.Read(&t.byChannel, 1);

	// 命令字
	if (m_received_data.GetDataLength() == 0) {
		return -1;
	}
	m_received_data.Read(&t.byOperation, 1);

	// 数据
	if (m_received_data.GetDataLength() < t.byDataLen) {
		return -1;
	}
	m_received_data.Read(t.abyData, (DWORD)t.byDataLen);

	// 最后2字节校验码
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
		// 如果是清点命令
		if (t.byOperation == 0x01) {
			// 如果是4字节数据
			if (t.byDataLen == 4) {
				// 如果状态字是环境噪音，读取完成，场内无标签，读取失败
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
		// 如果是清点命令
		if (t.byOperation == 0x01) {
			// 如果是4字节数据
			if (t.byDataLen == 4) {
				// 如果状态字是环境噪音，读取完成，场内无标签，读取失败
				if (t.abyData[0] == STATUS_NOISE || t.abyData[0] == STATUS_COMPLETE || t.abyData[0] == STATUS_NO_TAG || t.abyData[0] == STATUS_FAIL) {
					return 0;
				}
			}
			//  Tag数据（每次保存最后一个）
			else if (t.byDataLen == 11) {
				// 排除结尾为"F0"的标签
				if ( *(t.abyData + 3 + (8 - 1)) != 0xF0 ) {
					m_sigInvTagIetm(t.abyData + 3, 8);
				}
			}
		}
		ret = ReadToken(t);
	}

	// 格式不对
	m_eStatus = STATUS_CLOSE;
	return -1;
}







// 遍历系统里的所有串口
static BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: 在此添加控件通知处理程序代码
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

// 把字符串转为字节
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