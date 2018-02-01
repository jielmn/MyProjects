#include <afx.h>
#include <assert.h>
#include "BindingReader.h"
#include "mi.h"
#include "Business.h"
#include "LmnThread.h"
#include "UiMessage.h"

extern HWND  g_hWnd;

CBindingReader *  CBindingReader::pInstance = 0;
LmnToolkits::Thread  * g_thrd_binding_reader = 0;
MessageHandlerBindingReader * g_handler_binding_reader = 0;

CBindingReader::CBindingReader() {

}

CBindingReader::~CBindingReader() {

}

CBindingReader *  CBindingReader::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBindingReader;
	}

	return pInstance;
}

int CBindingReader::Init() {
	char buf[1024];
	GetCurrentDirectory(1024, buf);

	CString str;
	str.Format("%s", buf);
	HINSTANCE hInstMaster = LoadLibrary(".\\function.dll");
	if (!hInstMaster)
	{
		return ERROR_FAILED_TO_LOAD_FUNCTION_DLL;
	}

	//	(FARPROC &)API_OpenComm   = GetProcAddress(m_hInstMaster, _T("API_OpenComm"));
	//	(FARPROC &)API_CloseComm  = GetProcAddress(m_hInstMaster, _T("API_CloseComm"));
	(FARPROC &)MF_Read = GetProcAddress(hInstMaster, _T("MF_Read"));
	(FARPROC &)MF_Write = GetProcAddress(hInstMaster, _T("MF_Write"));
	(FARPROC &)MF_InitValue = GetProcAddress(hInstMaster, _T("MF_InitValue"));
	(FARPROC &)MF_Dec = GetProcAddress(hInstMaster, _T("MF_Dec"));
	(FARPROC &)MF_Inc = GetProcAddress(hInstMaster, _T("MF_Inc"));
	//	(FARPROC &)RDM_GetSnr     = GetProcAddress(m_hInstMaster, _T("RDM_GetSnr"));
	//	(FARPROC &)MF_Request     = GetProcAddress(m_hInstMaster, _T("MF_Request"));
	//	(FARPROC &)MF_Anticoll    = GetProcAddress(m_hInstMaster, _T("MF_Anticoll"));
	//	(FARPROC &)MF_Select      = GetProcAddress(m_hInstMaster, _T("MF_Select"));
	(FARPROC &)MF_Halt = GetProcAddress(hInstMaster, _T("MF_Halt"));

	//	(FARPROC &)MF_Restore           = GetProcAddress(m_hInstMaster, _T("MF_Restore"));
	(FARPROC &)ControlLED = GetProcAddress(hInstMaster, _T("ControlLED"));
	(FARPROC &)ControlBuzzer = GetProcAddress(hInstMaster, _T("ControlBuzzer"));
	(FARPROC &)MF_Getsnr = GetProcAddress(hInstMaster, _T("MF_Getsnr"));
	//	(FARPROC &)GetVersionNum        = GetProcAddress(m_hInstMaster, _T("GetVersionNum"));
	//	(FARPROC &)API_SetDeviceAddress = GetProcAddress(m_hInstMaster, _T("API_SetDeviceAddress"));
	(FARPROC &)SetSerNum = GetProcAddress(hInstMaster, _T("SetSerNum"));
	(FARPROC &)GetSerNum = GetProcAddress(hInstMaster, _T("GetSerNum"));

	(FARPROC &)TypeB_Request = GetProcAddress(hInstMaster, _T("TypeB_Request"));
	(FARPROC &)TYPEB_SFZSNR = GetProcAddress(hInstMaster, _T("TYPEB_SFZSNR"));

	(FARPROC &)ISO15693_Inventory = GetProcAddress(hInstMaster, _T("ISO15693_Inventory"));
	(FARPROC &)ISO15693_Read = GetProcAddress(hInstMaster, _T("ISO15693_Read"));
	(FARPROC &)ISO15693_Write = GetProcAddress(hInstMaster, _T("ISO15693_Write"));

	(FARPROC &)UL_Request = GetProcAddress(hInstMaster, _T("UL_Request"));
	(FARPROC &)UL_HLRead = GetProcAddress(hInstMaster, _T("UL_HLRead"));
	(FARPROC &)UL_HLWrite = GetProcAddress(hInstMaster, _T("UL_HLWrite"));

	if (MF_Read == NULL ||
		MF_Write == NULL ||
		MF_InitValue == NULL ||
		MF_Dec == NULL ||
		MF_Inc == NULL ||
		MF_Getsnr == NULL ||
		SetSerNum == NULL ||
		GetSerNum == NULL ||
		ControlLED == NULL ||
		ControlBuzzer == NULL ||
		TypeB_Request == NULL ||
		TYPEB_SFZSNR == NULL ||
		ISO15693_Inventory == NULL ||
		ISO15693_Read == NULL ||
		ISO15693_Write == NULL ||
		UL_Request == NULL ||
		MF_Halt == NULL ||
		UL_HLRead == NULL ||
		UL_HLWrite == NULL)
	{
		return ERROR_FAILED_TO_LOAD_FUNCTION_DLL;
	}
	return 0;
}

int  CBindingReader::Inventory(TagId * pId) {

	int   ret;
	BYTE  cardnumber[100];
	BYTE  pBuffer[100];

	ret = ISO15693_Inventory(cardnumber, pBuffer);
	if (1 == ret) {
		ret = ERROR_NOT_FOUND_TAG;
	}
	else if (2 == ret) {
		ret = ERROR_BINDING_READER_FAILED_TO_INVENTORY;
	}
	else if ( 0 != ret ) {
		ret = -1;
	}
	else {
		int card = cardnumber[0];

		if (card >= 2) {
			return ERROR_BINDING_READER_FAILED_TOO_MANY_CARDS;
		}

		int nSize = sizeof(pId->Id);
		for (int i = 0; i < nSize; i++) {
			pId->Id[i] = *(pBuffer + 2 + (nSize - 1 - i));
		}
	}

	return ret;
}

void  CBindingReader::NotifyInventory(int nResult, const TagId * pId) {
	assert(pId);
	if (g_hWnd != 0) {
		TagId * pNewId = new TagId;
		memcpy(pNewId, pId, sizeof(TagId));		
		::PostMessage(g_hWnd, UM_INVENTORY_RET, nResult, (LPARAM)pNewId);
	}
		
}

MessageHandlerBindingReader::MessageHandlerBindingReader() {

}

MessageHandlerBindingReader::~MessageHandlerBindingReader() {

}

void MessageHandlerBindingReader::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId) {

	case MSG_INVENTORY:
	{
		TagId  tTagId;
		memset(&tTagId, 0, sizeof(tTagId));
		int ret = CBindingReader::GetInstance()->Inventory(&tTagId);
		// 通知UI界面
		CBindingReader::GetInstance()->NotifyInventory(ret, &tTagId);

		// 如果是成功的盘点，则在下次盘点时留下足够长的时间
		DWORD  dwRelayTime = DELAY_INVENTORY_TIME;

		if (0 == ret) {
			dwRelayTime = 2000;
		}

		// 再次盘点
		//g_thrd_binding_reader->PostDelayMessage(DELAY_INVENTORY_TIME, g_handler_binding_reader, MSG_INVENTORY);
		g_thrd_binding_reader->PostDelayMessage(dwRelayTime, g_handler_binding_reader, MSG_INVENTORY);
	}
	break;

	default:
		break;
	}
}

