#include <afx.h>
#include "InvReader.h"
#include "Business.h"

#include "inc/rfidlib.h"
#include "inc/rfidlib_reader.h"
#include "inc/rfidlib_AIP_ISO15693.h"
#include "inc/rfidlib_aip_iso14443A.h"
#include "inc/rfidlib_aip_iso18000p3m3.h"




static BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = NULL;
	vCom.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	CString valuename, databuffer;
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename.GetBuffer(200), &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer.GetBuffer(200), &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		std::string com_name = databuffer;
		vCom.push_back(com_name);

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);
	return TRUE;
}

static void MY_TAG_REPORT(void *cbParam, RFID_READER_HANDLE hreader, DWORD AIPtype, DWORD TagType, DWORD AntId, BYTE uid[], WORD uidlen) {
	CBusiness * pBusiness = (CBusiness *)cbParam;

	TagItem * pItem = new TagItem;
	if (0 == pItem) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "no memory!");
		return;
	}

	if (uidlen > sizeof(pItem->abyUid)) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "too long uid!");
		delete pItem;
		return;
	}

	memcpy(pItem->abyUid, uid, uidlen);
	pItem->dwUidLen = uidlen;

	// 通知界面
	pBusiness->NotifyUiInventory(pItem);
}







CInvReader::CInvReader(CBusiness * pBusiness) {
	m_pBusiness = pBusiness;

	m_hr_reader      = 0;

	m_bNewAI         = FALSE;
	m_enISO15693     = TRUE;
	m_enAFI          = FALSE;
	m_enISO14443a    = FALSE;
	m_enISO18000p3m3 = FALSE;

	m_afi = 0;
}

CInvReader::~CInvReader() {
	Clear();
}


int CInvReader::ReconnectReader() {
	err_t iret = NO_ERR;

	std::vector<std::string>  vCom;
	GetAllSerialPortName(vCom);

	const std::string PARAMS = "RDType=M201;CommType=COM;BaudRate=38400;Frame=8E1;BusAddr=255;COMName=";

	m_eReaderStatus = STATUS_CLOSE;
	std::vector<std::string>::iterator it;
	for (it = vCom.begin(); it != vCom.end(); it++) {
		std::string  sItem = *it;
		std::string  sFullParam = PARAMS + sItem;

		iret = RDR_Open(sFullParam.c_str(), &m_hr_reader);
		// 如果OK
		if (NO_ERR == iret) {
			m_eReaderStatus = STATUS_OPEN;		

			// 盘点
			m_pBusiness->InventoryAsyn();

			break;
		}
	}

	if (m_eReaderStatus == STATUS_CLOSE) {
		// 重连
		m_pBusiness->ReconnectReaderAsyn(RECONNECT_READER_TIME);
	}
	
	// 通知界面
	m_pBusiness->NotifyUiReaderStatus(m_eReaderStatus);

	return 0;
}

void CInvReader::Clear() {

}


void CInvReader::Inventory() {
	err_t iret;
	DWORD totalCount = 0;
	iret = rfid_inventory(m_hr_reader, m_bNewAI, 0, 0, m_enISO15693,
		m_enAFI, (BYTE)m_afi,
		m_enISO14443a,
		m_enISO18000p3m3, MY_TAG_REPORT, m_pBusiness, &totalCount);
	// 如果OK
	if (iret == NO_ERR) {
		// 再来一次盘点
		m_pBusiness->InventoryAsyn();
	}
	// 如果有问题
	else {
		// 重连
		m_pBusiness->ReconnectReaderAsyn(RECONNECT_READER_TIME);

		m_eReaderStatus = STATUS_CLOSE;
		// 通知界面
		m_pBusiness->NotifyUiReaderStatus(m_eReaderStatus);
	}
}


CInvReader::READER_STATUS CInvReader::GetStatus() {
	return m_eReaderStatus;
}