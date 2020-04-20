#include "PatientDataDlg.h"
#include "SixGridsUI.h"
#include "resource.h"

#define TIMER_TEMP_UI             1002
#define INTERVAL_TIMER_TEMP_UI    200
#define PATIENT_DLG_WIDTH         800
#define PATIENT_DLG_HEIGHT        802
#define IMAGE_MIN_WIDTH           224             // 曲线图最小宽度
#define IMAGE_PREFER_WIDTH        540             // 曲线图理想宽度

//自定义事件排序
static bool sortFun(PatientEvent * p1, PatientEvent * p2)
{
	return p1->m_time_1 < p2->m_time_2;
}

CPatientDataDlg::CPatientDataDlg() {
	m_tree = 0;
	m_switch = 0;

	memset(m_szTagId, 0, sizeof(m_szTagId));
	memset(m_szUIPName, 0, sizeof(m_szUIPName));

	m_waiting_bar = 0;
	m_bBusy = FALSE;

	memset( &m_patient_info, 0, sizeof(m_patient_info) );
	memset( m_patient_data,  0, sizeof(m_patient_data) );

	m_preview = 0;

	m_date_start = 0;
	m_date_end = 0;
	m_lay_events = 0;
	m_selected_event = -1;
	m_cur_temp = 0;
	m_instant_temp = 0;
	m_OtherData_Week = 0;
	m_img = 0;
	m_layImg = 0;

	m_out_hospital_time = 0;
	m_btnPrev = 0;
	m_btnNext = 0;
	m_btnZoomout = 0;
	m_btnZoomin = 0;

	m_lblPageIndex = 0;
}

CPatientDataDlg::~CPatientDataDlg() {
	ClearVector(m_vEvents);
	ClearVector(m_VTemp);
}

void   CPatientDataDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();
	CDuiString strText;

	if (msg.sType == "windowinit") {
		OnMyInited();
	}
	else if (msg.sType == "click") {
		if ( name == "btnPrintPreview" ) {
			OnPrintPreview();
		}
		else if (name == "btnReturn") {
			OnReturn();
		}
		else if (name == "btnPrint") {
			OnPrint();   
		}
		else if (name == "btnZoomIn") {
			m_preview->ZoomIn();
		}
		else if (name == "btnZoomOut") {
			m_preview->ZoomOut();
		}
		else if (name == "btnAddEvent") {
			OnAddMyEvent(new CMyEventUI);
		}
		else if (name == "btnDelEvent") {
			OnDelMyEvent();
		}
		else if (name == "btnAuto") {
			OnAutoTemp();
		}
		else if ( name == "btnPrev" ) {
			OnPrevPage();
		}
		else if ( name == "btnNext" ) {
			OnNextPage();
		}
		else if (name == "btnPrev_1") {
			OnPrevPage_1();
		}
		else if (name == "btnNext_1") {
			OnNextPage_1();
		}
		else {
			if ( msg.pSender->GetClass() == "MyEvent" ) {
				OnMyEventSelected(msg.pSender);
			}
		}
	}
	else if (msg.sType == "textchanged") {
		if (name == "DateTimeS") {
			OnDateStartChanged();			
		}
	}
	else if (msg.sType == "killfocus") {
		if (name == "DateTimeS") {
			OnDateStartKillFocus();
			m_instant_temp = 0;
			::SetTimer(GetHWND(), TIMER_TEMP_UI, INTERVAL_TIMER_TEMP_UI, 0);
		}
		else if (name == "img"|| name == "layImg" || name == "layTree" ) {
			m_instant_temp = 0;
			::SetTimer(GetHWND(), TIMER_TEMP_UI, INTERVAL_TIMER_TEMP_UI, 0);
		}
	}
	else if (msg.sType == "tempui_setfocus") {
		m_instant_temp = (CTempUI *)msg.pSender;
		//strText.Format("%d-%d temp ui setfocus! \n", 
		//	m_instant_temp->GetParent()->GetParent()->GetTag(), m_instant_temp->GetTag() );
		//OutputDebugString(strText);
		::SetTimer(GetHWND(), TIMER_TEMP_UI, INTERVAL_TIMER_TEMP_UI, 0); 
	}
	else if (msg.sType == "tempui_killfocus") {
		m_instant_temp = 0;
		//OutputDebugString("tempui_killfocus \n");
		::SetTimer(GetHWND(), TIMER_TEMP_UI, INTERVAL_TIMER_TEMP_UI, 0);
	}
	else if (msg.sType == "setfocus") {
		if (name == "img" || name == "layImg" || name == "layTree" || name == "DateTimeS") {
			KillTimer(GetHWND(), TIMER_TEMP_UI);
		}
	}
	else if (msg.sType == "get_temp") {
		int t = msg.wParam;
		//strText.Format("temp = %.2f \n", t / 100.0f);
		//OutputDebugString(strText);
		if ( t > 0 && m_cur_temp) {
			m_cur_temp->SetFocusValue(t);
		}
	}
	WindowImplBase::Notify(msg);
}

void   CPatientDataDlg::InitWindow() {
	WindowImplBase::InitWindow();
}

void  CPatientDataDlg::OnMyInited() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_PATIENT_DATA_NAME);
	m_switch = (CTabLayoutUI *)m_PaintManager.FindControl("switch");
	m_waiting_bar = (CWaitingBarUI *)m_PaintManager.FindControl("waiting_bar");
	m_btnPreview = (CButtonUI *)m_PaintManager.FindControl("btnPrintPreview");
	m_btnPrint = (CButtonUI *)m_PaintManager.FindControl("btnPrint");
	m_btnReturn = (CButtonUI *)m_PaintManager.FindControl("btnReturn");
	m_preview = (CPatientDataPrintPreviewUI *)m_PaintManager.FindControl("preview");
	m_img = (CPatientImg *)m_PaintManager.FindControl("img");
	m_layImg = (CHorizontalLayoutUI *)m_PaintManager.FindControl("layImg");
	m_btnPrev = (CButtonUI *)m_PaintManager.FindControl("btnPrev");
	m_btnNext = (CButtonUI *)m_PaintManager.FindControl("btnNext");
	m_btnZoomout = (CButtonUI *)m_PaintManager.FindControl("btnZoomOut");
	m_btnZoomin = (CButtonUI *)m_PaintManager.FindControl("btnZoomIn");

	m_tree->SetSelectedItemBkColor(0xFFFFFFFF);
	m_tree->SetHotItemBkColor(0xFFFFFFFF);  

	InitInfo();
	InitData();
	
	CBusiness::GetInstance()->QueryPatientInfoAsyn(m_szTagId);
	SetBusy(TRUE);
}
   
