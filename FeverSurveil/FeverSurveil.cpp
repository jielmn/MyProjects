
// FeverSurveil.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FeverSurveil.h"
#include "FeverSurveilDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFeverSurveilApp

BEGIN_MESSAGE_MAP(CFeverSurveilApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFeverSurveilApp ����

CFeverSurveilApp::CFeverSurveilApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFeverSurveilApp ����

CFeverSurveilApp theApp;


// CFeverSurveilApp ��ʼ��

BOOL CFeverSurveilApp::InitInstance()
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


	g_cfg = new FileConfig();
	g_cfg->Init("FeverSurveil.cfg");

	g_log = new FileLog();
	g_log->Init("FeverSurveil.log");

	char buf[8192];
	g_cfg->GetConfig("sync command", buf, sizeof(buf), "55 01 03 dd aa");
	TransferReaderCmd(SYNC_COMMAND, buf);

	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "55 01 01 dd aa");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("clear command", buf, sizeof(buf), "55 01 04 dd aa");
	TransferReaderCmd(CLEAR_COMMAND, buf);

	
	g_db = new CMyDatabase();
	g_serial_port = new CSerialPort();

	g_handler = new MyMessageHandler();

	LmnToolkits::ThreadManager::GetInstance();
	g_thrd_db = new LmnToolkits::Thread();
	g_thrd_serial_port = new LmnToolkits::Thread();
	g_thrd_db->Start();
	g_thrd_serial_port->Start();




	CFeverSurveilDlg dlg;
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

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	g_thrd_db->Stop();
	g_thrd_serial_port->Stop();

	delete g_thrd_db;
	delete g_thrd_serial_port;

	LmnToolkits::ThreadManager::GetInstance()->ReleaseInstance();

	delete g_db;

	delete g_handler;

	g_serial_port->Deinit();
	delete g_serial_port;

	g_cfg->Deinit();
	g_log->Deinit();

	delete g_cfg;
	delete g_log;

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

