/*
* File:         SRAPIV2.H
* Created:      03/09/2001
* Description:  The Function Prototype Header File for SRAPIV2.DLL
*					 (The API for Serial Reader (SR100, SR120, SR950 .. etc)
*
*****************************************************************************
*
* Version | Date     | Description
* --------+----------+------------------------------------------------
* V1.02   | 03/12/01 | Date type of the parameter Value in the MIFARE

*********************************************************************************/

/******************* System Functions ******************************/

//1. Set communciation baudrate.
HANDLE (WINAPI * API_OpenComm)(char *com, int Baudrate);

//2. To close communication port.
int (WINAPI * API_CloseComm)(HANDLE commHandle);


/******************* MIFARE High Level Functions ******************************/
//int (WINAPI * MF_Read)(int mode, int blk_add, int num_blk, BYTE *snr, BYTE *buffer);
int (WINAPI * MF_Read)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);

int (WINAPI * MF_Write)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);

//int (WINAPI * API_PCDInitVal)(int DeviceAddress, unsigned char mode, unsigned char SectNum, unsigned char *snr, int value);
int (WINAPI * MF_InitValue)(int mode, int SectNum, BYTE *snr, BYTE *value);
      
//int (WINAPI * API_PCDDec)(int DeviceAddress, unsigned char mode, unsigned char SectNum, unsigned char *snr, int *value);
int (WINAPI * MF_Dec)(int mode, int SectNum, BYTE *snr, BYTE *value);

//int (WINAPI * API_PCDInc)(int DeviceAddress, unsigned char mode, unsigned char SectNum, unsigned char *snr, int *value);
int (WINAPI * MF_Inc)(int mode, int SectNum, BYTE *snr, BYTE *value);

int (WINAPI * RDM_GetSnr)(HANDLE commHandle, int deviceAddress, unsigned char *pCardNo);

// REQA发送寻卡指令.
int (WINAPI * MF_Request)(HANDLE commHandle, int DeviceAddress, unsigned char inf_mode, unsigned char *Buffer);

// Select 选择卡
int (WINAPI * MF_Select)(HANDLE commHandle, int DeviceAddress, unsigned char *snr);

// Halt 中断卡 选择卡，使卡进入被中断的状态…
//int (WINAPI * MF_Halt)(HANDLE commHandle, int DeviceAddress);
int (WINAPI * MF_Halt)();

// Anticoll 防冲突
int (WINAPI * MF_Anticoll)(HANDLE commHandle, int DeviceAddress, unsigned char *snr, unsigned char &status) ;

int (WINAPI * MF_Restore)(HANDLE commHandle, int DeviceAddress, unsigned char mode, 
						  unsigned char cardlength, unsigned char *carddata );

//int (WINAPI * MF_Getsnr)(unsigned char mode, unsigned char halt, unsigned char  *snr, unsigned char *value); //new
int (WINAPI * MF_Getsnr)(int mode, int halt, BYTE *snr, BYTE *value); //new

int (WINAPI * ControlBuzzer)(int freq, int duration, BYTE *buffer);

int (WINAPI * ControlLED)(int freq, int duration, BYTE *buffer);

int (WINAPI * GetVersionNum)(int DeviceAddress, char *VersionNum);

int (WINAPI * API_SetDeviceAddress)(HANDLE commHandle, int DeviceAddress, unsigned char newAddr, unsigned char *buffer);

int (WINAPI * SetSerNum)(BYTE *newValue, BYTE *buffer);

int (WINAPI * GetSerNum)(BYTE *buffer);

int (WINAPI * TypeB_Request)(BYTE *buffer); // new

int (WINAPI * TYPEB_SFZSNR)(int mode, int halt, BYTE *snr, BYTE *value); // new

int (WINAPI * UL_Request)(int mode, BYTE *snr); // new

int (WINAPI * UL_HLRead)(int mode, int blk_add, BYTE *snr, BYTE *buffer); // new

int (WINAPI *UL_HLWrite)(int mode, int blk_add, BYTE *snr, BYTE *buffer); // new

int (WINAPI * ISO15693_Inventory)(BYTE *Cardnumber, BYTE *pBuffer);

int (WINAPI * ISO15693_Read)(int flags, int blk_add, int num_blk, BYTE *uid, BYTE *buffer);

int (WINAPI * ISO15693_Write)(int flags, int blk_add, int num_blk, BYTE *uid, BYTE *data);