LRESULT  CPatientDataDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if ( wParam == TIMER_TEMP_UI ) {			
			if (m_cur_temp != m_instant_temp) {
				BOOL  bSelected = FALSE;
				if (0 != m_cur_temp) {
					bSelected = TRUE;
				}
				m_cur_temp = m_instant_temp;
				// 取消
				if ( 0 == m_cur_temp ) {
					RECT r = { 0 };
					::GetWindowRect(GetHWND(), &r);
					::MoveWindow(GetHWND(), r.left, r.top, PATIENT_DLG_WIDTH, PATIENT_DLG_HEIGHT, TRUE);
					m_layImg->EnableScrollBar(false, false);					
				}
				// 选中
				else {
					int nImgWidth = 0;

					// 之前未选中
					if (!bSelected) {
						RECT r = { 0 };
						::GetWindowRect(GetHWND(), &r);
						// 如果宽度足够大
						int nDiff = g_data.m_nScreenWidth - (r.right - r.left);
						if ( nDiff > IMAGE_PREFER_WIDTH) {
							::MoveWindow(GetHWND(), r.left, r.top, PATIENT_DLG_WIDTH + IMAGE_PREFER_WIDTH, PATIENT_DLG_HEIGHT, TRUE);
							nImgWidth = IMAGE_PREFER_WIDTH;
						}
						else if (nDiff > IMAGE_MIN_WIDTH) {
							::MoveWindow(GetHWND(), r.left, r.top, PATIENT_DLG_WIDTH + nDiff, PATIENT_DLG_HEIGHT, TRUE);
							nImgWidth = nDiff;
						}
						else {
							::MoveWindow(GetHWND(), r.left, r.top, PATIENT_DLG_WIDTH + IMAGE_MIN_WIDTH, PATIENT_DLG_HEIGHT, TRUE);
							nImgWidth = IMAGE_MIN_WIDTH;
						}
						m_layImg->SetFixedWidth(nImgWidth - 5);
						m_layImg->EnableScrollBar(false, true);
					}

					int nDayIndex = m_cur_temp->GetParent()->GetParent()->GetTag(); 
					int nTimeIndex = m_cur_temp->GetTag();
					assert(nDayIndex >= 0 && nDayIndex < 7);
					assert(nTimeIndex >= 0 && nTimeIndex < 6);
					time_t tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_start->GetTime()));

					m_img->m_pVec = &m_VTemp;
					m_img->m_tStart = tFirstDay + nDayIndex * 3600 * 24 + 3600 * 4 * nTimeIndex + 3600 * 2;
					m_img->m_tEnd = m_img->m_tStart + 4 * 3600 + 1800;
					m_img->MyInvalidate(TRUE, nImgWidth - 5); 
				}
				::InvalidateRect(GetHWND(), 0, TRUE);
			}
			KillTimer(GetHWND(), wParam);
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CPatientDataDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if ( uMsg == UM_PATIENT_INFO ) {
		OnPatientInfoRet(wParam, lParam);
	}
	else if (uMsg == UM_PATIENT_DATA) {
		OnPatientDataRet(wParam, lParam);
	}
	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

