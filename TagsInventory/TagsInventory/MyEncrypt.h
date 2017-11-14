#pragma once

char * MyEncrypt( const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
int    MyDecrypt( const char * szSrc, void * pDest, DWORD & dwDestSize);