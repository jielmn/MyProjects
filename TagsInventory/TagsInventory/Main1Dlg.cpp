// Main1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TagsInventory.h"
#include "Main1Dlg.h"
#include "afxdialogex.h"


void CDuiFrameWndP::InitWindow()
{

}



// CMain1Dlg �Ի���

IMPLEMENT_DYNAMIC(CMain1Dlg, CDialogEx)

CMain1Dlg::CMain1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAIN1, pParent)
{

}

CMain1Dlg::~CMain1Dlg()
{
}

void CMain1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMain1Dlg, CDialogEx)
END_MESSAGE_MAP()


// CMain1Dlg ��Ϣ�������
BOOL CMain1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}