DuiLib::CControlUI * CPatientDataDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYTREE_CLASS_NAME)) {
		return new CMyTreeCfgUI(160);
	}
	else if (0 == strcmp(pstrClass, "WaitingBar")) {
		return new CWaitingBarUI;
	}
	else if (0 == strcmp(pstrClass, "PatientDataPrintPreview")) {
		return new CPatientDataPrintPreviewUI;
	}
	else if (0 == strcmp(pstrClass, "MyDateTime")) {
		return new CMyDateUI;
	}
	else if (0 == strcmp(pstrClass, "PatientImage")) {
		return new CPatientImg(GetHWND());
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void   CPatientDataDlg::InitInfo() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	CMyDateUI * pDateTime = 0;
	CShiftUI * pShift = 0;
	CHorizontalLayoutUI * pHLayout = 0;
	CButtonUI * pBtn = 0;
	CControlUI * pCtl = 0;
	CMyDateTimeUI * pMyTime = 0;

	strText.Format("病人基础信息");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 姓名
	pEdit = new CEditUI;
	m_tree->AddNode("姓名", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382); 

	// 性别
	pCombo = new CComboUI;
	AddComboItem(pCombo, "(空)", 0);
	AddComboItem(pCombo, "男", 1);
	AddComboItem(pCombo, "女", 2);
	pCombo->SelectItem(0);
	pCombo->SetFixedWidth(120);
	m_tree->AddNode("性别", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	// 年龄
	pEdit = new CEditUI;
	//pEdit->SetNumberOnly(true);
	m_tree->AddNode("年龄", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 门诊号
	pEdit = new CEditUI;
	m_tree->AddNode("门诊号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 住院号
	pEdit = new CEditUI;
	m_tree->AddNode("住院号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 入院日期
	strText.Format("入院"); 
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("是否入院", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pMyTime = new CMyDateTimeUI;
	m_tree->AddNode("入院日期", pSubTitleNode, 0, pMyTime, 2, 0xFF386382);

	// 出院日期
	strText.Format("出院");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("是否出院", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pMyTime = new CMyDateTimeUI;
	m_tree->AddNode("出院日期", pSubTitleNode, 0, pMyTime, 2, 0xFF386382);

	// 科别
	pShift = new CShiftUI;    
	m_tree->AddNode("科别", pTitleNode, 0, pShift, 2, 0xFF386382, -1, -1, 30 );         

	// 病室
	pShift = new CShiftUI;
	m_tree->AddNode("病室", pTitleNode, 0, pShift, 2, 0xFF386382, -1, -1, 30);

	// 床号
	pShift = new CShiftUI;
	m_tree->AddNode("床号", pTitleNode, 0, pShift, 2, 0xFF386382, -1, -1, 30);

	// 手术事件
	strText.Format("事件");
	pHLayout = new CHorizontalLayoutUI;
	pCtl = new CControlUI;
	pHLayout->Add(pCtl);

	pBtn = new CButtonUI;
	pBtn->SetText("添加");
	pBtn->SetFixedWidth(60);
	pBtn->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtn->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtn->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtn->SetFont(1);
	pBtn->SetTextColor(0xFF386382);
	pBtn->SetName("btnAddEvent");
	pHLayout->Add(pBtn);

	pCtl = new CControlUI;
	pCtl->SetFixedWidth(10);
	pHLayout->Add(pCtl);

	pBtn = new CButtonUI;
	pBtn->SetText("删除");
	pBtn->SetFixedWidth(60);
	pBtn->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtn->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtn->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtn->SetFont(1);
	pBtn->SetTextColor(0xFF386382);
	pBtn->SetName("btnDelEvent");
	pHLayout->Add(pBtn);  
	
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pHLayout, 3, 0xFF666666, -1, -1, 32);
	m_lay_events = new CVerticalLayoutUI;
	m_lay_events->SetName("12345");
	RECT r = { 1,1,1,1 };
	m_lay_events->SetInset(r);
	m_lay_events->SetManager(&this->m_PaintManager, 0);
	m_lay_events->EnableScrollBar(true);
	m_lay_events->SetSepHeight(4);
	m_tree->AddNode("手术等事件", pSubTitleNode, 0, m_lay_events, 2, 0xFF386382, -1, -1, 99);
}

void  CPatientDataDlg::AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pElement->SetTag(tag);
	pCombo->Add(pElement);
}

void  CPatientDataDlg::InitData() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	CDateTimeUI * pDateTime = 0;
	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	CSixGridsUI * pSixGrids = 0;
	CSevenGridsUI * pSevenGrids = 0;
	CDuiString  week_days[7];
	CHorizontalLayoutUI * pLayH = 0;
	CVerticalLayoutUI * pLayV = 0;
	CControlUI * pCtl = 0;
	CLabelUI * pLabel = 0;
	CButtonUI * pBtn = 0;
	CButtonUI * pBtnPrev = 0;
	CButtonUI * pBtnNext = 0;
	RECT r = { 5,0,5,0 };
	CControlUI * pToolTip = 0;
	CVerticalLayoutUI * pLayV1 = 0;
	RECT r1 = { 10,0,0,0 };

	time_t now = time(0);
	GetSevenDayStr(week_days, 7, now - 3600 * 24 * 6);

	strText.Format("病人数据");	
	m_date_start = new CMyDateUI;
	m_date_start->SetFixedWidth(180);
	m_date_start->SetFont(3);
	m_date_start->SetTextColor(0xFF666666);
	m_date_start->SetTextPadding(r);
	m_date_start->SetName("DateTimeS");
	m_date_start->SetVisible(false);

	pCtl = new CControlUI;
	pCtl->SetFixedWidth(10);
	pCtl->SetVisible(false);
	pLabel = new CLabelUI;
	pLabel->SetText("到");
	pLabel->SetFixedWidth(60);
	pLabel->SetFont(3);
	pLabel->SetTextColor(0xFF666666);
	pLabel->SetVisible(false);

	m_date_end   = new CMyDateUI;
	m_date_end->SetFixedWidth(180);
	m_date_end->SetFont(3);
	m_date_end->SetTextColor(0xFF666666);
	m_date_end->SetTextPadding(r);
	m_date_end->SetEnabled(false);
	m_date_end->SetName("DateTimeE");
	m_date_end->SetVisible(false);

	pBtnPrev = new CButtonUI;
	pBtnPrev->SetText("上一页");
	pBtnPrev->SetFixedWidth(100);
	pBtnPrev->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtnPrev->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtnPrev->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtnPrev->SetFont(1);
	pBtnPrev->SetTextColor(0xFF386382);
	pBtnPrev->SetName("btnPrev_1");
	//pBtnPrev->SetEnabled(false);

	pBtnNext = new CButtonUI;
	pBtnNext->SetFixedWidth(100);
	pBtnNext->SetText("下一页");
	pBtnNext->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtnNext->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtnNext->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtnNext->SetFont(1);
	pBtnNext->SetTextColor(0xFF386382);
	pBtnNext->SetName("btnNext_1");
	//pBtnNext->SetEnabled(false);

	pLayH = new CHorizontalLayoutUI;
	pLayH->Add(m_date_start);
	pLayH->Add(pCtl);
	pLayH->Add(pLabel);
	pLayH->Add(m_date_end);

	pCtl = new CControlUI;
	pCtl->SetFixedWidth(100);
	pLayH->Add(pCtl);

	pLayH->Add(pBtnPrev);

	pCtl = new CControlUI;
	pCtl->SetFixedWidth(40);
	pLayH->Add(pCtl);

	pLayH->Add(pBtnNext);

	pCtl = new CControlUI;
	//pCtl->SetFixedWidth(40);
	pLayH->Add(pCtl);

	pLabel = new CLabelUI;
	pLabel->SetText("第");
	pLabel->SetFixedWidth(40);
	pLabel->SetFont(3);
	pLabel->SetTextColor(0xFF666666);
	pLabel->SetAttribute("align", "center");
	pLayH->Add(pLabel);

	pLabel = new CLabelUI;
	pLabel->SetText("0");
	pLabel->SetFixedWidth(40);
	pLabel->SetFont(3);
	pLabel->SetTextColor(0xFF666666);
	pLabel->SetAttribute("align", "center");
	pLabel->SetName("lblPageIndex_1");
	m_lblPageIndex = pLabel;
	pLayH->Add(pLabel);

	pLabel = new CLabelUI;
	pLabel->SetText("页");
	pLabel->SetFixedWidth(40);
	pLabel->SetFont(3);
	pLabel->SetTextColor(0xFF666666);
	pLabel->SetAttribute("align", "center");
	pLayH->Add(pLabel);

	pTitleNode = m_tree->AddNode(strText, 0, 0, pLayH, 3, 0xFF666666, -1, -1, 40);

	// 体温
	strText.Format("体温");
	pSixGrids = new CSixGridsUI;  
	pSixGrids->SetMode(1);
	pSixGrids->SetFixedHeight(22);

	pLayV = new CVerticalLayoutUI;
	pLayH = new CHorizontalLayoutUI;

	pCtl = new CControlUI;
	pCtl->SetFixedWidth(6);
	pCtl->SetBkColor(0xFF93F7F1);
	pLabel = new CLabelUI;
	pLabel->SetTextPadding(r);
	pLabel->SetText("展开或关闭降温措施后的体温");
	pLabel->SetFont(2);
	pLabel->SetTextColor(0xFF0000FF);
	pBtn = new CButtonUI;
	pBtn->SetFixedWidth(120);
	pBtn->SetText("自动填充");
	pBtn->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtn->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtn->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtn->SetFont(1);
	pBtn->SetTextColor(0xFF386382);
	pBtn->SetName("btnAuto");

	pToolTip = new CControlUI; 
	pToolTip->SetBkImage("question.png");
	pToolTip->SetFixedWidth(16);
	pToolTip->SetFixedHeight(16);     
	pToolTip->SetToolTip("1.自动填充会把每个时段的最高温度填进对应的温度格子<n>2.不能识别降温措施后的降温，如果要降温，请手动填写<n>3.有数据的格子不会自动填充");

	pLayV1 = new CVerticalLayoutUI;
	pLayV1->Add(new CControlUI);
	pLayV1->Add(pToolTip);
	pLayV1->Add(new CControlUI);
	pLayV1->SetFixedWidth(26);
	pLayV1->SetInset(r1);
 
	pLayH->Add(pCtl);
	pLayH->Add(pLabel);
	pLayH->Add(pBtn);
	pLayH->Add(pLayV1);

	pLayV->Add(pLayH);
	pLayH = new CHorizontalLayoutUI;
	pLayH->SetFixedHeight(3);
	pLayV->Add(pLayH);
	pLayV->Add(pSixGrids);


	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pLayV, 3, 0xFF666666, -1, -1, 60);

	for (int i = 0; i < 7; i++) {
		CSixTempUI * pTemp = new CSixTempUI;
		pTemp->SetTag(i);
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pTemp, 2, 0xFF386382, -1, -1, 30);
	}

	// 脉搏  
	strText.Format("脉搏");   
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666); 


	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30 );     
	}

	// 呼吸 
	strText.Format("呼吸");
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);		
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666);

	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		pSixGrids->SetNumberOnly(FALSE);
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30);
	}
	 
	// 其他数据
	strText.Format("其他数据");
	pSevenGrids = new CSevenGridsUI;
	pSevenGrids->SetMode(1);
	GetSevenDayStr(week_days, 7, now - 3600 * 24 * 6, TRUE);
	pSevenGrids->SetWeekStr(week_days, 7);
	m_OtherData_Week = pSevenGrids;
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSevenGrids, 3, 0xFF666666);

	const char * item_title[7] = { "大便次数", "尿量(次) ml", "总入量 ml", "总出量 ml", "血压 kpa", "体重 kg", "过敏药物" };
	for (int i = 0; i < 7; i++) {
		pSevenGrids = new CSevenGridsUI;
		pSevenGrids->SetNumberOnly(FALSE);
		if (i == 4)
			pSevenGrids->SetFont(5);    
		m_tree->AddNode(item_title[i], pSubTitleNode, 0, pSevenGrids, 2, 0xFF386382, 2, 0xFF386382, 30);    
	}
}

