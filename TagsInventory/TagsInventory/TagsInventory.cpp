
// TagsInventory.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TagsInventory.h"
#include "TagsInventoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTagsInventoryApp

BEGIN_MESSAGE_MAP(CTagsInventoryApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTagsInventoryApp ����

CTagsInventoryApp::CTagsInventoryApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTagsInventoryApp ����

CTagsInventoryApp theApp;


// CTagsInventoryApp ��ʼ��

BOOL CTagsInventoryApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	RDR_LoadReaderDrivers(_T("\\Drivers"));

	g_cfg = new FileConfig();
	g_cfg->Init("inventory.cfg");

	char buf[8192];
	g_cfg->GetConfig("log verify id", buf, sizeof(buf));
	g_bLogVerifyID = GetBoolean(buf);

	g_cfg_change = new FileConfig();
	g_cfg_change->Init("last.cfg");

	g_log = new FileLog();
	g_log->Init();

	g_mydb = new CMyDatabase();
	g_myreader = new CMyReader();

	LmnToolkits::ThreadManager::GetInstance();
	g_thrd_db = new LmnToolkits::Thread();
	g_thrd_reader = new LmnToolkits::Thread();
	g_thrd_db->Start();
	g_thrd_reader->Start();

	g_handler_db = new MyMessageHandlerDb();
	g_handler_reader = new MyMessageHandlerReader();



	CTagsInventoryDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	g_thrd_db->Stop();
	g_thrd_reader->Stop();

	delete g_thrd_db;
	delete g_thrd_reader;
	delete g_handler_db;
	delete g_handler_reader;
	LmnToolkits::ThreadManager::GetInstance()->ReleaseInstance();

	g_log->Deinit();
	g_cfg_change->Deinit();
	g_cfg->Deinit();
	delete g_log;
	delete g_cfg_change;
	delete g_cfg;

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

