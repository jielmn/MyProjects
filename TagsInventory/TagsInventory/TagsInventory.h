
// TagsInventory.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTagsInventoryApp: 
// �йش����ʵ�֣������ TagsInventory.cpp
//

class CTagsInventoryApp : public CWinApp
{
public:
	CTagsInventoryApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTagsInventoryApp theApp;