void CPatientDataDlg::OnPrintPreview() {
	PatientInfo info;
	std::vector<PatientEvent * > vEvents;
	PatientData data[7];

	// 获取UI的填入信息
	GetPatientInfo(&info, vEvents);
	GetPatientData(data, 7);

	memcpy(&m_preview->m_patient_info, &info, sizeof(PatientInfo));
	memcpy(m_preview->m_patient_data, data, sizeof(PatientData) * 7);
	m_preview->m_tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_start->GetTime()));
	ClearVector(m_preview->m_vEvents);
	std::copy(vEvents.begin(), vEvents.end(), std::back_inserter(m_preview->m_vEvents));	

	m_switch->SelectItem(1);
	m_preview->Invalidate();
}

void CPatientDataDlg::OnReturn() {
	m_switch->SelectItem(0);      
}

void  CPatientDataDlg::OnPrint() { 

	PatientInfo info;
	std::vector<PatientEvent * > vEvents;
	PatientData data[7];

	// 获取UI的填入信息
	GetPatientInfo(&info, vEvents);
	GetPatientData(data, 7);
	time_t tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_start->GetTime()));

	CXml2ChartFile   xmlChart;
	LoadXmlChart(xmlChart);


	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	//PrintDlg(&printInfo);
	if (PrintDlg(&printInfo) == TRUE)
	{
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");

		::SetMapMode(printInfo.hDC, MM_ANISOTROPIC);

		SIZE  size = { 760, 1044 };
		::SetWindowExtEx(printInfo.hDC, size.cx, size.cy, 0);
		size.cx = GetDeviceCaps(printInfo.hDC, HORZRES);
		size.cy = GetDeviceCaps(printInfo.hDC, VERTRES);
		::SetViewportExtEx(printInfo.hDC, size.cx, size.cy, 0);

		StartDoc(printInfo.hDC, &di);

		for (int i = 0; i < 1; i++) {
			StartPage(printInfo.hDC);

			// SetBkMode(printInfo.hDC, TRANSPARENT);
			PrepareXmlChart(xmlChart, &info, data, 7, tFirstDay, vEvents);
			PrintXmlChart(printInfo.hDC, xmlChart, 0, 0, data, 7, tFirstDay, vEvents, info);

			EndPage(printInfo.hDC);
		}

		EndDoc(printInfo.hDC);
		DeleteDC(printInfo.hDC);
	}

	ClearVector(vEvents);
}

void  CPatientDataDlg::GetSevenDayStr(CDuiString * pDays, DWORD dwSize, time_t tFirstTime, BOOL bMonthDay /*= FALSE*/) {
	assert(dwSize >= 7);
	if ( dwSize < 7 ) {
		return;
	}

	char buf[256];
	time_t tZeroTime = GetAnyDayZeroTime(tFirstTime);
	time_t tTodayZeroTime = GetTodayZeroTime();
	int nWeekDayIndex = GetWeekDay(tZeroTime);

	for (int i = 0; i < 7; i++) {
		time_t t = tZeroTime + 3600 * 24 * i;
		if (bMonthDay)
			Date2String_md(buf, sizeof(buf), &t);
		else
			Date2String(buf, sizeof(buf), &t);

		pDays[i] = buf;
		if (t == tTodayZeroTime) {
			pDays[i] += "(今天)";         
		}
		else {
			pDays[i] += "(";
			pDays[i] += GetWeekDayShortName( (nWeekDayIndex + i) % 7 );
			pDays[i] += ")";
		}
	}
}

// 从界面获取填入的info
void  CPatientDataDlg::GetPatientInfo( PatientInfo * pInfo, 
	                                   std::vector<PatientEvent * > & vEvents ) {
	memset(pInfo, 0, sizeof(PatientInfo));
	ClearVector(vEvents);

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 1;

	// 姓名
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szPName, cfgValue.m_strEdit, MAX_TAG_PNAME_LENGTH);
	nRow++;

	// 性别
	m_tree->GetConfigValue(nRow, cfgValue);
	pInfo->m_sex = cfgValue.m_nComboSel;
	nRow++;

	// 年龄
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_age, cfgValue.m_strEdit, MAX_AGE_LENGTH);
	nRow++;

	// 门诊号
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szOutpatientNo, cfgValue.m_strEdit, MAX_OUTPATIENT_NO_LENGTH);
	nRow++;

	// 住院号
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szHospitalAdmissionNo, cfgValue.m_strEdit, MAX_HOSPITAL_ADMISSION_NO_LENGTH);
	nRow++;

	// 入院日期
	m_tree->GetConfigValue(nRow+1, cfgValue);
	if ( cfgValue.m_bCheckbox ) {
		m_tree->GetConfigValue(nRow+2, cfgValue);
		pInfo->m_in_hospital = cfgValue.m_time;
	}
	else {
		pInfo->m_in_hospital = 0;
	}
	nRow += 3;

	// 出院日期
	m_tree->GetConfigValue(nRow + 1, cfgValue);
	if (cfgValue.m_bCheckbox) {
		m_tree->GetConfigValue(nRow + 2, cfgValue);
		pInfo->m_out_hospital = cfgValue.m_time;
	}
	else {
		pInfo->m_out_hospital = 0;
	}
	nRow += 3;

	// 科室
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szMedicalDepartment,  cfgValue.m_Values[0], MAX_MEDICAL_DEPARTMENT_LENGTH);
	STRNCPY(pInfo->m_szMedicalDepartment2, cfgValue.m_Values[1], MAX_MEDICAL_DEPARTMENT_LENGTH);
	nRow++;

	// 病室
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szWard, cfgValue.m_Values[0], MAX_WARD_LENGTH);
	STRNCPY(pInfo->m_szWard2, cfgValue.m_Values[1], MAX_WARD_LENGTH);
	nRow++;

	// 床号
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szBedNo, cfgValue.m_Values[0], MAX_BED_NO_LENGTH);
	STRNCPY(pInfo->m_szBedNo2, cfgValue.m_Values[1], MAX_BED_NO_LENGTH);
	nRow++;

	// 事件
	int cnt = m_lay_events->GetCount();
	for ( int i = 0; i < cnt; i++) {
		CMyEventUI * pEvent = (CMyEventUI *)m_lay_events->GetItemAt(i);

		PatientEvent * pItem = new PatientEvent;
		memset(pItem, 0, sizeof(PatientEvent));
		pEvent->GetValue( pItem->m_nId, pItem->m_nType, pItem->m_time_1, pItem->m_time_2);
		STRNCPY(pItem->m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);

		vEvents.push_back(pItem);
	}

	// 按照时间顺序排序
	sort(vEvents.begin(), vEvents.end(), sortFun);
}

