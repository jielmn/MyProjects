
// FeverSurveil.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFeverSurveilApp: 
// �йش����ʵ�֣������ FeverSurveil.cpp
//

class CFeverSurveilApp : public CWinApp
{
public:
	CFeverSurveilApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFeverSurveilApp theApp;