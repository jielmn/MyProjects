
// MyEncryptTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyEncryptToolApp: 
// �йش����ʵ�֣������ MyEncryptTool.cpp
//

class CMyEncryptToolApp : public CWinApp
{
public:
	CMyEncryptToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyEncryptToolApp theApp;