// 从界面获取填入的data
void  CPatientDataDlg::GetPatientData(PatientData * pData, DWORD dwSize) {
	assert(dwSize >= 7);
	memset(pData, 0, sizeof(PatientData) * 7);

	CMyTreeCfgUI::ConfigValue   cfgValue;
	// 指向体温大栏
	int nRow = 18; 

	// 体温
	for (int i = 0; i < 7; i++) {
		m_tree->GetConfigValue(nRow + 1 + i, cfgValue);
		for (int j = 0; j < 6; j++) {
			pData[i].m_temp[j] = cfgValue.m_nValues[j][0];
			pData[i].m_descend_temp[j] = cfgValue.m_nValues[j][1];
		}
	}
	nRow += 8;


	// 脉搏
	for (int i = 0; i < 7; i++) {
		m_tree->GetConfigValue(nRow + 1 + i, cfgValue);
		for (int j = 0; j < 6; j++) {
			pData[i].m_pulse[j] = 0;
			sscanf_s(cfgValue.m_Values[j], "%d", &pData[i].m_pulse[j]);
		}		
	}
	nRow += 8;

	// 呼吸
	for (int i = 0; i < 7; i++) {
		m_tree->GetConfigValue(nRow + 1 + i, cfgValue);
		for (int j = 0; j < 6; j++) {
			STRNCPY(pData[i].m_breath[j], cfgValue.m_Values[j], MAX_BREATH_LENGTH);
		}
	}
	nRow += 9;

	// 大便次数
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_defecate, cfgValue.m_Values[i], MAX_DEFECATE_LENGTH);
	}
	nRow++;

	// 尿量
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_urine, cfgValue.m_Values[i], MAX_URINE_LENGTH);
	}
	nRow++;

	// 总入量
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_income, cfgValue.m_Values[i], MAX_INCOME_LENGTH);
	}
	nRow++;

	// 总出量
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_output, cfgValue.m_Values[i], MAX_OUTPUT_LENGTH);
	}
	nRow++;

	// 血压
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_szBloodPressure, cfgValue.m_Values[i], MAX_BLOOD_PRESSURE_LENGTH);
	}
	nRow++;

	// 体重
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_szWeight, cfgValue.m_Values[i], MAX_WEIGHT_LENGTH);		
	}
	nRow++;

	// 过敏药物
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY( pData[i].m_szIrritability, cfgValue.m_Values[i], MAX_IRRITABILITY_LENGTH );
	}
	nRow++;
}

void CPatientDataDlg::OnFinalMessage(HWND hWnd) {
	// 如果还在获取数据
	if (m_bBusy) {
		WindowImplBase::OnFinalMessage(hWnd);
		return;
	}

	PatientInfo info;
	std::vector<PatientEvent * > vEvents;
	PatientData data[7];

	GetPatientInfo(&info, vEvents);
	STRNCPY(info.m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);	
	m_out_hospital_time = info.m_out_hospital;

	// 保存姓名
	if ( 0 != strcmp(info.m_szPName, m_patient_info.m_szPName) ) {
		CBusiness::GetInstance()->SaveTagPNameAsyn(m_szTagId, info.m_szPName);
	}

	if ( IsPatientInfoChanged(&info) ) {
		// 保存数据库
		CBusiness::GetInstance()->SavePatientInfoAsyn(&info);
	}

	// 保存事件
	CBusiness::GetInstance()->SavePatientEventsAsyn(m_szTagId, vEvents);
	ClearVector(vEvents);

	GetPatientData(data, 7);
	time_t  tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_start->GetTime()));

	for (DWORD i = 0; i < 7; i++) {
		if (IsPatientDataChanged( &data[i], i ) ) {
			STRNCPY( data[i].m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH ); 
			data[i].m_date = tFirstDay + 3600 * 24 * i;
			// 保存数据库
			CBusiness::GetInstance()->SavePatientDataAsyn(&data[i]); 
		}
	}
	
	STRNCPY( m_szUIPName, info.m_szPName, MAX_TAG_PNAME_LENGTH );
	WindowImplBase::OnFinalMessage(hWnd);
}

// 病人info是否改变
BOOL  CPatientDataDlg::IsPatientInfoChanged(PatientInfo * pInfo) {

	if ( pInfo->m_sex != m_patient_info.m_sex )
		return TRUE;

	if ( 0 != strcmp(pInfo->m_age,m_patient_info.m_age) )
		return TRUE;

	if ( 0 != strcmp(pInfo->m_szOutpatientNo, m_patient_info.m_szOutpatientNo) )
		return TRUE;

	if (0 != strcmp(pInfo->m_szHospitalAdmissionNo, m_patient_info.m_szHospitalAdmissionNo))
		return TRUE;

	if (0 != strcmp(pInfo->m_szMedicalDepartment, m_patient_info.m_szMedicalDepartment))
		return TRUE;

	if (0 != strcmp(pInfo->m_szWard, m_patient_info.m_szWard))
		return TRUE;

	if (0 != strcmp(pInfo->m_szBedNo, m_patient_info.m_szBedNo))
		return TRUE;

	if (0 != strcmp(pInfo->m_szMedicalDepartment2, m_patient_info.m_szMedicalDepartment2))
		return TRUE;

	if (0 != strcmp(pInfo->m_szWard2, m_patient_info.m_szWard2))
		return TRUE;

	if (0 != strcmp(pInfo->m_szBedNo2, m_patient_info.m_szBedNo2))
		return TRUE;

	if ( pInfo->m_in_hospital != m_patient_info.m_in_hospital )
		return TRUE;

	if (pInfo->m_out_hospital != m_patient_info.m_out_hospital)
		return TRUE;

	return FALSE;
}

// 病人data是否改变
BOOL  CPatientDataDlg::IsPatientDataChanged(PatientData * pData, int nIndex) {
	assert(nIndex >= 0 && nIndex < 7);
	
	for (int i = 0; i < 6; i++) {
		// 体温
		if (pData->m_temp[i] != m_patient_data[nIndex].m_temp[i]) {
			return TRUE;
		}

		// 降温
		if (pData->m_descend_temp[i] != m_patient_data[nIndex].m_descend_temp[i]) {
			return TRUE;
		}

		// 脉搏
		if (pData->m_pulse[i] != m_patient_data[nIndex].m_pulse[i]) {
			return TRUE;
		}

		// 呼吸
		if ( 0 != strcmp(pData->m_breath[i], m_patient_data[nIndex].m_breath[i]) ) {
			return TRUE;
		}
	}

	if ( 0 != strcmp (pData->m_defecate, m_patient_data[nIndex].m_defecate) )
		return TRUE;

	if ( 0 != strcmp(pData->m_urine,m_patient_data[nIndex].m_urine) )
		return TRUE;

	if (0 != strcmp(pData->m_income, m_patient_data[nIndex].m_income) )
		return TRUE;

	if (0 != strcmp(pData->m_output, m_patient_data[nIndex].m_output) )
		return TRUE;

	if ( 0 != strcmp(pData->m_szBloodPressure, m_patient_data[nIndex].m_szBloodPressure) )
		return TRUE;

	if ( 0 != strcmp(pData->m_szWeight,m_patient_data[nIndex].m_szWeight) )
		return TRUE;

	if ( 0 != strcmp( pData->m_szIrritability, m_patient_data[nIndex].m_szIrritability) )
		return TRUE;

	return FALSE;
}

