#ifndef __RFIDLIB_DRV_M60_H__
#define __RFIDLIB_DRV_M60_H__




DWORD RFIDLIB_API M60_GetLibVersion(LPTSTR buf ,DWORD nSize ) ;
err_t RFIDLIB_API M60_Open(LPCTSTR comname,DWORD baud,LPCTSTR frame,BYTE busAddr,RFID_READER_HANDLE *o_hr /* out parameter */)   ;


















#endif
