
// PrintBarcode.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPrintBarcodeApp: 
// �йش����ʵ�֣������ PrintBarcode.cpp
//

class CPrintBarcodeApp : public CWinApp
{
public:
	CPrintBarcodeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPrintBarcodeApp theApp;