void CPatientDataDlg::SetBusy(BOOL bBusy /*= TRUE*/) {
	if (m_bBusy == bBusy)
		return;

	if ( bBusy ) {
		m_tree->SetMouseChildEnabled(false);
		m_waiting_bar->SetVisible(true);
		m_waiting_bar->Start();
		m_btnPreview->SetEnabled(false);
		m_btnPrint->SetEnabled(false);
		m_btnReturn->SetEnabled(false);
		m_btnPrev->SetEnabled(false);
		m_btnNext->SetEnabled(false);
		m_btnZoomout->SetEnabled(false);
		m_btnZoomin->SetEnabled(false);
	}
	else {
		m_tree->SetMouseChildEnabled(true);
		m_waiting_bar->Stop();
		m_waiting_bar->SetVisible(false);		
		m_btnPreview->SetEnabled(true);
		m_btnPrint->SetEnabled(true);
		m_btnReturn->SetEnabled(true);
		m_btnPrev->SetEnabled(true);
		m_btnNext->SetEnabled(true);
		m_btnZoomout->SetEnabled(true);
		m_btnZoomin->SetEnabled(true);
	}
	
	m_bBusy = bBusy;
}

void  CPatientDataDlg::OnPatientInfo(PatientInfo * pInfo, const std::vector<PatientEvent * > & vEvents) {
	PatientInfo * pNewInfo = new PatientInfo;
	memcpy(pNewInfo, pInfo, sizeof(PatientInfo));

	std::vector<PatientEvent * > * pVec = new std::vector<PatientEvent * >;
	std::vector<PatientEvent * >::const_iterator it;
	for (it = vEvents.begin(); it != vEvents.end(); ++it) {
		PatientEvent * pEvent = *it;
		PatientEvent * pNewEvent = new PatientEvent;
		memcpy(pNewEvent, pEvent, sizeof(PatientEvent));
		pVec->push_back(pNewEvent);
	}

	::PostMessage(GetHWND(), UM_PATIENT_INFO, (WPARAM)pNewInfo, (LPARAM)pVec);
}

void  CPatientDataDlg::OnPatientData(PatientData * pData, DWORD dwSize, const std::vector<TempItem *> & vTemp) {
	assert(dwSize >= 7);

	PatientData * pNewData = new PatientData[7];
	memcpy(pNewData, pData, sizeof(PatientData) * 7);

	std::vector<TempItem *> * pVTemp = new std::vector<TempItem *>;
	std::vector<TempItem *>::const_iterator it;
	for (it = vTemp.begin(); it != vTemp.end(); ++it) {
		TempItem * pTemp = *it;
		TempItem * pNewTemp = new TempItem;
		memcpy(pNewTemp, pTemp, sizeof(TempItem));
		pVTemp->push_back(pNewTemp);
	}

	::PostMessage(GetHWND(), UM_PATIENT_DATA, (WPARAM)pNewData, (LPARAM)pVTemp);
}

void  CPatientDataDlg::OnPatientInfoRet(WPARAM wParam, LPARAM  lParam) {
	PatientInfo * pInfo = (PatientInfo *)wParam;
	memcpy(&m_patient_info, pInfo, sizeof(PatientInfo));
	delete pInfo;

	std::vector<PatientEvent * > * pVec = (std::vector<PatientEvent * > *)lParam;
	std::vector<PatientEvent * >::const_iterator it;
	ClearVector(m_vEvents);
	for (it = pVec->begin(); it != pVec->end(); ++it) {
		PatientEvent * pEvent = *it;
		m_vEvents.push_back(pEvent);
	}
	delete pVec;

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 1;

	// 姓名
	cfgValue.m_strEdit = m_patient_info.m_szPName;
	if (cfgValue.m_strEdit.GetLength() == 0)
		cfgValue.m_strEdit = m_szUIPName;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 性别
	cfgValue.m_nComboSel = m_patient_info.m_sex;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 年龄
	cfgValue.m_strEdit = m_patient_info.m_age;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 门诊号
	cfgValue.m_strEdit = m_patient_info.m_szOutpatientNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 住院号
	cfgValue.m_strEdit = m_patient_info.m_szHospitalAdmissionNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;
	
	// 入院日期
	if ( m_patient_info.m_in_hospital > 0 ) {
		cfgValue.m_bCheckbox = TRUE;
		m_tree->SetConfigValue(nRow+1, cfgValue);

		cfgValue.m_time = m_patient_info.m_in_hospital;
		m_tree->SetConfigValue(nRow+2, cfgValue);
	}
	else {
		cfgValue.m_bCheckbox = FALSE;
		m_tree->SetConfigValue(nRow+1, cfgValue);
	}
	nRow+=3;

	// 出院日期
	if (m_patient_info.m_out_hospital > 0) {
		cfgValue.m_bCheckbox = TRUE;
		m_tree->SetConfigValue(nRow + 1, cfgValue);

		cfgValue.m_time = m_patient_info.m_out_hospital;
		m_tree->SetConfigValue(nRow + 2, cfgValue);
	}
	else {
		cfgValue.m_bCheckbox = FALSE;
		m_tree->SetConfigValue(nRow + 1, cfgValue);
	}
	nRow += 3;

	// 科室
	cfgValue.m_Values[0] = m_patient_info.m_szMedicalDepartment;
	cfgValue.m_Values[1] = m_patient_info.m_szMedicalDepartment2;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 病室
	cfgValue.m_Values[0] = m_patient_info.m_szWard;
	cfgValue.m_Values[1] = m_patient_info.m_szWard2;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 床号
	cfgValue.m_Values[0] = m_patient_info.m_szBedNo;
	cfgValue.m_Values[1] = m_patient_info.m_szBedNo2;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 事件
	for (it = m_vEvents.begin(); it != m_vEvents.end(); ++it) {
		PatientEvent * pItem = *it;
		CMyEventUI * pUI = new CMyEventUI;
		pUI->SetValue(pItem->m_nId, pItem->m_nType, pItem->m_time_1, pItem->m_time_2);
		OnAddMyEvent(pUI);
	}

	time_t tFirstDay = 0;
	// 如果有住院日期
	//if ( m_patient_info.m_in_hospital > 0 ) {
	//	// 如果没有出院日期
	//	if (m_patient_info.m_out_hospital == 0) {
	//		time_t  tHospital = GetAnyDayZeroTime(m_patient_info.m_in_hospital);
	//		time_t  tTodayZero = GetTodayZeroTime();
	//		if (tTodayZero < tHospital) {
	//			tFirstDay = tTodayZero;
	//		}
	//		// 差距小于一周
	//		else if (tTodayZero - tHospital < 3600 * 24 * 6) {
	//			tFirstDay = tHospital;
	//		}
	//		// 差距大于一周
	//		else {
	//			tFirstDay = tTodayZero - 3600 * 24 * 6;
	//		}
	//	}
	//	// 有出院日期
	//	else {
	//		tFirstDay = GetAnyDayZeroTime(m_patient_info.m_in_hospital);
	//	}		
	//}
	//else {
	//	tFirstDay = GetTodayZeroTime();
	//}

	CDuiString  strText;
	// 如果有住院日期
	if (m_patient_info.m_in_hospital > 0) {
		// 如果没有出院日期
		if (m_patient_info.m_out_hospital == 0) {
			time_t  tHospital  = GetAnyDayZeroTime(m_patient_info.m_in_hospital);
			time_t  tTodayZero = GetTodayZeroTime();
			if (tTodayZero <= tHospital) {
				tFirstDay = tHospital;
				m_lblPageIndex->SetText("1");
			}
			else {
				time_t  diff   = tTodayZero - tHospital;
				int nPageIndex = (int)diff / (86400 * 7) + 1;
				tFirstDay = tHospital + 86400 * 7 * (nPageIndex - 1);
				strText.Format("%d", nPageIndex);
				m_lblPageIndex->SetText(strText);
			}
		}
		// 有出院日期
		else {
			tFirstDay = GetAnyDayZeroTime(m_patient_info.m_in_hospital);
			m_lblPageIndex->SetText("1");
		}
	}
	// 没有住院日期
	else {
		tFirstDay = GetTodayZeroTime();
		m_lblPageIndex->SetText("");
	}

	SYSTEMTIME s = Time2SysTime(tFirstDay);
	m_date_start->SetMyTime(&s);
	s = Time2SysTime(tFirstDay + 3600 * 24 * 6);
	m_date_end->SetMyTime(&s);

	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);
}

void  CPatientDataDlg::OnPatientDataRet(WPARAM wParam, LPARAM  lParam) {
	PatientData * pData = (PatientData *)wParam;
	memcpy(m_patient_data, pData, sizeof(PatientData) * 7);
	delete[] pData;

	ClearVector(m_VTemp);
	std::vector<TempItem *> * pVTemp = (std::vector<TempItem *> *)lParam;
	if (pVTemp) {
		std::vector<TempItem *>::iterator it;
		for (it = pVTemp->begin(); it != pVTemp->end(); ++it) {
			m_VTemp.push_back(*it);
		}
		pVTemp->clear();
		delete pVTemp;
	}

	if (m_cur_temp) {
		int nDayIndex = m_cur_temp->GetParent()->GetParent()->GetTag();
		int nTimeIndex = m_cur_temp->GetTag();
		assert(nDayIndex >= 0 && nDayIndex < 7);
		assert(nTimeIndex >= 0 && nTimeIndex < 6);
		time_t tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_start->GetTime()));

		m_img->m_pVec = &m_VTemp;
		m_img->m_tStart = tFirstDay + nDayIndex * 3600 * 24 + 3600 * 4 * nTimeIndex;
		m_img->m_tEnd = m_img->m_tStart + 4 * 3600 + 1800;
		m_img->MyInvalidate();
	}
	
	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 18;

	CDuiString  week_days[7];
	GetSevenDayStr(week_days, 7, m_patient_data[0].m_date);

	// 体温
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			cfgValue.m_nValues[j][0] = m_patient_data[i].m_temp[j];
			cfgValue.m_nValues[j][1] = m_patient_data[i].m_descend_temp[j];
		}
		m_tree->SetConfigValue(nRow + 1 + i, cfgValue);
		m_tree->SetTitle(nRow + 1 + i, week_days[i]);
	}
	nRow += 8;

	
	// 脉搏
	for (int i = 0; i < 7; i++) {		
		for (int j = 0; j < 6; j++) {
			if (m_patient_data[i].m_pulse[j] > 0)
				cfgValue.m_Values[j].Format("%d", m_patient_data[i].m_pulse[j]);
			else
				cfgValue.m_Values[j] = "";
		}
		m_tree->SetConfigValue(nRow + 1 + i, cfgValue);
		m_tree->SetTitle(nRow + 1 + i, week_days[i]);
	}
	nRow += 8;

	// 呼吸
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			cfgValue.m_Values[j] = m_patient_data[i].m_breath[j];
		}
		m_tree->SetConfigValue(nRow + 1 + i, cfgValue);
		m_tree->SetTitle(nRow + 1 + i, week_days[i]);
	}
	nRow += 9;	

	// 大便次数
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_defecate;		
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 尿量
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_urine;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 总入量
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_income;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 总出量
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_output;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 血压
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_szBloodPressure;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 体重
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_szWeight;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 过敏药物
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_szIrritability;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	GetSevenDayStr(week_days, 7, m_patient_data[0].m_date, TRUE);
	m_OtherData_Week->SetWeekStr(week_days, 7);   

	SetBusy(FALSE);

	if (m_switch->GetCurSel() == 1) {
		PatientInfo info;
		std::vector<PatientEvent * > vEvents;
		PatientData data[7];

		// 获取UI的填入信息
		GetPatientInfo(&info, vEvents);
		GetPatientData(data, 7);

		memcpy(&m_preview->m_patient_info, &info, sizeof(PatientInfo));
		memcpy(m_preview->m_patient_data, data, sizeof(PatientData) * 7);
		m_preview->m_tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_start->GetTime()));
		ClearVector(m_preview->m_vEvents);
		std::copy(vEvents.begin(), vEvents.end(), std::back_inserter(m_preview->m_vEvents));
		m_preview->Invalidate();
	}
}

// 时间范围的起始时间改变
void  CPatientDataDlg::OnDateStartChanged() {
	//CDuiString  str; 
	//SYSTEMTIME s = m_date_start->GetTime();
	//str.Format("%d-%d-%d %d:%d:%d \n", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond);
	//::OutputDebugString(str);
}

// 时间控件失去焦点
void  CPatientDataDlg::OnDateStartKillFocus() {

	// 先保存数据
	PatientData data[7];
	GetPatientData(data, 7);
	time_t  tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_end->GetTime())) - 3600 * 24 * 6;

	for (DWORD i = 0; i < 7; i++) {
		if (IsPatientDataChanged(&data[i], i)) {
			STRNCPY(data[i].m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);
			data[i].m_date = tFirstDay + 3600 * 24 * i;
			// 保存数据库
			CBusiness::GetInstance()->SavePatientDataAsyn(&data[i]);
		}
	}
	// END 保存数据



	SYSTEMTIME s = m_date_start->GetTime();
	tFirstDay = SysTime2Time(s);
	time_t tEndDay = tFirstDay + 3600 * 24 * 6;
	SYSTEMTIME s1 = Time2SysTime(tEndDay);
	m_date_end->SetMyTime(&s1);

	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);
	SetBusy(TRUE);
}

// 选中了事件UI
void  CPatientDataDlg::OnMyEventSelected(CControlUI * pCtl) {
	int cnt = m_lay_events->GetCount();
	for (int i = 0; i < cnt; i++) {
		CMyEventUI * pItem = (CMyEventUI *)m_lay_events->GetItemAt(i);
		if (pItem == pCtl) {
			pItem->SetSelected(TRUE); 
			m_selected_event = i;
		}
		else {
			pItem->SetSelected(FALSE); 
		}
	}
}

// 
void  CPatientDataDlg::OnAddMyEvent(CMyEventUI * pEventUI) {
	int cnt = m_lay_events->GetCount();
	if (cnt > 0) {
		RECT r = { 1,0,1,1 };
		pEventUI->SetBorderSize(r);
	}
	else {
		pEventUI->SetBorderSize(1);
		pEventUI->SetSelected(TRUE);
		m_selected_event = 0;
	}
	m_lay_events->Add(pEventUI);
}

//
void  CPatientDataDlg::OnDelMyEvent() {
	int cnt = m_lay_events->GetCount();
	if (0 == cnt)
		return;
	if (m_selected_event < 0)
		return;
	if (m_selected_event >= cnt)
		return;

	m_lay_events->RemoveAt(m_selected_event);
	cnt = m_lay_events->GetCount();
	if ( cnt > 0 ) {
		CMyEventUI * pEvent = (CMyEventUI *)m_lay_events->GetItemAt(0);
		pEvent->m_lay_1->SetBorderSize(1);

		if (m_selected_event < cnt) {
			pEvent = (CMyEventUI *)m_lay_events->GetItemAt(m_selected_event);
			pEvent->SetSelected(TRUE);
		}
		else {
			pEvent = (CMyEventUI *)m_lay_events->GetItemAt(cnt-1);
			pEvent->SetSelected(TRUE);
			m_selected_event = cnt - 1;
		}
	}
	else {
		m_selected_event = -1;
	}
}

//
CDuiString  CPatientDataDlg::FormatInt(int nValue) {
	CDuiString str;
	if (nValue > 0) {
		str.Format("%d", nValue);
	}
	return str;
}

// 自动填充体温
void  CPatientDataDlg::OnAutoTemp() {
	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 18;
	time_t tFirstDay = GetAnyDayZeroTime(m_patient_data[0].m_date);

	for (int i = 0; i < 7; i++) {
		// 先获取温度数据
		m_tree->GetConfigValue(nRow + 1 + i, cfgValue);
		for (int j = 0; j < 6; j++) {
			// 如果是空的数据
			if ( (cfgValue.m_nValues[j][0] == 0) && (cfgValue.m_nValues[j][1] == 0) ) {
				time_t tStart = tFirstDay + 3600 * 24 * i + 3600 * 4 * j + 3600 * 2;
				time_t tEnd = tStart + 3600 * 4;
				cfgValue.m_nValues[j][0] = GetTemp(tStart, tEnd);
				cfgValue.m_nValues[j][1] = 0;
			}			
		}
		m_tree->SetConfigValue(nRow + 1 + i, cfgValue);
	}
}

DWORD  CPatientDataDlg::GetTemp(time_t tStart, time_t tEnd) {
	DWORD  dwTemp = 0;
	std::vector<TempItem *>::iterator it;
	for (it = m_VTemp.begin(); it != m_VTemp.end(); ++it) {
		TempItem * pItem = *it;
		if ( pItem->m_time < tStart )
			continue;
		if (pItem->m_time >= tEnd)
			break;
		// 以前是取最后的值，现在改为最大值
		if (pItem->m_dwTemp > dwTemp)
			dwTemp = pItem->m_dwTemp;
	}
	return dwTemp;
}

// 上一页，下一页
void  CPatientDataDlg::OnPrevPage() {
	OnPrevPage_1();
}

void  CPatientDataDlg::OnNextPage() {	
	OnNextPage_1();
}

void  CPatientDataDlg::OnPrevPage_1() {

	PatientInfo info;
	std::vector<PatientEvent * > vEvents;

	GetPatientInfo(&info, vEvents);
	if ( info.m_in_hospital <= 0 ) {
		MessageBox(GetHWND(), "请先输入住院日期", "错误", 0);
		return;
	}

	time_t  tHospital = GetAnyDayZeroTime(info.m_in_hospital);
	time_t  tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_end->GetTime())) - 3600 * 24 * 6;
	if ( tFirstDay - 86400 * 7 < tHospital ) {
		return;
	}

	// 先保存数据
	PatientData data[7];
	GetPatientData(data, 7);

	for (DWORD i = 0; i < 7; i++) {
		if (IsPatientDataChanged(&data[i], i)) {
			STRNCPY(data[i].m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);
			data[i].m_date = tFirstDay + 3600 * 24 * i;
			// 保存数据库
			CBusiness::GetInstance()->SavePatientDataAsyn(&data[i]);
		}
	}
	// END 保存数据

	tFirstDay -= 3600 * 24 * 7;    // 604800
	SYSTEMTIME s1 = Time2SysTime(tFirstDay);
	m_date_start->SetMyTime(&s1);

	time_t tEndDay = tFirstDay + 3600 * 24 * 6;
	SYSTEMTIME s2 = Time2SysTime(tEndDay);
	m_date_end->SetMyTime(&s2);

	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);
	SetBusy(TRUE);

	time_t  diff = tFirstDay - tHospital;
	int nPageIndex = (int)diff / (86400 * 7) + 1;
	CDuiString  strText;
	strText.Format("%d", nPageIndex);
	m_lblPageIndex->SetText(strText);
}

void  CPatientDataDlg::OnNextPage_1() {
	PatientInfo info;
	std::vector<PatientEvent * > vEvents;

	GetPatientInfo(&info, vEvents);
	if (info.m_in_hospital <= 0) {
		MessageBox(GetHWND(), "请先输入住院日期", "错误", 0);
		return;
	}

	time_t  tHospital = GetAnyDayZeroTime(info.m_in_hospital);
	time_t  tFirstDay = GetAnyDayZeroTime(SysTime2Time(m_date_end->GetTime())) - 3600 * 24 * 6;

	// 先保存数据
	PatientData data[7];
	GetPatientData(data, 7);

	for (DWORD i = 0; i < 7; i++) {
		if (IsPatientDataChanged(&data[i], i)) {
			STRNCPY(data[i].m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);
			data[i].m_date = tFirstDay + 3600 * 24 * i;
			// 保存数据库
			CBusiness::GetInstance()->SavePatientDataAsyn(&data[i]);
		}
	}
	// END 保存数据

	tFirstDay += 3600 * 24 * 7;    // 604800
	SYSTEMTIME s1 = Time2SysTime(tFirstDay);
	m_date_start->SetMyTime(&s1);

	time_t tEndDay = tFirstDay + 3600 * 24 * 6;
	SYSTEMTIME s2 = Time2SysTime(tEndDay);
	m_date_end->SetMyTime(&s2);

	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);
	SetBusy(TRUE);

	CDuiString  strText;
	if ( tFirstDay >= tHospital ) {
		time_t  diff = tFirstDay - tHospital;
		int nPageIndex = (int)diff / (86400 * 7) + 1;		
		strText.Format("%d", nPageIndex);
		m_lblPageIndex->SetText(strText);
	}
	else {
		m_lblPageIndex->SetText("");
	}
}