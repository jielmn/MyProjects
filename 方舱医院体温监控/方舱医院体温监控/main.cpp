// win32_1.cpp : ����Ӧ�ó������ڵ㡣
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "DuiMenu.h"
#include "AboutDlg.h"
#include "SettingDlg.h"
#include "TagUI.h"
#include "PatientDataDlg.h"
#include "HelpDlg.h"

#define   TIMER_DRAG_DROP_GRID                   1001
#define   INTERVAL_TIMER_DRAG_DROP_GRIDS         1500

// 1���Ӷ�ʱ��
#define   TIMER_60_SECONDS                       1002
#define   INTERVAL_TIMER_60_SECONDS              60000

// ������tag��ɫ��ʱ��
#define   TIMER_NEW_TAG                          1101
#define   INTERVAL_TIMER_NEW_TAG                 5000

//�Զ����¼�����
static bool sortTagUI(CTagUI * p1, CTagUI * p2)
{
	if (p1->m_nBindingGridIndex > 0 && p2->m_nBindingGridIndex > 0) {
		return p1->m_nBindingGridIndex < p2->m_nBindingGridIndex;
	}
	else if (p1->m_nBindingGridIndex > 0) {
		return true;
	}
	else if (p2->m_nBindingGridIndex > 0) {
		return false;
	}
	else {
		return false;
	}
}
  
CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {	
	m_dwCurSelGridIndex = 0;
	m_eGridStatus = GRID_STATUS_GRIDS;

	m_tabs = 0;
	m_layMain = 0;
	memset(m_pGrids, 0, sizeof(m_pGrids));
	m_layPages = 0;
	m_btnPrevPage = 0;
	m_btnNextPage = 0;

	m_nDgSourceIndex = -1;
	m_nDgDestIndex = -1;
	m_dragdropGrid = 0;
	m_bDragTimer = FALSE;
	m_bFlipPrevPage = FALSE;

	m_btnMenu = 0;
	m_dwDgStartTick = 0;

	m_lblBarTips = 0;
	m_lblLaunchStatus = 0;

	m_cstHandImg = 0;
	m_layTags = 0;
	m_dragdrop_tag_dest_index = -1;
	m_dragdrop_tag_cur_index = -1;
	m_hand_tabs = 0;
	m_dragdrop_tag_timetick = 0;
	m_layDragDropGrids = 0;

	m_LastSaveExcelTime = 0;
	m_nQCurPageIndex = 0;
	m_nQCurPageIndex1 = 0;

	m_optInHospital = 0;
	m_optOutHospital = 0;

	m_CubeItems = 0;
	m_CubeItems_high_temp = 0;
	m_bNewTagDragDrop = FALSE;
	m_HightLightItem = 0;
	m_layRight = 0;
	m_layMain1 = 0;

	m_edtBedNo1 = 0;
	m_edtName1 = 0;
	m_edtPhone1 = 0;

	m_CurTag = 0;

	memset(&m_cur_tag, 0, sizeof(m_cur_tag));

	m_btnCubeBed = 0;
	m_lblCubePatientName = 0;
	m_lblCubePhone = 0;

	m_CubeImg = 0;
	m_nMaxCubeBedNo = 0;

	m_SelectedCubeItem = 0;
	m_SelectedCubeItem_high_temp = 0;
	m_MenuCuteItem = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {
	ClearVector(m_vQRet);
	ClearVector(m_vQRet1);

	ClearVector(m_cube_items);
}

void  CDuiFrameWnd::RemoveAllGrids() {
	// ����ӿؼ�
	int nCnt = m_layMain->GetCount();
	for (int i = 0; i < nCnt; i++) {
		m_layMain->RemoveAt(0, true);
	}
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	RemoveAllGrids();

	// ����grids
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		assert(m_pGrids[i]);
		m_pGrids[i]->Delete();
	}
}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	g_data.m_hWnd = GetHWND();
	g_data.m_hCursor = LoadCursor(NULL, IDC_SIZENS);
	time_t  now = time(0);

	/*************  ��ȡ�ؼ� *****************/
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(TABS_ID));
	m_layMain = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layPages = static_cast<DuiLib::CHorizontalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_PAGES));
	m_btnPrevPage = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(BUTTON_PREV_PAGE));
	m_btnNextPage = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(BUTTON_NEXT_PAGE));
	m_dragdropGrid = m_PaintManager.FindControl(DRAG_DROP_GRID); 
	m_btnMenu = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MENU));
	m_lblBarTips = static_cast<CLabelUI*>(m_PaintManager.FindControl(LBL_BAR_TIPS));
	m_lblLaunchStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl(LBL_LAUNCH_STATUS));
	m_lblSurReaderTry = static_cast<CLabelUI*>(m_PaintManager.FindControl(LBL_PROC_TIPS));
	m_optInHospital = static_cast<COptionUI*>(m_PaintManager.FindControl("opn_inhospital"));
	m_optOutHospital = static_cast<COptionUI*>(m_PaintManager.FindControl("opn_outhospital"));

#if !SHOW_IN_HOSPITAL_FLAG
	m_optInHospital->SetVisible(false);
	m_optOutHospital->SetVisible(false); 
#endif

	// ��Ӵ���
	for ( DWORD i = 0; i < MAX_GRID_COUNT; i++ ) {
		m_pGrids[i] = new CGridUI;                   
		m_pGrids[i]->SetBorderSize(GRID_BORDER_SIZE);
		m_pGrids[i]->SetBorderColor(GRID_BORDER_COLOR);
		m_pGrids[i]->SetName(GRID_NAME);
		m_pGrids[i]->SetTag(i);
		m_pGrids[i]->SetBedNo(i + 1);
		m_pGrids[i]->SetMode( (CModeButton::Mode)g_data.m_CfgData.m_GridCfg[i].m_dwGridMode );
		m_layMain->Add(m_pGrids[i]);
		m_pGrids[i]->SetPatientName(g_data.m_CfgData.m_GridCfg[i].m_szPatientName);
	}
	m_cstHandImg = static_cast<CMyHandImage *>(m_PaintManager.FindControl(CST_HAND_IMAGE));
	m_layTags = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_TAGS));

	g_data.m_DragDropCtl = m_PaintManager.FindControl(DRAG_DROP_CTL); 
	g_data.m_edRemark    = static_cast<CEditUI *>( m_PaintManager.FindControl(EDIT_REMARK) );
	g_data.m_edHandRemark = static_cast<CEditUI *>(m_PaintManager.FindControl(EDIT_HAND_REMARK)); 

	m_optDefaultSort = static_cast<COptionUI *>(m_PaintManager.FindControl(OPT_DEFAULT));
	m_optTimeSort = static_cast<COptionUI *>(m_PaintManager.FindControl(OPT_TIME));

	m_hand_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(HAND_TABS_ID));
	m_layDragDropGrids = static_cast<CTileLayoutUI *>(m_PaintManager.FindControl(LAY_DRAGDROP_GRIDS));

	// סԺ��Ϣ
	m_edQName = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQName"));
	m_cmbQSex = static_cast<CComboUI *>(m_PaintManager.FindControl("cmbQSex"));
	m_edQAge  = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQAge"));
	m_edQOutPatient = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQOutPatient"));
	m_edQHospitalAdmissionNo = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQHospitalAdmissionNo"));
	m_datInHospitalStart = static_cast<CDateTimeUI *>(m_PaintManager.FindControl("datQTime1"));
	m_datInHospitalEnd = static_cast<CDateTimeUI *>(m_PaintManager.FindControl("datQTime2"));
	m_btnQuery = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnQ"));
	m_q_waiting_bar = static_cast<CWaitingBarUI *>(m_PaintManager.FindControl("Q_waiting_bar"));
	m_lblQueryRet = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblQRet"));
	m_lstQueryRet = static_cast<CListUI *>(m_PaintManager.FindControl("lstQRet"));
	m_btnQPrev = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnQPrePage"));
	m_btnQNext = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnQNextPage"));

	m_lblQueryRet->SetText("");
	m_btnQPrev->SetEnabled(false); 
	m_btnQNext->SetEnabled(false);	
	time_t t = now - 3600 * 24 * 30;
	SYSTEMTIME s = Time2SysTime(t);
	char szDate[256];
	Date2String(szDate, sizeof(szDate), &t);
	m_datInHospitalStart->SetTime(&s);
	m_datInHospitalStart->SetText(szDate);
	m_q_waiting_bar->SetBkImage("file='progress_back_1.png' corner='5,5,5,5'");

	// ��Ժ��Ϣ
	m_edQName1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQName1"));
	m_cmbQSex1 = static_cast<CComboUI *>(m_PaintManager.FindControl("cmbQSex1"));
	m_edQAge1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQAge1"));
	m_edQOutPatient1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQOutPatient1"));
	m_edQHospitalAdmissionNo1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtQHospitalAdmissionNo1"));
	m_datInHospitalStart1 = static_cast<CDateTimeUI *>(m_PaintManager.FindControl("datQTime11"));
	m_datInHospitalEnd1 = static_cast<CDateTimeUI *>(m_PaintManager.FindControl("datQTime12"));
	m_datOutHospitalStart1 = static_cast<CDateTimeUI *>(m_PaintManager.FindControl("datQTime21"));
	m_datOutHospitalEnd1 = static_cast<CDateTimeUI *>(m_PaintManager.FindControl("datQTime22"));
	m_btnQuery1 = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnQ1"));
	m_q_waiting_bar1 = static_cast<CWaitingBarUI *>(m_PaintManager.FindControl("Q_waiting_bar1"));
	m_lblQueryRet1 = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblQRet1"));
	m_lstQueryRet1 = static_cast<CListUI *>(m_PaintManager.FindControl("lstQRet1"));
	m_btnQPrev1 = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnQPrePage1"));
	m_btnQNext1 = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnQNextPage1"));

	m_lblQueryRet1->SetText("");
	m_btnQPrev1->SetEnabled(false);
	m_btnQNext1->SetEnabled(false); 
	m_datInHospitalStart1->SetTime(&s);
	m_datInHospitalStart1->SetText(szDate);
	m_datOutHospitalStart1->SetTime(&s);
	m_datOutHospitalStart1->SetText(szDate);
	m_q_waiting_bar1->SetBkImage("file='progress_back_1.png' corner='5,5,5,5'");


	/////////////////// ����
	m_CubeItems = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl("layCubeItems"));
	m_CubeItems_high_temp = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl("layCubeItems_1"));
	m_SubSwitch = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl("subswitch"));
	m_layRight = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl("layRight"));
	m_layMain1 = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl("layMain_1"));
	m_edtBedNo1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtBedNo1"));
	m_edtName1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtName1"));
	m_edtPhone1 = static_cast<CEditUI *>(m_PaintManager.FindControl("edtPhone1"));
	m_CurTag = static_cast<CNewTagUI *>(m_PaintManager.FindControl("curtag"));
	m_btnCubeBed = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnBedCube"));
	m_lblCubePatientName = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblPatientNameCube"));
	m_lblCubePhone = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblPhoneCube"));
	m_CubeImg = static_cast<CMyImageUI *>(m_PaintManager.FindControl("cstCubeImage"));

	m_layMain1->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMain1Size);

#if CUBE_TEST_FLAG
	for (int i = 0; i < 1000; i++) {
		CCubeItemUI * item = new CCubeItemUI;
		CDuiString  s;

		item->SetBedNo(i + 1);
		s.Format("����%d", i + 1);
		item->SetPatientName(s);
		s.Format("1981325%04d", i + 1);
		item->SetPhone(s);
		item->SetTemperature(3600 + (i * 10) % 400 );
		item->SetTime(now);
		m_CubeItems->Add(item);
	}

	for (int i = 0; i < 1000; i++) {
		CCubeItemUI * item = new CCubeItemUI;
		item->SetBedNo(i + 100);
		item->SetPatientName("����1");
		item->SetPhone("123xyz");
		item->SetTemperature(3600 + i * 10);
		item->SetTime(now);
		m_CubeItems_high_temp->Add(item);
	}
#endif
                  
	//////////// 

	
	/*************  end ��ȡ�ؼ� *****************/

	m_cstHandImg->m_sigTagErased.connect(this, &CDuiFrameWnd::OnHandTagErasedNotify);

	RefreshGridsPage();

	m_hand_tabs->SelectItem(1);	 

	// ����prepared��
	//CheckDevice();
	CBusiness::GetInstance()->PrepareAsyn();

	/***** ������ʱ�� ****/
	SetTimer( GetHWND(), TIMER_60_SECONDS, INTERVAL_TIMER_60_SECONDS, 0 );
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if ( 0 == strcmp("SurgencyTemp", pstrClass) ) {		
		strText.Format( "%s.xml", pstrClass );
		pUI = builder.Create( (const char *)strText, (UINT)0, &m_callback, &m_PaintManager );
		return pUI; 
	}
	else if (0 == strcmp("HandReaderTemp", pstrClass)) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Cube", pstrClass)) {
		strText.Format("%s.xml", pstrClass);                           
		pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("DragDrop", pstrClass)) {
		return new CDragDropUI;
	}
	else if (0 == strcmp("InHospital", pstrClass)) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("OutHospital", pstrClass)) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("WaitingBar", pstrClass)) {
		return new CWaitingBarUI;
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("opn_monitor")) {
			m_tabs->SelectItem(TAB_INDEX_MONITOR);
		}
		else if (name == _T("opn_reader")) {
			// �����Ĭ�����򣬰Ѱ󶨵ķ���ǰ��
			HandTagSortType eSortType = GetHandTagSortType();
			if (eSortType == HandTagSortType_Default) {
				OnHandTagDefaultOrder();
			}
			m_tabs->SelectItem(TAB_INDEX_READER);                  
		}
		else if (name == _T("opn_inhospital")) {
			m_tabs->SelectItem(TAB_INDEX_INHOSPITAL); 
		}
		else if (name == _T("opn_outhospital")) {
			m_tabs->SelectItem(TAB_INDEX_OUTHOSPITAL);
		}
		else if ( name == "rdTimeOrder" ) {
			OnHandTagTimeOrder();
		}
		else if (name == "rdDefaultOrder") {
			OnHandTagDefaultOrder();
		}
	}
	else if (msg.sType == "click") {
		if ( name == BUTTON_NEXT_PAGE ) {
			NextPage();
		}
		else if (name == BUTTON_PREV_PAGE) {
			PrevPage();
		}
		else if (name == BTN_MENU) {
			OnBtnMenu(msg);
		}
		else if (name == BTN_PRINT) {
			OnBtnPrint(g_data.m_CfgData.m_GridOrder[m_dwCurSelGridIndex]);
		}
		else if (name == "btnQ") {
			OnQueryInHospital();
		}
		else if (name == "btnQNextPage") {
			NextInHospitalPage();
		}
		else if (name == "btnQPrePage") {
			PrevInHospitalPage();
		}
		else if (name == "btnQ1") {
			OnQueryOutHospital();
		}
		else if (name == "btnQNextPage1") {
			NextOutHospitalPage();
		}
		else if (name == "btnQPrePage1") {
			PrevOutHospitalPage();
		}
		else if ( name == "btnAdd1" ) {
			OnAddCubeItem();
		}
	}
	else if (msg.sType == "setting") {
		OnSetting();
	}
	else if (msg.sType == "about") {
		OnAbout();
	}
	else if (msg.sType == "help") {
		OnHelp();
	}
	else if (msg.sType == "killfocus") {
		if (name == EDIT_REMARK) {
			::OnEdtRemarkKillFocus();
		}
		else if (name == EDIT_HAND_REMARK) {
			::OnEdtHandRemarkKillFocus();
		}
	}
	else if (msg.sType == "menu") {
		if (name == CST_IMG_LBL_TEMP) { 
			OnImageMenu(msg.pSender->GetTag(), msg.ptMouse, msg.pSender);
		}
		else if (name == CST_IMAGE) {
			OnImageMenu(msg.pSender->GetTag(), msg.ptMouse, msg.pSender);
		}
		else if (name == "cstMyImageHand") {
			OnHandImageMenu(msg.ptMouse, msg.pSender);
		}
		else if (name == "layHandTag") {
			OnTagMenu(msg);
		}
		else if (name == "lstQRet") {
			OnInHospitalMenu(msg);
		}
		else if (name == "lstQRet1") {
			OnOutHospitalMenu(msg);
		}
	}
	else if ( msg.sType == "menu_export_excel" ) {
		OnExportExcel(msg.wParam);
	}
	else if ( msg.sType == "menu_print_excel" ) {
		OnPrintExcel(msg.wParam); 
	}
	else if ( msg.sType == "menu_print_chart" ) {
		OnBtnPrint(msg.wParam);
	}
	// �����ֳֶ�������tag��ӡ���µ�
	else if (msg.sType == "menu_print_chart_1") {
		OnBtnPrint1(msg);
	}
	else if (msg.sType == "menu_hand_export_excel") {
		OnHanxExportExcel();
	}
	else if (msg.sType == "menu_hand_print_excel") {
		OnHandPrintExcel();
	}
	else if (msg.sType == "tag_selected") {
		OnHandTagSelected(msg.pSender);
	}
	else if (msg.sType == "menu_del_tag") {
		OnDeleteTag(msg);
	}
	else if (msg.sType == "menu_print_chart_2") {
		OnBtnPrint2(msg);
	}
	else if (msg.sType == "menu_print_chart_3") {
		OnBtnPrint3(msg);
	}
	else if (msg.sType == "cube_menu" ) {
		OnCubeItemMenu(msg);
	}
	else if (msg.sType == "cube_click") {
		OnCubeItemClick(msg);
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_LBUTTONDBLCLK) {
		BOOL bHandled = FALSE;
		OnDbClick(bHandled);
		if (bHandled)
			return 0;
	}
	else if (uMsg == WM_TIMER) {
		if (wParam == TIMER_DRAG_DROP_GRID) {
			OnFlipPage();
		}
		else if (wParam == TIMER_60_SECONDS) {
			On60SecondsTimer();
		}
		else if (wParam == TIMER_NEW_TAG) {
			OnNewTagTimer();
		}
	}
	else if (uMsg == WM_DEVICECHANGE) {
		CheckDevice();
	}
	else if (uMsg == UM_LAUNCH_STATUS) {
		OnLaunchStatus(wParam, lParam);
	}
	else if (uMsg == UM_TRY_SUR_READER) {
		OnTrySurReader(wParam, lParam);
	}
	else if (uMsg == UM_SUR_READER_STATUS) {
		OnSurReaderStatus(wParam, lParam);
	}
	else if (uMsg == UM_SUR_READER_TEMP) {
		OnSurReaderTemp(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_TEMP_BY_TAG_ID_RET) {
		OnQueryTempRet(wParam, lParam);
	}
	else if (uMsg == UM_HAND_READER_TEMP) {
		OnHandReaderTemp(wParam, lParam);
	}
	else if (uMsg == UM_PREPARED) {
		OnPrepared(wParam, lParam);
	}
	else if (uMsg == UM_ALL_HAND_TAG_TEMP_DATA) {
		OnAllHandTagTempData(wParam, lParam);
	}
	else if (uMsg == UM_BINDING_TAG_GRID_RET) {
		OnTagBindingGridRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_HAND_TEMP_BY_TAG_ID_RET) {
		OnQueryHandTempRet(wParam, lParam);
	}
	else if ( uMsg == UM_TAG_NAME_CHANGED ) {
		OnTagNameChanged( wParam, lParam );
	}
	else if (uMsg == UM_QUERY_BINDING_BY_TAG_RET) {
		OnQueryBindingByTag(wParam, lParam);
	}
	else if (uMsg == UM_DEL_TAG_RET) {
		OnDelTagRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_INHOSPITAL_RET) {
		OnQueryInHospitalRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_OUTHOSPITAL_RET) {
		OnQueryOutHospitalRet(wParam, lParam);
	}
	else if (uMsg == UM_CHECK_COM_PORTS_RET) {
		OnChecComPortsRet(wParam, lParam);
	}
	else if (uMsg == UM_GET_ALL_CUBE_ITEMS) {
		OnGetAllCubeItems(wParam, lParam);
	}
	else if (uMsg == UM_ADD_CUBE_ITEMS_RET) {
		OnAddCubeItemRet(wParam, lParam);
	}
	else if (uMsg == UM_CUBE_TEMP_ITEM) {
		OnCubeTempItem(wParam, lParam);
	}
	else if (uMsg == UM_CUBE_BINDING_RET) {
		OnCubeBindingTag(wParam, lParam);
	}
	else if (uMsg == UM_CUBE_QUERY_TEMP_RET) {
		OnQueryCubeTempRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

LRESULT  CDuiFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	// layWindow������margin
	DWORD dwWidth = LOWORD(lParam) - 10 * 2;           
	// 30�ǵײ�status�ĸ߶�, 32�Ǳ������߶�, 85��tabҳ�߶�
	DWORD dwHeight = HIWORD(lParam) - 30 - 1 * 2 - 32 - 85;
	RefreshGridsSize(dwWidth, dwHeight);

	// �Ժ�������������ַ�ʽ����ӿؼ�OnSize�ص�����
	// m_Control->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMySize);
	// ����ԭ�ͣ�bool OnMySize(void * pParam);
	ResetDragdropGrids(dwWidth - 500, dwHeight);

	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(0, pt);
	// ���û�е�����κοؼ�
	if (0 == pCtl) {
		return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
	}

	// ����ԭʼ����Ŀؼ�
	CControlUI* pOriginalCtl = pCtl;
	int nSelTab = m_tabs->GetCurSel();

	// ����ǵ�һTabҳ
	if ( nSelTab == TAB_INDEX_MONITOR ) {
		//// ����Ƕ����״̬
		//if (m_eGridStatus == GRID_STATUS_GRIDS) {
		//	while (pCtl) {
		//		if (pCtl->GetName() == GRID_NAME) {
		//			// ������ǵ���޸����ְ�ť
		//			if (0 != strcmp(pOriginalCtl->GetClass(), "Button")
		//				&& 0 != strcmp(pOriginalCtl->GetClass(), "ModeButton")) {
		//				m_nDgSourceIndex = GetGridOrderByGridId(pCtl->GetTag());
		//				m_nDgDestIndex = -1;
		//				m_dwDgStartTick = LmnGetTickCount();
		//			}
		//			break;
		//		}
		//		pCtl = pCtl->GetParent();
		//	}
		//}
		//// ��������״̬
		//else if (m_eGridStatus == GRID_STATUS_MAXIUM) {
		//	while (pCtl) {
		//		if (pCtl->GetName() == LAY_READER) {
		//			CReaderUI * pReader = (CReaderUI *)pCtl;
		//			DWORD i = pReader->GetGridIndex();
		//			DWORD j = pReader->GetReaderIndex();
		//			m_pGrids[i]->OnSurReaderSelected(j);
		//			break;
		//		}
		//		pCtl = pCtl->GetParent();
		//	}
		//}

		while (pCtl) {
			if (pCtl->GetClass() == "NewTag") {
				// �����ǰ����tag���ſ�ʼ�϶�
				if (m_cur_tag.szTagId[0] != '\0') {
					m_dwDgStartTick = LmnGetTickCount();
					m_bNewTagDragDrop = TRUE;
					m_HightLightItem = 0;
				}
				break;
			}
			pCtl = pCtl->GetParent();
		}
	}
	else if (nSelTab == TAB_INDEX_READER) {
		while (pCtl) {
			if ( 0 == strcmp( pCtl->GetClass(), "TagUI") ) {
				// ���ǵ���˰�ť
				if ( 0 != strcmp( pOriginalCtl->GetClass(), "Button") ) {
					CTagUI * pTagUI = (CTagUI *)pCtl;
					m_dragdrop_tag = pTagUI->GetTagId();
					m_dragdrop_tag_dest_index = -1;
					m_dragdrop_tag_cur_index = pTagUI->GetBindingGridIndex();
					m_dragdrop_tag_timetick = LmnGetTickCount();
				}
			}
			pCtl = pCtl->GetParent();
		}
	}
	
	
	return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	//if (m_nDgSourceIndex >= 0) {
	//	StopMoveGrid();
	//}
	//// �����϶�tag ui
	//else if (m_dragdrop_tag.GetLength() > 0) {
	//	StopMoveTagUI();
	//}
	if (m_bNewTagDragDrop) {
		StopMoveNewTag();
	}
	return WindowImplBase::OnLButtonUp(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	POINT  pt;
	DWORD  dwCurTick = 0;

	//if (m_nDgSourceIndex >= 0) {
	//	dwCurTick = LmnGetTickCount();
	//	if (dwCurTick - m_dwDgStartTick < DRAG_DROP_DELAY_TIME) {
	//		return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
	//	}

	//	pt.x = LOWORD(lParam);
	//	pt.y = HIWORD(lParam);
	//	MoveGrid(pt);
	//}
	//// �����϶�tag ui
	//else if (m_dragdrop_tag.GetLength() > 0) {
	//	dwCurTick = LmnGetTickCount();
	//	// ����1��
	//	if ( dwCurTick - m_dragdrop_tag_timetick >= 100 ) {
	//		pt.x = LOWORD(lParam);
	//		pt.y = HIWORD(lParam);
	//		MoveTagUI(pt);
	//	}
	//}

	if (m_bNewTagDragDrop) {
		dwCurTick = LmnGetTickCount();
		if (dwCurTick - m_dwDgStartTick < DRAG_DROP_DELAY_TIME) {
			return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}

		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		MoveNewTag(pt);
	}
	return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

// �ƶ����ӣ�����˳��
void   CDuiFrameWnd::MoveGrid(const POINT & pt) {
	if (!m_dragdropGrid->IsVisible()) {
		m_dragdropGrid->SetVisible(true);
	}

	int x = 100 / 2;
	int y = 100 / 2;

	RECT r;
	r.left   = pt.x - x;
	r.right  = r.left + x * 2;
	r.top    = pt.y - y;
	r.bottom = r.top + y * 2;
	m_dragdropGrid->SetPos(r);

	// ���������ĸ���
	OnMoveGrid(pt);
}

void   CDuiFrameWnd::MoveNewTag(const POINT & pt) {
	if (!m_dragdropGrid->IsVisible()) {
		m_dragdropGrid->SetVisible(true);
	}

	int x = 100 / 2;
	int y = 100 / 2;

	RECT r;
	r.left = pt.x - x;
	r.right = r.left + x * 2;
	r.top = pt.y - y;
	r.bottom = r.top + y * 2;
	m_dragdropGrid->SetPos(r);

	// ���������ĸ���
	OnMoveNewTag(pt);
}

// �����ƶ������У������ĸ���Ҫ����
void   CDuiFrameWnd::OnMoveGrid(const POINT & pt) {
	RECT rect   = m_layMain->GetPos();
	int nWidth  = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;

	if ( nWidth <= 0 || nHeight <= 0 ) {
		return;
	}

	assert(g_data.m_CfgData.m_dwLayoutColumns > 0);
	assert(g_data.m_CfgData.m_dwLayoutRows > 0);

	// ÿҳ����grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// ��ǰҳindex
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// ��ǰҳ�ĵ�һ��grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// ��ǰҳ�����grid index
	DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage < g_data.m_CfgData.m_dwLayoutGridsCnt
		? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

	// �����˷�Χ
	if ( pt.x <= rect.left || pt.x >= rect.right || pt.y <= rect.top || pt.y >= rect.bottom ) {	
		if (m_nDgDestIndex >= 0) {
			m_pGrids[g_data.m_CfgData.m_GridOrder[m_nDgDestIndex]]->SetBorderColor(GRID_BORDER_COLOR);
		}
		m_nDgDestIndex = -1;

		// ����Ǵ�ֱ���򳬳���Χ��ˮƽ����û�г���
		if ( pt.x > rect.left && pt.x < rect.right ) {
			// ������ȷ�����򣬹ر�Timer
			if (m_bDragTimer) {
				KillTimer( GetHWND(), TIMER_DRAG_DROP_GRID );
				m_bDragTimer = FALSE;
			}
		}
		// �����ˮƽ���򳬳���Χ����ֱ����û�г���
		else if (pt.y > rect.top && pt.y < rect.bottom) {
			// �������·�ҳ����Ҫ������ʱ��
			// ��ǰ��ҳ
			if ( pt.x <= rect.left ) {
				// ���������ǰ��ҳ
				if ( dwGridPageIndex > 0) {
					// ���û�п�����ʱ��
					if (!m_bDragTimer) {
						SetTimer(GetHWND(), TIMER_DRAG_DROP_GRID, INTERVAL_TIMER_DRAG_DROP_GRIDS, 0);
						m_bDragTimer = TRUE;
						m_bFlipPrevPage = TRUE;
					}
				}				
			}
			// ���ҳ
			else if (pt.x >= rect.right) {
				// �������ҳ
				if ( dwMaxGridIndex < g_data.m_CfgData.m_dwLayoutGridsCnt - 1 ) {
					// ���û�п�����ʱ��
					if (!m_bDragTimer) {
						SetTimer(GetHWND(), TIMER_DRAG_DROP_GRID, INTERVAL_TIMER_DRAG_DROP_GRIDS, 0);
						m_bDragTimer = TRUE;
						m_bFlipPrevPage = FALSE;
					}
				}
			}
		}
		// ˮƽ�ʹ�ֱ���򶼳�����Χ
		else {
			// ������ȷ�����򣬹ر�Timer
			if (m_bDragTimer) {
				KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
				m_bDragTimer = FALSE;
			}
		}
		return;
	}

	SIZE s = m_layMain->GetItemSize();
	for ( DWORD i = dwFirstGridIndexCurPage; i < dwMaxGridIndex; i++ ) {
		int  nRowIndex = (i - dwFirstGridIndexCurPage) / g_data.m_CfgData.m_dwLayoutColumns;
		int  nColIndex = (i - dwFirstGridIndexCurPage) % g_data.m_CfgData.m_dwLayoutColumns;
		if ((pt.x > rect.left + s.cx * nColIndex) && (pt.x < rect.left + s.cx * (nColIndex + 1))
			&& (pt.y > rect.top + s.cy * nRowIndex) && (pt.y < rect.top + s.cy * (nRowIndex + 1))) {
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetBorderColor(GRID_HILIGHT_BORDER_COLOR);
			m_nDgDestIndex = i;
		}
		else {
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetBorderColor(GRID_BORDER_COLOR);
		}
	}
	KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
	m_bDragTimer = FALSE;
}

// ֹͣ�ƶ����ӣ�ȷ������λ��
void  CDuiFrameWnd::StopMoveGrid() {
	m_dragdropGrid->SetVisible(false);
	KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
	m_bDragTimer = FALSE;

	// û��ѡ��Ҫ�϶���λ��
	if (m_nDgDestIndex < 0) {
		m_nDgSourceIndex = -1;
		m_nDgDestIndex = -1;
		return;
	}

	// ȡ��������border
	assert(m_nDgDestIndex < (int)g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[g_data.m_CfgData.m_GridOrder[m_nDgDestIndex]]->SetBorderColor(GRID_BORDER_COLOR);

	// ���source��dest���
	if (m_nDgSourceIndex == m_nDgDestIndex) {
		m_nDgSourceIndex = -1;
		m_nDgDestIndex = -1;
		return;
	}

	// ���磺grid 2�ϵ�grid 4λ��
	// ��λ������: 1, 3, 4, 2
	if ( m_nDgSourceIndex < m_nDgDestIndex ) {
		DWORD  dwSourceId = g_data.m_CfgData.m_GridOrder[m_nDgSourceIndex];
		for ( int i = m_nDgSourceIndex; i < m_nDgDestIndex; i++ ) {
			g_data.m_CfgData.m_GridOrder[i] = g_data.m_CfgData.m_GridOrder[i + 1];
		}
		g_data.m_CfgData.m_GridOrder[m_nDgDestIndex] = dwSourceId;
		m_dwCurSelGridIndex = m_nDgDestIndex;
	}
	else {
		DWORD  dwSourceId = g_data.m_CfgData.m_GridOrder[m_nDgSourceIndex];
		for (int i = m_nDgSourceIndex; i > m_nDgDestIndex; i--) {
			g_data.m_CfgData.m_GridOrder[i] = g_data.m_CfgData.m_GridOrder[i - 1];
		}
		g_data.m_CfgData.m_GridOrder[m_nDgDestIndex] = dwSourceId;
		m_dwCurSelGridIndex = m_nDgDestIndex;
	}
	RefreshGridsPage();

	char szDefaultOrder[256];	
	GetDefaultGridOrder(szDefaultOrder, sizeof(szDefaultOrder), g_data.m_CfgData.m_dwLayoutGridsCnt);

	char szOrder[256];
	GetGridOrder(szOrder, sizeof(szOrder), g_data.m_CfgData.m_dwLayoutGridsCnt, g_data.m_CfgData.m_GridOrder);

	g_data.m_cfg->SetConfig ( CFG_GRIDS_ORDER, szOrder, szDefaultOrder );
	g_data.m_cfg->Save();

	m_nDgSourceIndex = -1;
	m_nDgDestIndex = -1;
}

void  CDuiFrameWnd::OnMoveNewTag(const POINT & pt) {
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(m_CubeItems, pt);

	CCubeItemUI * pHighLight = 0;

	// ���û���ƶ����κοؼ���
	if (0 == pCtl) {
		if (m_HightLightItem) {
			m_HightLightItem->SetBorderSize(0);
		}
		m_HightLightItem = 0;
		return;
	}

	while (pCtl) {
		if (pCtl->GetClass() == "CubeItem") {
			pHighLight = (CCubeItemUI *)pCtl;
			break;
		}
		pCtl = pCtl->GetParent();
	}

	if (pHighLight) {
		if ( m_HightLightItem ) {
			if (m_HightLightItem != pHighLight) {
				pHighLight->SetBorderSize(1);
				pHighLight->SetBorderColor(GRID_HILIGHT_BORDER_COLOR);
				m_HightLightItem->SetBorderSize(0);
			}
		}
		m_HightLightItem = pHighLight;
	}
	else {
		if (m_HightLightItem) {
			m_HightLightItem->SetBorderSize(0);
		}
		m_HightLightItem = 0;
	}
}

void  CDuiFrameWnd::StopMoveNewTag() {
	m_dragdropGrid->SetVisible(false);
	m_bNewTagDragDrop = FALSE;

	if (m_HightLightItem) {
		m_HightLightItem->SetBorderSize(0);

		int nBedNo = m_HightLightItem->GetBedNo();
		CBusiness::GetInstance()->CubeBindingTagAsyn(nBedNo, m_cur_tag.szTagId);

		m_HightLightItem = 0;
	}
}

// �϶����ӹ����У��ϵ���������ң����·�ҳ
void  CDuiFrameWnd::OnFlipPage() {
	// ÿҳ����grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// ��ǰҳindex
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// ��ǰҳ�ĵ�һ��grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// ��ǰҳ�����grid index
	DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage < g_data.m_CfgData.m_dwLayoutGridsCnt
		? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

	if (m_bFlipPrevPage) {
		if ( dwGridPageIndex > 0 ) {
			m_dwCurSelGridIndex = dwFirstGridIndexCurPage - dwCntPerPage;
			RefreshGridsPage();
		}
		else {
			KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
			m_bDragTimer = FALSE;
		}
	}
	else {
		if ( dwMaxGridIndex < g_data.m_CfgData.m_dwLayoutGridsCnt - 1 ) {
			m_dwCurSelGridIndex = dwFirstGridIndexCurPage + dwCntPerPage;
			RefreshGridsPage();
		}
		else {
			KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
			m_bDragTimer = FALSE;
		}
	}
}

// ����gridsһҳ(��������"��һҳ", "��һҳ"����ʼ���ȵ�)
void   CDuiFrameWnd::RefreshGridsPage() {
	// ��������е�grids
	RemoveAllGrids();

	/**** �������Ҫ��grids  ***/

	// �����״̬
	if (m_eGridStatus == GRID_STATUS_GRIDS) {
		// ÿҳ����grids
		DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
		// ��ǰҳindex
		DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
		// ��ǰҳ�ĵ�һ��grid index
		DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
		// ��ǰҳ�����grid index
		DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage  < g_data.m_CfgData.m_dwLayoutGridsCnt
			? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

		// �趨�̶�����
		m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);
		for (DWORD i = dwFirstGridIndexCurPage; i < dwMaxGridIndex; i++) {
			assert(g_data.m_CfgData.m_GridOrder[i] < g_data.m_CfgData.m_dwLayoutGridsCnt);
			m_layMain->Add(m_pGrids[g_data.m_CfgData.m_GridOrder[i]]);
			// m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetVisible(true);
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetInternVisible(true);
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetView(0);
		}

		// �����Ҫ��ҳ��ʾ
		if (g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows
			< g_data.m_CfgData.m_dwLayoutGridsCnt) {
			m_layPages->SetVisible(true);

			if ( dwGridPageIndex == 0 ) {
				m_btnPrevPage->SetEnabled(false);
			}
			else {
				m_btnPrevPage->SetEnabled(true);
			}

			if (dwGridPageIndex == (g_data.m_CfgData.m_dwLayoutGridsCnt - 1) / dwCntPerPage) {
				m_btnNextPage->SetEnabled(false); 
			}
			else {
				m_btnNextPage->SetEnabled(true);
			}
		}
		else {
			m_layPages->SetVisible(false);
		}
	}
	else {
		// �趨�̶�����
		m_layMain->SetFixedColumns(1);
		// ��ӵ�������
		m_layMain->Add(m_pGrids[g_data.m_CfgData.m_GridOrder[m_dwCurSelGridIndex]]);
		// �ر�����ҳ
		m_layPages->SetVisible(false);
	}
	
}

// ����grids size
// width, height: m_layMain��parent��
void   CDuiFrameWnd::RefreshGridsSize(int width, int height) {
	if (0 == m_layMain)
		return;

	SIZE s;
	CContainerUI * pParent = (CContainerUI *)m_layMain->GetParent();

	if ( GRID_STATUS_GRIDS == m_eGridStatus ) { 

		// �����Ҫ��ҳ��ʾ
		if (g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows
			< g_data.m_CfgData.m_dwLayoutGridsCnt) {
			height -= 30;
		}

		s.cx = ( width - 1 ) / (int)g_data.m_CfgData.m_dwLayoutColumns + 1;
		s.cy = ( height - 1 ) / (int)g_data.m_CfgData.m_dwLayoutRows + 1;
		OnGridSizeChanged(s);
		m_layMain->SetItemSize(s);
	}
	else {
		// height += 30;
		s.cx = width;
		s.cy = height;
		m_layMain->SetItemSize(s);
	}
}

void   CDuiFrameWnd::RefreshGridsSize() {
	if (0 == m_layMain)
		return;
	CControlUI* pParent = m_layMain->GetParent();
	RECT rect = pParent->GetPos();
	RefreshGridsSize(rect.right - rect.left, rect.bottom - rect.top);
}

void   CDuiFrameWnd::NextPage() {
	// ÿҳ����grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// ��ǰҳindex
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// ��ǰҳ�ĵ�һ��grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// ��һҳ�ĵ�һgrid index�����޸ĵ�ǰ��grid index
	m_dwCurSelGridIndex = dwFirstGridIndexCurPage + dwCntPerPage;

	RefreshGridsPage();
}

void   CDuiFrameWnd::PrevPage() {
	// ÿҳ����grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// ��ǰҳindex
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// ��ǰҳ�ĵ�һ��grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;

	// ��һҳ�ĵ�һgrid index�����޸ĵ�ǰ��grid index
	assert(dwFirstGridIndexCurPage >= dwCntPerPage);
	m_dwCurSelGridIndex = dwFirstGridIndexCurPage - dwCntPerPage;

	RefreshGridsPage();
}

void  CDuiFrameWnd::OnDbClick(BOOL & bHandled) {
	POINT point;
	CDuiString strName;
	CDuiString strClass;
	bHandled = FALSE;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);

	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_tabs, point);
	if (0 == pFindControl) {
		return;
	}

	DuiLib::CDuiString  clsName = pFindControl->GetClass();
	// ���˫��һЩ��ť����������ģ�������
	if (0 == strcmp(clsName, DUI_CTR_BUTTON)) {
		return;
	}
	else if (0 == strcmp(clsName, DUI_CTR_OPTION)) {
		return;
	}
	else if (0 == strcmp(clsName, DUI_CTR_SCROLLBAR)) {
		return;
	}
	else if (0 == strcmp(clsName, "MyImage")) {
		return; 
	}
	else if (0 == strcmp(clsName, "ModeButton")) {
		return;
	}

	while (pFindControl) {
		strName = pFindControl->GetName();
		strClass = pFindControl->GetClass();

		if (0 == strcmp(strName, GRID_NAME)) {

			if (m_eGridStatus == GRID_STATUS_GRIDS) {
				// �ҵ����
				DWORD dwGridIndex = GetGridOrderByGridId(pFindControl->GetTag());
				m_dwCurSelGridIndex = dwGridIndex;
				m_eGridStatus = GRID_STATUS_MAXIUM;
			}
			else {
				m_eGridStatus = GRID_STATUS_GRIDS;
			}
			
			CGridUI * pGrid = (CGridUI *)pFindControl;
			pGrid->SwitchView(); 

			RefreshGridsPage();
			RefreshGridsSize();

			bHandled = TRUE;
			break;
		}
		else if ( 0 == strcmp(strClass, "CubeItem")) {
			m_SubSwitch->SelectItem(1); 
			CCubeItemUI * pItem = (CCubeItemUI *)pFindControl;
			CDuiString  strText;
			strText.Format("%d", pItem->GetBedNo());
			m_btnCubeBed->SetText(strText);
			m_lblCubePatientName->SetText(pItem->GetPatientName());
			m_lblCubePhone->SetText(pItem->GetPhone());
			CBusiness::GetInstance()->QueryCubeTempAsyn(pItem->GetBedNo());
			// ���image���¶�����
			m_CubeImg->ClearCubeData();
			m_CubeImg->MyInvalidate();
			m_nMaxCubeBedNo = pItem->GetBedNo();
		}
		else if (0 == strcmp(strName, "maxiumCube")) {
			m_SubSwitch->SelectItem(0);
			m_nMaxCubeBedNo = 0;
		}
		pFindControl = pFindControl->GetParent();   
	}  
}

// ����ˡ��˵�����ť
void   CDuiFrameWnd::OnBtnMenu(TNotifyUI& msg) {
	RECT r = m_btnMenu->GetPos();
	POINT pt = { r.left, r.bottom };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

// ����ˡ����á�
void  CDuiFrameWnd::OnSetting() {
	CDuiString  strText;
	DWORD  dwValue = 0;
	BOOL   bValue = FALSE;

	CfgData  oldData = g_data.m_CfgData;
	CSettingDlg * pSettingDlg = new CSettingDlg;
	pSettingDlg->m_data = g_data.m_CfgData;
	pSettingDlg->Create(this->m_hWnd, _T("����"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// �������click ok
	if (0 != ret) {
		delete pSettingDlg;
		return;
	}

	g_data.m_CfgData = pSettingDlg->m_data;

	dwValue = DEFAULT_MAIN_LAYOUT_COLUMNS;
	g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, &dwValue);

	dwValue = DEFAULT_MAIN_LAYOUT_ROWS;
	g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_ROWS, g_data.m_CfgData.m_dwLayoutRows, &dwValue);

	dwValue = DEFAULT_MAIN_LAYOUT_GRIDS_COUNT;
	g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_GRIDS_COUNT, g_data.m_CfgData.m_dwLayoutGridsCnt, &dwValue);

	dwValue = 0;
	g_data.m_cfg->SetConfig(CFG_AREA_ID_NAME, g_data.m_CfgData.m_dwAreaNo, &dwValue);

	// ������������
	bValue = DEFAULT_ALARM_VOICE_SWITCH;
	g_data.m_cfg->SetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, &bValue);

	// �Զ�����excel
	bValue = FALSE;
	g_data.m_cfg->SetBooleanConfig(CFG_AUTO_SAVE_EXCEL, g_data.m_CfgData.m_bAutoSaveExcel, &bValue);

	// ʮ��ê
	bValue = FALSE;
	g_data.m_cfg->SetBooleanConfig(CFG_CROSS_ANCHOR, g_data.m_CfgData.m_bCrossAnchor, &bValue);

	// �ֳ������ʾ�¶�
	dwValue = DEFAULT_MIN_TEMP_IN_SHOW;
	g_data.m_cfg->SetConfig(CFG_HAND_MIN_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMinTemp, &dwValue);
	// �ֳ������ʾ�¶�
	dwValue = DEFAULT_MAX_TEMP_IN_SHOW;
	g_data.m_cfg->SetConfig(CFG_HAND_MAX_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMaxTemp, &dwValue);

	// �ֳֵ��±���
	dwValue = DEFAULT_LOW_TEMP_ALARM;
	g_data.m_cfg->SetConfig(CFG_HAND_LOW_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderLowTempAlarm, &dwValue);
	// �ֳָ��±���
	dwValue = DEFAULT_HIGH_TEMP_ALARM;
	g_data.m_cfg->SetConfig(CFG_HAND_HIGH_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderHighTempAlarm, &dwValue);

	for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
		SaveGrid(i);
	}

	// �����grid����ɾ��
	for (DWORD i = g_data.m_CfgData.m_dwLayoutGridsCnt; i < MAX_GRID_COUNT; i++) {
		RemoveGridCfg(i);
	}

	// ������Ŀ�ı�
	if ( oldData.m_dwLayoutGridsCnt != g_data.m_CfgData.m_dwLayoutGridsCnt ) {
		g_data.m_cfg->RemoveConfig(CFG_GRIDS_ORDER);

		// ����ĸ��ӵ�����Ҫɾ��
		for (DWORD i = oldData.m_dwLayoutGridsCnt; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
			m_pGrids[i]->ResetData();
		}

		m_dwCurSelGridIndex = 0;
		m_eGridStatus = GRID_STATUS_GRIDS;
		ResetGridOrder();
		RefreshGridsPage();
		RefreshGridsSize();

		CBusiness::GetInstance()->RestartLaunchAsyn();
	}
	// ������Ŀ����
	else {
		// �У��иı�
		if (oldData.m_dwLayoutColumns != g_data.m_CfgData.m_dwLayoutColumns
			|| oldData.m_dwLayoutRows != g_data.m_CfgData.m_dwLayoutRows) {
			m_eGridStatus = GRID_STATUS_GRIDS;
			RefreshGridsPage();
			RefreshGridsSize();
		}		

		DWORD  dwDelay = 0;
		for ( DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++ ) {
			// �����ʱ�ɼ�����ı�
			if (oldData.m_GridCfg[i].m_dwCollectInterval != g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval) {
				CBusiness::GetInstance()->GetGridTemperatureAsyn(i, dwDelay);
				dwDelay += 200;
			}
		}
	}

	g_data.m_cfg->Save();
	::InvalidateRect(GetHWND(), 0, TRUE);
	delete pSettingDlg;


	// �����gridɾ���ֳ�tag��
	for ( DWORD i = g_data.m_CfgData.m_dwLayoutGridsCnt; i < oldData.m_dwLayoutGridsCnt; i++ ) {
		CBusiness::GetInstance()->RemoveGridBindingAsyn(i+1);		
	}

	// ������ɾ����
	int nTagCnt = m_layTags->GetCount();
	for (int i = 0; i < nTagCnt; i++) {
		CTagUI * pTagUI = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pTagUI->m_nBindingGridIndex > (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
			pTagUI->SetBindingGridIndex(0);
			break;
		}
	}
}

// �����������
void  CDuiFrameWnd::SaveGrid(DWORD  i) {
	CDuiString  strText;
	BOOL  bValue = FALSE;
	DWORD  dwValue = 0;

	strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
	dwValue = 0;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval, &dwValue);
	
	//strText.Format("%s %lu", CFG_GRID_MIN_TEMP, i + 1);
	//dwValue = DEFAULT_MIN_TEMP_IN_SHOW;
	//g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp, &dwValue);

	//strText.Format("%s %lu", CFG_GRID_MAX_TEMP, i + 1);
	//dwValue = DEFAULT_MAX_TEMP_IN_SHOW;
	//g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp, &dwValue);

	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
		dwValue = DEFAULT_LOW_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm, &dwValue);

		strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
		dwValue = DEFAULT_HIGH_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm, &dwValue);
	}

	strText.Format("%s %lu", CFG_HAND_READER_LOW_TEMP_ALARM, i + 1);
	dwValue = DEFAULT_LOW_TEMP_ALARM;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm, &dwValue);
	
	strText.Format("%s %lu", CFG_HAND_READER_HIGH_TEMP_ALARM, i + 1);
	dwValue = DEFAULT_HIGH_TEMP_ALARM;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm, &dwValue);	
}

// ɾ����������
void  CDuiFrameWnd::RemoveGridCfg(DWORD  i) {
	CDuiString  strText;

	strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	strText.Format("%s %lu", CFG_GRID_MIN_TEMP, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	strText.Format("%s %lu", CFG_GRID_MAX_TEMP, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	strText.Format("%s %lu", CFG_GRID_MODE, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
		strText.Format("%s %lu %lu", CFG_READER_SWITCH, i + 1, j + 1);
		g_data.m_cfg->RemoveConfig(strText);

		strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
		g_data.m_cfg->RemoveConfig(strText);

		strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
		g_data.m_cfg->RemoveConfig(strText);
	}
}
 
// ����ˡ����ڡ�
void  CDuiFrameWnd::OnAbout() {   
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("����"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	pAboutDlg->ShowModal();

	delete pAboutDlg;
}

// ����ˡ�������
void   CDuiFrameWnd::OnHelp() {
	CHelpDlg * pHelpDlg = new CHelpDlg;

	pHelpDlg->Create(this->m_hWnd, _T("����"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pHelpDlg->CenterWindow();
	pHelpDlg->ShowModal();

	delete pHelpDlg;
}

// Ӳ���豸�䶯�������д��ڱ䶯
void  CDuiFrameWnd::CheckDevice() {
	char szComPort[16];
	int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

	// ��������
	if (!g_data.m_bMultipleComport) {
		if (nFindCount > 1) {
			m_lblBarTips->SetText("���ڶ��USB-SERIAL CH340���ڣ���ֻ����һ��������");
		}
		else if (0 == nFindCount) {
			m_lblBarTips->SetText("û���ҵ�USB-SERIAL CH340���ڣ������ӷ�������USB��");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}
	else {
		if (0 == nFindCount) { 
			m_lblBarTips->SetText("û���ҵ�USB-SERIAL CH340���ڣ������ӷ�������USB��");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}

	CBusiness::GetInstance()->CheckLaunchAsyn();
}

// ������״̬
void   CDuiFrameWnd::OnLaunchStatus(WPARAM wParam, LPARAM  lParam) {
	CLmnSerialPort::PortStatus e = (CLmnSerialPort::PortStatus)wParam;
	if (e == CLmnSerialPort::OPEN) {
		m_lblLaunchStatus->SetText("���������ӳɹ�");
	}
	else {
		m_lblLaunchStatus->SetText("���������ӶϿ�");
	}
}

// ���ж��������ڶ�������ʾ
void    CDuiFrameWnd::OnTrySurReader(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	BOOL   bStart = (BOOL)lParam;

	CDuiString  strText;
	if (bStart) {
		strText.Format("%lu%c���������ڲ���...", i + 1, (char)(j + 'A'));
		m_lblSurReaderTry->SetText(strText);
	}
	else {
		m_lblSurReaderTry->SetText("");
	}
}

// ���ж�����״̬
void  CDuiFrameWnd::OnSurReaderStatus(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	BOOL   bConnected = (BOOL)lParam;

	m_pGrids[i]->SetSurReaderStatus(j, bConnected);
}

// ���ж������¶�
void   CDuiFrameWnd::OnSurReaderTemp(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	TempItem * pItem = (TempItem *)lParam;
	m_pGrids[i]->OnSurReaderTemp(j, *pItem);
	delete pItem;
}

// �����¶������С
void   CDuiFrameWnd::OnGridSizeChanged(const SIZE & s) {
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		m_pGrids[i]->OnSizeChanged(s);
	}

	if (s.cy >= 295) {
		g_data.m_nVMargin = 40;
	}
	else if (s.cy >= 245) {
		g_data.m_nVMargin = 30;
	}
	else if (s.cy >= 195) {
		g_data.m_nVMargin = 20;
	}
	else{
		g_data.m_nVMargin = 10;
	}
}

//  1���Ӷ�ʱ��( 1. ��ʱ������ȥʱ��; 2. ɾ��һ��ǰ���¶����� )
void   CDuiFrameWnd::On60SecondsTimer() {
	//for (int i = 0; i < MAX_GRID_COUNT; i++) {
	//	m_pGrids[i]->UpdateElapsed();
	//	m_pGrids[i]->PruneData();
	//	m_pGrids[i]->Invalidate();
	//}

	//m_cstHandImg->PruneData();

	//CDuiString  strCurHandTag = m_cstHandImg->GetCurTagId();
	//if ( strCurHandTag.GetLength() > 0 ) {
	//	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strCurHandTag);
	//	// �����tag�Ѿ�ɾ��
	//	if ( it == m_tags_ui.end() ) {
	//		// �������tag
	//		if ( m_layTags->GetCount() > 0 ) {
	//			CControlUI * pChildUI = m_layTags->GetItemAt(0);
	//			OnHandTagSelected(pChildUI);
	//		}
	//		// ���û��tag
	//		else {
	//			m_cstHandImg->SetCurTag("");
	//		}
	//	}
	//}

	///*********** ��ʱ����excel *************/
	//time_t now = time(0);
	//struct tm  tmp;
	//localtime_s(&tmp, &now);

	//// CBusiness::GetInstance()->SaveExcelAsyn();   ���ڵ�����

	//// ʵ���÷�
	//if ((tmp.tm_hour == 8 || tmp.tm_hour == 12 || tmp.tm_hour == 18 || tmp.tm_hour == 0)
	//	&& (tmp.tm_min >= 0 && tmp.tm_min <= 1)) {		
	//	// ���μ��ʱ������30����
	//	if (now - m_LastSaveExcelTime >= 1800) {
	//		// ����
	//		CBusiness::GetInstance()->SaveExcelAsyn();
	//		m_LastSaveExcelTime = now;
	//	}
	//}		
}

// ��ѯ�¶Ƚ��
void   CDuiFrameWnd::OnQueryTempRet(WPARAM wParam, LPARAM  lParam) {
	void ** pParam = (void **)wParam;

	char * pTagId = (char *)pParam[0];
	WORD   wBedNo = (WORD)(pParam[1]);
	std::vector<TempItem*> * pvRet = (std::vector<TempItem*> *)pParam[2];

	DWORD  i = (wBedNo - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wBedNo - 1) % MAX_READERS_PER_GRID;
	assert(i < MAX_GRID_COUNT);

	m_pGrids[i]->OnQueryTempRet( j, pTagId, *pvRet );

	delete[] pTagId;
	//ClearVector(*pvRet);
	delete pvRet;
	delete[] pParam;
}

// �Ҽ������˵�
void   CDuiFrameWnd::OnImageMenu(DWORD  dwGridIndex, const POINT & pt, CControlUI * pParent ) {
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_image.xml"), pParent, dwGridIndex, 0 );
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE); 
}

// ����Excel
void   CDuiFrameWnd::OnExportExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[dwIndex]->ExportExcel();
}

// ��ӡExcelͼ��
void   CDuiFrameWnd::OnPrintExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[dwIndex]->PrintExcel();
}
 
// �ֳֶ������¶�
void   CDuiFrameWnd::OnHandReaderTemp(WPARAM wParam, LPARAM  lParam) {
	TempItem * pItem            = (TempItem*)wParam;
	char *     tag_patient_name = (char *)lParam;
	assert(pItem);
	assert(tag_patient_name);

	BOOL  bNewTag = FALSE;
	m_cstHandImg->OnHandTemp(pItem, bNewTag);

	CTagUI * pTagUI = 0;

	assert(pItem->m_szTagId[0] != '\0');
	// ��Tag
	if ( bNewTag ) {
		pTagUI = new CTagUI;  
		m_layTags->AddAt(pTagUI,0); 
		pTagUI->SetFixedHeight(TAG_UI_HEIGHT);
		pTagUI->OnHandTemp(pItem, tag_patient_name); 

		m_tags_ui.insert( std::make_pair(pItem->m_szTagId, pTagUI) );

		CBusiness::GetInstance()->QueryBindingByTagIdAsyn(pItem->m_szTagId);		
	}
	else {		
		pTagUI = m_tags_ui[pItem->m_szTagId];
		assert(pTagUI);
		pTagUI->OnHandTemp(pItem, tag_patient_name);

		// �Ƿ�ʱ������
		HandTagSortType eSortType = GetHandTagSortType();
		if ( eSortType == HandTagSortType_Time ) {
			CTagUI * pItem = (CTagUI *) m_layTags->GetItemAt(0);
			// ������ǵ�һλ
			if (pItem != pTagUI) {
				m_layTags->Remove(pTagUI, true);
				m_layTags->AddAt(pTagUI, 0);
			}			
		}
	}

	// ������˴�λ��
	int nBindingGridIndex = pTagUI->GetBindingGridIndex();
	if ( nBindingGridIndex > 0 ) {
		assert(nBindingGridIndex <= MAX_GRID_COUNT );
		m_pGrids[nBindingGridIndex - 1]->OnHandReaderTemp(*pItem);
	}

	// �Ѿ�������m_cstHandImg������
	// delete pItem;
	delete[] tag_patient_name;
}

// ���ݿ��������ݲ�ѯ���
void   CDuiFrameWnd::OnPrepared(WPARAM wParam, LPARAM  lParam) {
	CheckDevice();
}

// ��õ�ǰ���ֳ�Tag����
CDuiFrameWnd::HandTagSortType CDuiFrameWnd::GetHandTagSortType() {
	if (m_optDefaultSort->IsSelected())
		return HandTagSortType_Default;
	else
		return HandTagSortType_Time;
}

// ��������ֳ�Tag�¶�����
void   CDuiFrameWnd::OnAllHandTagTempData(WPARAM wParam, LPARAM  lParam) {
	std::vector<HandTagResult *> * pvRet = (std::vector<HandTagResult *> *)wParam;
	std::vector<HandTagResult *>::iterator it;

	assert( m_layTags->GetCount() == 0 );
	assert(m_tags_ui.size() == 0);

	for (it = pvRet->begin(); it != pvRet->end(); ++it) {
		HandTagResult * pItem = *it;
		// û������
		if (0 == pItem->m_pVec || pItem->m_pVec->size() == 0)
			continue;

		// ��������
		m_cstHandImg->OnHandTempVec(pItem->m_pVec, pItem->m_szTagId);

		CTagUI * pTagUI = new CTagUI;
		m_layTags->Add(pTagUI);
		pTagUI->SetFixedHeight(TAG_UI_HEIGHT);

		TempItem * pSubItem = pItem->m_pVec->at(pItem->m_pVec->size() - 1);
		pTagUI->OnHandTemp(pSubItem, pItem->m_szTagPName);
		pTagUI->SetBindingGridIndex(pItem->m_nBindingGridIndex);

		// ������˴�λ��
		if (pItem->m_nBindingGridIndex > 0) {
			assert(pItem->m_nBindingGridIndex <= MAX_GRID_COUNT);
			CBusiness::GetInstance()->QueryTempByHandTagAsyn(pItem->m_szTagId, pItem->m_nBindingGridIndex);
			m_pGrids[pItem->m_nBindingGridIndex - 1]->SetPatientNameInHandMode(pItem->m_szTagPName);
		}

		assert(pItem->m_szTagId[0] != '\0');
		m_tags_ui.insert(std::make_pair(pItem->m_szTagId, pTagUI));
	}

	if ( m_layTags->GetCount() > 0 ) {
		OnHandTagSelected(m_layTags->GetItemAt(0));
	}


	/**** �����ڴ� ****/
	for ( it = pvRet->begin(); it != pvRet->end(); ++it) {
		HandTagResult * pItem = *it;
		if ( 0 == pItem->m_pVec )
			continue;
		// �Ѿ�������m_cstHandImg
		//ClearVector(*pItem->m_pVec);
		//delete pItem->m_pVec;
	}
	ClearVector(*pvRet);
	delete pvRet;
}

// ѡ�����ֳ�Tag
void  CDuiFrameWnd::OnHandTagSelected(CControlUI * pTagUI) {	
	int nCnt = m_layTags->GetCount();
	for ( int i = 0; i < nCnt; i++ ) {
		CTagUI* pUI = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pUI == pTagUI ) {
			pUI->SetBkColor(0xFF555555);
			m_cstHandImg->SetCurTag(pUI->GetTagId());
			m_cur_selected_tag = pUI->GetTagId();
		}
		else {
			pUI->SetBkColor(0);
		}
	}	

	CTagUI* pUI = (CTagUI *)pTagUI;
	int cnt = m_layDragDropGrids->GetCount();
	for (int i = 0; i < cnt; i++) {
		CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(i);
		if (i == pUI->m_nBindingGridIndex - 1) {
			pChild->SetBorderColor(CUR_BINDING_GRID_BORDERCOLOR);
			pChild->SetTextColor(CUR_BINDING_GRID_TEXTCOLOR);
			pChild->SetBkColor(CUR_BINDING_GRID_BKCOLOR);
		}
		else {
			pChild->SetBorderColor(UNBINDING_GRID_BORDERCOLOR);
			pChild->SetTextColor(UNBINDING_GRID_TEXTCOLOR);
			pChild->SetBkColor(UNBINDING_GRID_BKCOLOR);
		}
	}
}

// ����˰�ʱ������
void  CDuiFrameWnd::OnHandTagTimeOrder() {
	int nCnt = m_layTags->GetCount();
	for ( int i = 0; i < nCnt; i++ ) {
		m_layTags->RemoveAt(0, true);
	}

	std::map<std::string, CTagUI *>::iterator it;
	for ( it = m_tags_ui.begin(); it != m_tags_ui.end(); ++it ) {
		CTagUI * pTagUI = it->second;
		assert(pTagUI);
		OnHandTagTimeOrder(pTagUI);
	}
}

// �����Ĭ������
void   CDuiFrameWnd::OnHandTagDefaultOrder() {
	std::vector<CTagUI *> vSort;
	int nCnt = m_layTags->GetCount();
	for (int i = 0; i < nCnt; i++) {
		CTagUI * pTagUI = (CTagUI *)m_layTags->GetItemAt(0);
		vSort.push_back(pTagUI);
		m_layTags->RemoveAt(0, true);
	}

	std::sort(vSort.begin(), vSort.end(), sortTagUI);
	std::vector<CTagUI *>::iterator it;
	for (it = vSort.begin(); it != vSort.end(); ++it) {
		CTagUI * pTagUI = *it;
		m_layTags->Add(pTagUI);
	}
}

void   CDuiFrameWnd::OnHandTagTimeOrder(CTagUI * pTagUI) {
	int nCnt = m_layTags->GetCount();

	int i = 0;
	for ( i = 0; i < nCnt; i++ ) {
		CTagUI * pItem = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pTagUI->m_item.m_time > pItem->m_item.m_time ) {
			break;
		}
	}

	m_layTags->AddAt(pTagUI, i);
}

// �ƶ�Tag UI���󶨴���
void   CDuiFrameWnd::MoveTagUI(const POINT & pt) {
	if (!m_dragdropGrid->IsVisible()) {
		m_dragdropGrid->SetVisible(true);
		//m_hand_tabs->SelectItem(1);

		ResetDragdropGrids();		
	}

	int x = 100 / 2;
	int y = 100 / 2;

	RECT r;
	r.left = pt.x - x;
	r.right = r.left + x * 2;
	r.top = pt.y - y;
	r.bottom = r.top + y * 2;
	m_dragdropGrid->SetPos(r);

	// TagUI ���������ĸ���
	OnMoveTagUI(pt);
}

// ֹͣ�ƶ�Tag UI��ȷ������λ��
void   CDuiFrameWnd::StopMoveTagUI() {
	if ( m_dragdrop_tag_dest_index >= 0 ) {
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)m_dragdrop_tag);
		if ( it != m_tags_ui.end() ) {
			CTagUI * pTag = it->second;
			assert( 0 == strcmp( pTag->m_item.m_szTagId, m_dragdrop_tag) );
			// ����󶨵�grid index�ı�
			if ( pTag->GetBindingGridIndex() != m_dragdrop_tag_dest_index + 1) {
				// �ȵ��𸴺������ý���
				CBusiness::GetInstance()->TagBindingGridAsyn(m_dragdrop_tag, m_dragdrop_tag_dest_index + 1);
			}			
		}
	}

	m_dragdrop_tag = "";
	m_dragdrop_tag_dest_index = -1;
	//m_hand_tabs->SelectItem(0);
	m_dragdropGrid->SetVisible(false);
}

// Tag UI�ƶ������У������ĸ���Ҫ����
void   CDuiFrameWnd::OnMoveTagUI(const POINT & pt) {
	RECT rect   = m_hand_tabs->GetPos();
	int  width  = rect.right - rect.left;
	int  height = rect.bottom - rect.top;

	int  nColumns = 0;
	int  nRows = 0;
	GetDragDropGridsParams(nColumns, nRows);
	assert(nColumns > 0 && nRows > 0);

	SIZE s  = m_layDragDropGrids->GetItemSize();
	int cnt = m_layDragDropGrids->GetCount();

	m_dragdrop_tag_dest_index = -1;
	for (int i = 0; i < nColumns; i++) {
		for (int j = 0; j < nRows; j++) {
			int nIndex = j * nColumns + i;

			// ����Grids��Χ��
			if ( nIndex < cnt ) {
				CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(nIndex);
				// �ھ��η�Χ��
				if ((pt.x > rect.left + s.cx * i) && (pt.x < rect.left + s.cx * (i + 1))
					&& (pt.y > rect.top + s.cy * j) && (pt.y < rect.top + s.cy * (j + 1))) {
					pChild->SetBorderColor(HILIGHT_BINDING_GRID_BORDERCOLOR);
					pChild->SetTextColor(HILIGHT_BINDING_GRID_TEXTCOLOR);
					pChild->SetBkColor(HILIGHT_BINDING_GRID_BKCOLOR);
					m_dragdrop_tag_dest_index = nIndex;
				}
				else {
					// ������ǵ�ǰ�󶨿�
					if (nIndex != m_dragdrop_tag_cur_index - 1) {
						pChild->SetBorderColor(UNBINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(UNBINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(UNBINDING_GRID_BKCOLOR);
					}
					// �ǵ�ǰ�󶨿�
					else {
						pChild->SetBorderColor(CUR_BINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(CUR_BINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(CUR_BINDING_GRID_BKCOLOR);
					}

				}
			}

		}
	}
}

// ��������LayoutGrids��ͼ
void   CDuiFrameWnd::ResetDragdropGrids(int width /*= 0*/, int height /*= 0*/) {
	if (0 == m_layDragDropGrids)
		return;

	if (0 == width || 0 == height) {
		RECT rect = m_hand_tabs->GetPos();
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}	

	m_layDragDropGrids->RemoveAll();

	int  nColumns = 0;
	int  nRows = 0;
	GetDragDropGridsParams(nColumns, nRows);
	assert(nColumns > 0 && nRows > 0);
	m_layDragDropGrids->SetFixedColumns(nColumns);

	SIZE s;
	s.cx = (width - 1) / nColumns + 1;
	s.cy = (height - 1) / nRows + 1;
	m_layDragDropGrids->SetItemSize(s);

	CDuiString  strText;
	for (int i = 0; i < (int)g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
		CLabelUI * pLabel = new CLabelUI;
		strText.Format("%d", i + 1);
		pLabel->SetText(strText);
		pLabel->SetTextColor(UNBINDING_GRID_TEXTCOLOR);
		pLabel->SetBorderColor(UNBINDING_GRID_BORDERCOLOR);
		pLabel->SetBkColor(UNBINDING_GRID_BKCOLOR);
		pLabel->SetBorderSize(1);
		pLabel->SetAttribute("align", "center");
		pLabel->SetFont(GetFontBySize(s));
		m_layDragDropGrids->Add(pLabel);
	}
}

// ��ȡDragdropGrids��ͼ���У���
void  CDuiFrameWnd::GetDragDropGridsParams(int & nCol, int & nRow) {
	for ( int i = 0; i < 8; i++ ) {
		if ( (i + 1) * (i + 1) >= (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
			nCol = nRow = i + 1;
			return;
		}
	}
	nCol = nRow = 8;
}

// ����Rect��С��ȡLabel�����С
int  CDuiFrameWnd::GetFontBySize(const SIZE & s) {
	
	if (s.cx >= 950 && s.cy >= 425) {
		return 27;
	}
	else if (s.cx >= 816 && s.cy >= 365) {
		return 26;
	}
	else if (s.cx > 680 && s.cy > 304) {
		return 25;
	}
	else if (s.cx > 572 && s.cy > 256) {
		return 24;
	}
	else if (s.cx > 466 && s.cy > 208) {
		return 23;
	}
	else if (s.cx > 350 && s.cy > 157) {
		return 22;
	}
	else if (s.cx > 280 && s.cy > 125) {
		return 21;
	}
	else {
		return 20;
	}
}

// tag �� grid index���
void   CDuiFrameWnd::OnTagBindingGridRet(WPARAM wParam, LPARAM  lParam) {
	TagBindingGridRet * pParam = (TagBindingGridRet*)wParam;

	std::map<std::string, CTagUI *>::iterator it;
	it = m_tags_ui.find(pParam->m_szTagId);
	if ( it != m_tags_ui.end() ) {
		CTagUI * pTagUI = it->second;
		if (pTagUI) {
			pTagUI->SetBindingGridIndex(pParam->m_nGridIndex);
			assert(pParam->m_nGridIndex > 0);
			CDuiString  strPName = pTagUI->GetPTagName();
			m_pGrids[pParam->m_nGridIndex - 1]->SetPatientNameInHandMode(strPName);

			// �����ǰѡ�е�tag���޸İ󶨵�tag
			if (m_cur_selected_tag == pTagUI->GetTagId()) {
				int cnt = m_layDragDropGrids->GetCount();
				for (int i = 0; i < cnt; i++) {
					CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(i);
					if (i == pTagUI->m_nBindingGridIndex - 1) {
						pChild->SetBorderColor(CUR_BINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(CUR_BINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(CUR_BINDING_GRID_BKCOLOR);
					}
					else {
						pChild->SetBorderColor(UNBINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(UNBINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(UNBINDING_GRID_BKCOLOR);
					}
				}
			}

		}			
	}

	it = m_tags_ui.find(pParam->m_szOldTagId);
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		if ( pTagUI )
			pTagUI->SetBindingGridIndex(0);
	}

	CBusiness::GetInstance()->QueryTempByHandTagAsyn(pParam->m_szTagId, pParam->m_nGridIndex);

	// �ɰ󶨴����ͼ�������Ҫ���
	if (pParam->m_nOldGridIndex > 0 ) {
		m_pGrids[pParam->m_nOldGridIndex - 1]->OnReleaseTagBinding();
	}

	delete pParam;
}

// ��ѯ�¶Ƚ��(�ֳ�Tag�¶�����)
void   CDuiFrameWnd::OnQueryHandTempRet(WPARAM wParam, LPARAM  lParam) {
	void ** pParam = (void **)wParam;

	char * pTagId     = (char *)pParam[0];
	int    nGridIndex = (int)(pParam[1]);
	std::vector<TempItem*> * pvRet = (std::vector<TempItem*> *)pParam[2];

	assert(nGridIndex <= MAX_GRID_COUNT && nGridIndex >= 1 );

	m_pGrids[nGridIndex-1]->OnQueryHandTempRet(pTagId, *pvRet);

	delete[] pTagId;
	// ClearVector(*pvRet);
	delete pvRet;
	delete[] pParam;
}

// tag name changed
void   CDuiFrameWnd::OnTagNameChanged( WPARAM wParam, LPARAM  lParam ) {
	CTagUI * pTagUI = (CTagUI *)wParam;
	assert(pTagUI);
	assert(pTagUI->m_nBindingGridIndex > 0);

	CDuiString  strName = pTagUI->GetPTagName();
	m_pGrids[pTagUI->m_nBindingGridIndex-1]->SetPatientNameInHandMode(strName);
}

// �Ҽ������˵�
void   CDuiFrameWnd::OnHandImageMenu(const POINT & pt, CControlUI * pParent) {
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_hand_image.xml"), pParent);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

// ����Excel
void   CDuiFrameWnd::OnHanxExportExcel() {
	CDuiString strTagId = m_cstHandImg->GetCurTagId();
	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find( (const char *)strTagId );
	if ( it != m_tags_ui.end() ) {
		CTagUI * pTagUI = it->second;
		if ( pTagUI ) {
			CDuiString strPName = pTagUI->GetPTagName();
			m_cstHandImg->ExportExcel(strPName);
		}		
	}	
}

// ��ӡExcelͼ��
void   CDuiFrameWnd::OnHandPrintExcel() {
	CDuiString strTagId = m_cstHandImg->GetCurTagId();
	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strTagId);
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		if (pTagUI) {
			CDuiString strPName = pTagUI->GetPTagName();
			m_cstHandImg->PrintExcel(strPName);
		}
	}
}

// �¶ȴ�ӡ
void   CDuiFrameWnd::OnBtnPrint(DWORD dwIndex) {
	CDuiString strTagId = m_pGrids[dwIndex]->GetCurTagId();
	if ( strTagId.GetLength() == 0 ) {
		::MessageBox(GetHWND(), "û��ѡ��������(tag)", "����", 0);
		return;
	}

	CDuiString strPatientName = m_pGrids[dwIndex]->GetCurPatientName();

	CPatientDataDlg * pDlg = new CPatientDataDlg;
	CBusiness::GetInstance()->m_sigPatientInfo.connect(pDlg, &CPatientDataDlg::OnPatientInfo);
	CBusiness::GetInstance()->m_sigPatientData.connect(pDlg, &CPatientDataDlg::OnPatientData);

	STRNCPY(pDlg->m_szTagId,   strTagId,       MAX_TAG_ID_LENGTH);
	STRNCPY(pDlg->m_szUIPName, strPatientName, MAX_TAG_PNAME_LENGTH);

	pDlg->Create(this->m_hWnd, _T("��ӡ���µ�"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// ������ָı�
	if ( strPatientName != pDlg->m_szUIPName ) {
		m_pGrids[dwIndex]->SetCurPatientName(pDlg->m_szUIPName);
	}

	// �����Ժ��,����ʾ	
	if ( pDlg->m_out_hospital_time > 0 ) {

		// ������ֳ�ģʽ
		if (m_pGrids[dwIndex]->GetMode() == CModeButton::Mode_Hand) {
			OnHandTagErasedNotify(strTagId);
			m_cstHandImg->DelTag(strTagId);
			m_cstHandImg->SetCurTag("");

			// ���ɾ����ѡ�е�tag
			if (m_cur_selected_tag == strTagId) {
				// �������tag
				if (m_layTags->GetCount() > 0) {
					CControlUI * pChildUI = m_layTags->GetItemAt(0);
					OnHandTagSelected(pChildUI);
				}
			}
		}			
		// ������ֳ�ģʽ(����)
		else if (m_pGrids[dwIndex]->GetMode() == CModeButton::Mode_Single) {
			m_pGrids[dwIndex]->OnOutHospital();
		}
	}

	// �������click ok
	if (0 != ret) {
		delete pDlg;
		return;
	}

	delete pDlg;            
}

void  CDuiFrameWnd::OnBtnPrint1(TNotifyUI& msg) {
	CTagUI * pTagUI = (CTagUI *)msg.pSender;
	CDuiString  strTagId = pTagUI->GetTagId();
	CDuiString strPatientName = pTagUI->GetPTagName();

	CPatientDataDlg * pDlg = new CPatientDataDlg;
	CBusiness::GetInstance()->m_sigPatientInfo.connect(pDlg, &CPatientDataDlg::OnPatientInfo);
	CBusiness::GetInstance()->m_sigPatientData.connect(pDlg, &CPatientDataDlg::OnPatientData);

	STRNCPY(pDlg->m_szTagId, strTagId, MAX_TAG_ID_LENGTH);
	STRNCPY(pDlg->m_szUIPName, strPatientName, MAX_TAG_PNAME_LENGTH);

	pDlg->Create(this->m_hWnd, _T("��ӡ���µ�"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// ������ָı�
	if (strPatientName != pDlg->m_szUIPName) {
		pTagUI->m_cstPatientName->SetText(pDlg->m_szUIPName);
	}

	// �����Ժ��,����ʾ	
	if (pDlg->m_out_hospital_time > 0) {

		// ������ֳ�ģʽ
		OnHandTagErasedNotify(strTagId);
		m_cstHandImg->DelTag(strTagId);
		m_cstHandImg->SetCurTag("");

		// ���ɾ����ѡ�е�tag
		if (m_cur_selected_tag == strTagId) {
			// �������tag
			if (m_layTags->GetCount() > 0) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
		}
	}

	// �������click ok
	if (0 != ret) {
		delete pDlg;
		return;
	}

	delete pDlg;
}

// סԺ�¶ȴ�ӡ
void  CDuiFrameWnd::OnBtnPrint2(TNotifyUI& msg) {
	int nSel = m_lstQueryRet->GetCurSel();
	if (nSel < 0)
		return;

	CListTextElementUI* pItemUI = (CListTextElementUI*)m_lstQueryRet->GetItemAt(nSel);
	CDuiString  strTagId = (const char *)pItemUI->GetTag();
	CDuiString  strPatientName = pItemUI->GetText(1);

	CPatientDataDlg * pDlg = new CPatientDataDlg;
	CBusiness::GetInstance()->m_sigPatientInfo.connect(pDlg, &CPatientDataDlg::OnPatientInfo);
	CBusiness::GetInstance()->m_sigPatientData.connect(pDlg, &CPatientDataDlg::OnPatientData);

	STRNCPY(pDlg->m_szTagId, strTagId, MAX_TAG_ID_LENGTH);
	STRNCPY(pDlg->m_szUIPName, strPatientName, MAX_TAG_PNAME_LENGTH);

	pDlg->Create(this->m_hWnd, _T("��ӡ���µ�"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// ������ָı�
	if (strPatientName != pDlg->m_szUIPName) {
		BOOL  bFind = FALSE;
		// ���ֲֳ�����
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strTagId);
		// ����ҵ���
		if (it != m_tags_ui.end()) {
			CTagUI * pTagUI = it->second;
			pTagUI->m_cstPatientName->SetText(pDlg->m_szUIPName);
			if ( pTagUI->GetBindingGridIndex() > 0 ) {
				int nIndex = pTagUI->GetBindingGridIndex() - 1;
				if (m_pGrids[nIndex]->GetMode() == CModeButton::Mode_Hand) {
					m_pGrids[nIndex]->SetCurPatientName(pDlg->m_szUIPName);
				}
			}
			bFind = TRUE;
		}

		// �Ӵ��񲿷���
		if (!bFind) {
			for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
				if ( m_pGrids[i]->GetCurTagId() == strTagId ) {
					m_pGrids[i]->SetCurPatientName(pDlg->m_szUIPName);
					break;
				}
			}
		}
	}

	// �����Ժ��,����ʾ	
	if (pDlg->m_out_hospital_time > 0) {

		// ���ֳ�ģʽ��
		OnHandTagErasedNotify(strTagId);
		m_cstHandImg->DelTag(strTagId);
		m_cstHandImg->SetCurTag("");

		// ���ɾ����ѡ�е�tag
		if (m_cur_selected_tag == strTagId) {
			// �������tag
			if (m_layTags->GetCount() > 0) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
		}

		// �Ӵ���ģʽ��
		for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
			if (m_pGrids[i]->GetCurTagId() == strTagId) {
				if ( m_pGrids[i]->GetMode() == CModeButton::Mode_Single ) {
					m_pGrids[i]->OnOutHospital();
				}
			}
		}
	}

	delete pDlg;
}

void  CDuiFrameWnd::OnBtnPrint3(TNotifyUI& msg) {
	int nSel = m_lstQueryRet1->GetCurSel();
	if (nSel < 0)
		return;

	CListTextElementUI* pItemUI = (CListTextElementUI*)m_lstQueryRet1->GetItemAt(nSel);
	CDuiString  strTagId = (const char *)pItemUI->GetTag();
	CDuiString  strPatientName = pItemUI->GetText(1);

	CPatientDataDlg * pDlg = new CPatientDataDlg;
	CBusiness::GetInstance()->m_sigPatientInfo.connect(pDlg, &CPatientDataDlg::OnPatientInfo);
	CBusiness::GetInstance()->m_sigPatientData.connect(pDlg, &CPatientDataDlg::OnPatientData);

	STRNCPY(pDlg->m_szTagId, strTagId, MAX_TAG_ID_LENGTH);
	STRNCPY(pDlg->m_szUIPName, strPatientName, MAX_TAG_PNAME_LENGTH);

	pDlg->Create(this->m_hWnd, _T("��ӡ���µ�"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// ������ָı�
	if (strPatientName != pDlg->m_szUIPName) {
		BOOL  bFind = FALSE;
		// ���ֲֳ�����
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strTagId);
		// ����ҵ���
		if (it != m_tags_ui.end()) {
			CTagUI * pTagUI = it->second;
			pTagUI->m_cstPatientName->SetText(pDlg->m_szUIPName);
			if (pTagUI->GetBindingGridIndex() > 0) {
				int nIndex = pTagUI->GetBindingGridIndex() - 1;
				if (m_pGrids[nIndex]->GetMode() == CModeButton::Mode_Hand) {
					m_pGrids[nIndex]->SetCurPatientName(pDlg->m_szUIPName);
				}
			}
			bFind = TRUE;
		}

		// �Ӵ��񲿷���
		if (!bFind) {
			for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
				if (m_pGrids[i]->GetCurTagId() == strTagId) {
					m_pGrids[i]->SetCurPatientName(pDlg->m_szUIPName);
					break;
				}
			}
		}
	}

	// �����Ժ��,����ʾ	
	if (pDlg->m_out_hospital_time > 0) {

		// ���ֳ�ģʽ��
		OnHandTagErasedNotify(strTagId);
		m_cstHandImg->DelTag(strTagId);
		m_cstHandImg->SetCurTag("");

		// ���ɾ����ѡ�е�tag
		if (m_cur_selected_tag == strTagId) {
			// �������tag
			if (m_layTags->GetCount() > 0) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
		}

		// �Ӵ���ģʽ��
		for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
			if (m_pGrids[i]->GetCurTagId() == strTagId) {
				if (m_pGrids[i]->GetMode() == CModeButton::Mode_Single) {
					m_pGrids[i]->OnOutHospital();
				}
			}
		}
	}

	delete pDlg;
}

// ��ѯ����tag�İ�grid
void   CDuiFrameWnd::OnQueryBindingByTag(WPARAM wParam, LPARAM lParam) {
	TagBindingGridRet * pParam = (TagBindingGridRet *)wParam;

	std::map<std::string, CTagUI *>::iterator it;
	it = m_tags_ui.find(pParam->m_szTagId);
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		if (pTagUI) {
			pTagUI->SetBindingGridIndex(pParam->m_nGridIndex);
			assert(pParam->m_nGridIndex > 0);
			CDuiString  strPName = pTagUI->GetPTagName();
			m_pGrids[pParam->m_nGridIndex - 1]->SetPatientNameInHandMode(strPName);

			// �����ǰѡ�е�tag���޸İ󶨵�tag
			if (m_cur_selected_tag == pTagUI->GetTagId()) {
				int cnt = m_layDragDropGrids->GetCount();
				for (int i = 0; i < cnt; i++) {
					CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(i);
					if (i == pTagUI->m_nBindingGridIndex - 1) {
						pChild->SetBorderColor(CUR_BINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(CUR_BINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(CUR_BINDING_GRID_BKCOLOR);
					}
					else {
						pChild->SetBorderColor(UNBINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(UNBINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(UNBINDING_GRID_BKCOLOR);
					}
				}
			}
		}
	}

	CBusiness::GetInstance()->QueryTempByHandTagAsyn(pParam->m_szTagId, pParam->m_nGridIndex);

	assert(pParam);
	delete pParam;
}

// �Ҽ������˵�
void   CDuiFrameWnd::OnTagMenu(TNotifyUI& msg) {
	CTagUI * pTagUI = (CTagUI *)msg.pSender->GetParent();
	CDuiString s = pTagUI->GetTagId();

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_tag.xml"), pTagUI);
	pMenu->Init(*this, msg.ptMouse);
	pMenu->ShowWindow(TRUE);      
}

// ɾ��Tag
void   CDuiFrameWnd::OnDeleteTag(TNotifyUI& msg) {
	if ( IDYES == ::MessageBox(GetHWND(), "ȷ��Ҫɾ����Tagȫ��������", "ɾ��Tag", MB_YESNO | MB_DEFBUTTON2 ) ) {
		CTagUI * pTagUI = (CTagUI *)msg.pSender;
		CDuiString  strTagId = pTagUI->GetTagId();
		CBusiness::GetInstance()->DelTagAsyn(strTagId);
	}
}

// ɾ��Tag���ݽ��
void  CDuiFrameWnd::OnDelTagRet(WPARAM wParam, LPARAM lParam) {
	char * szTagId = (char *)wParam;

	OnHandTagErasedNotify(szTagId);
	m_cstHandImg->DelTag(szTagId);
	m_cstHandImg->SetCurTag("");

	// �������tag
	if (m_layTags->GetCount() > 0) {
		CControlUI * pChildUI = m_layTags->GetItemAt(0);
		OnHandTagSelected(pChildUI);
	}

	assert(szTagId);
	delete[] szTagId;
}

// ��ѯסԺ��Ϣ
void  CDuiFrameWnd::OnQueryInHospital() {
	TQueryInHospital  query;
	memset(&query, 0, sizeof(query));
	CDuiString  strText;

	strText = m_edQName->GetText();
	STRNCPY(query.m_szPName, strText, MAX_TAG_PNAME_LENGTH);

	strText = m_edQHospitalAdmissionNo->GetText();
	STRNCPY(query.m_szHospitalAdmissionNo, strText, MAX_HOSPITAL_ADMISSION_NO_LENGTH);

	strText = m_edQOutPatient->GetText();
	STRNCPY(query.m_szOutpatientNo, strText, MAX_OUTPATIENT_NO_LENGTH);

	strText = m_edQAge->GetText();
	STRNCPY(query.m_age, strText, MAX_AGE_LENGTH);

	query.m_sex = m_cmbQSex->GetCurSel();
	query.m_in_hospital_s = GetAnyDayZeroTime(SysTime2Time( m_datInHospitalStart->GetTime() ));
	query.m_in_hospital_e = GetAnyDayZeroTime(SysTime2Time( m_datInHospitalEnd->GetTime() )) + 3600 * 24;

	CBusiness::GetInstance()->QueryInHospitalAsyn(&query);

	SetQueryBusy(TRUE);	
}

// ��ѯסԺ��Ϣʱæ
void   CDuiFrameWnd::SetQueryBusy(BOOL bBusy /*= TRUE*/) {
	if (bBusy) {
		m_q_waiting_bar->SetVisible(true);
		m_q_waiting_bar->Start();
		m_edQName->SetEnabled(false);
		m_cmbQSex->SetEnabled(false);
		m_edQAge->SetEnabled(false);
		m_edQOutPatient->SetEnabled(false);
		m_edQHospitalAdmissionNo->SetEnabled(false);
		m_datInHospitalStart->SetEnabled(false);
		m_datInHospitalEnd->SetEnabled(false);
		m_btnQuery->SetEnabled(false);
		m_lstQueryRet->SetEnabled(false);
		m_btnQPrev->SetEnabled(false);
		m_btnQNext->SetEnabled(false);    
	}
	else {
		m_q_waiting_bar->Stop();
		m_q_waiting_bar->SetVisible(false);		
		m_edQName->SetEnabled(true);
		m_cmbQSex->SetEnabled(true);
		m_edQAge->SetEnabled(true);
		m_edQOutPatient->SetEnabled(true);
		m_edQHospitalAdmissionNo->SetEnabled(true);
		m_datInHospitalStart->SetEnabled(true);
		m_datInHospitalEnd->SetEnabled(true);
		m_btnQuery->SetEnabled(true);
		m_lstQueryRet->SetEnabled(true);
		//m_btnQPrev->SetEnabled(true);
		//m_btnQNext->SetEnabled(true);
	}
}

// ��ѯסԺ��Ϣ���
void  CDuiFrameWnd::OnQueryInHospitalRet(WPARAM wParam, LPARAM lParam) {
	std::vector<InHospitalItem *> * pRet = (std::vector<InHospitalItem*> *)wParam;
	ClearVector(m_vQRet);
	std::copy(pRet->begin(), pRet->end(), std::back_inserter(m_vQRet));

	SetQueryBusy(FALSE);

	CDuiString  strText;
	strText.Format("(%lu����¼)", m_vQRet.size());
	m_lblQueryRet->SetText(strText);

	m_nQCurPageIndex = 0;
	ShowInHospitalRetPage(m_nQCurPageIndex);

	delete pRet;
}

// ��ʾסԺ����ڼ�ҳ
void  CDuiFrameWnd::ShowInHospitalRetPage(int nPageIndex) {
	m_lstQueryRet->RemoveAll();

	int nCnt = m_vQRet.size();
	// û�м�¼
	if ( 0 == nCnt ) {
		m_btnQPrev->SetEnabled(false);
		m_btnQNext->SetEnabled(false);
		return;
	}

	int nMaxPage = (m_vQRet.size() - 1) / MAX_ITEMS_PER_PAGE;
	assert(nPageIndex >= 0 && nPageIndex <= nMaxPage);

	if (nPageIndex == 0) {
		if ( nMaxPage == 0 ) {
			m_btnQPrev->SetEnabled(false);
			m_btnQNext->SetEnabled(false);
		}
		else {
			m_btnQPrev->SetEnabled(false);
			m_btnQNext->SetEnabled(true);
		}
	}
	else if (nPageIndex < nMaxPage) {
		m_btnQPrev->SetEnabled(true);
		m_btnQNext->SetEnabled(true);
	}
	else {
		m_btnQPrev->SetEnabled(true);
		m_btnQNext->SetEnabled(false);
	}

	CDuiString strText;
	char szDate[256];
	for (int i = nPageIndex * MAX_ITEMS_PER_PAGE; i < (nPageIndex + 1) * MAX_ITEMS_PER_PAGE && i < nCnt; i++ ) {
		CListTextElementUI* pItem = new CListTextElementUI;
		m_lstQueryRet->Add(pItem);
		InHospitalItem * pData = m_vQRet[i];

		strText.Format("%d", i + 1);
		pItem->SetText(0, strText);

		pItem->SetText(1, pData->m_szPName);
		pItem->SetText(2, GetSexStr(pData->m_sex));
		pItem->SetText(3, pData->m_age); 
		pItem->SetText(4, pData->m_szOutpatientNo);
		pItem->SetText(5, pData->m_szHospitalAdmissionNo);
		pItem->SetText(6, LmnFormatTime(szDate, sizeof(szDate), pData->m_in_hospital, "%Y-%m-%d %H:%M"));
		pItem->SetText(7, FormatEvents(pData->m_events, pData->m_events_cnt) );
		pItem->SetTag((UINT_PTR)pData->m_szTagId);
	}

}

// ��ʽ�������¼���Ϣ
CDuiString  CDuiFrameWnd::FormatEvents(PatientEvent * events, int nEventCnt) {
	CDuiString  strText;

	for ( int i = 0; i < nEventCnt; i++ ) {
		CDuiString  strItem;
		char szDate[256];
		strItem.Format("%s %s", LmnFormatTime(szDate, sizeof(szDate), events[i].m_time_1, "%Y-%m-%d %H:%M"), GetEventTypeStr(events[i].m_nType) );
		if (strText.GetLength() == 0) {
			strText = strItem;
		}
		else {
			strText += ";";
			strText += strItem;  
		}			
	}

	return strText;
}

// סԺ��Ϣ��һҳ
void   CDuiFrameWnd::NextInHospitalPage() {
	m_nQCurPageIndex++;
	ShowInHospitalRetPage(m_nQCurPageIndex);
}

// סԺ��Ϣ��һҳ
void  CDuiFrameWnd::PrevInHospitalPage() {
	m_nQCurPageIndex--;
	ShowInHospitalRetPage(m_nQCurPageIndex);
}

// סԺ�б��Ҽ������˵�
void  CDuiFrameWnd::OnInHospitalMenu(TNotifyUI& msg) {
	int nSel = m_lstQueryRet->GetCurSel();
	if (nSel < 0)
		return;

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_inhos.xml"), m_lstQueryRet);
	pMenu->Init(*this, msg.ptMouse);
	pMenu->ShowWindow(TRUE);
}

// ��ѯ��Ժ��Ϣ
void   CDuiFrameWnd::OnQueryOutHospital() {
	TQueryOutHospital  query;
	memset(&query, 0, sizeof(query));
	CDuiString  strText;

	strText = m_edQName1->GetText();
	STRNCPY(query.m_szPName, strText, MAX_TAG_PNAME_LENGTH);

	strText = m_edQHospitalAdmissionNo1->GetText();
	STRNCPY(query.m_szHospitalAdmissionNo, strText, MAX_HOSPITAL_ADMISSION_NO_LENGTH);

	strText = m_edQOutPatient1->GetText();
	STRNCPY(query.m_szOutpatientNo, strText, MAX_OUTPATIENT_NO_LENGTH);

	strText = m_edQAge1->GetText();
	STRNCPY(query.m_age, strText, MAX_AGE_LENGTH);

	query.m_sex = m_cmbQSex1->GetCurSel();
	query.m_in_hospital_s = GetAnyDayZeroTime(SysTime2Time(m_datInHospitalStart1->GetTime()));
	query.m_in_hospital_e = GetAnyDayZeroTime(SysTime2Time(m_datInHospitalEnd1->GetTime())) + 3600 * 24;
	query.m_out_hospital_s = GetAnyDayZeroTime(SysTime2Time(m_datOutHospitalStart1->GetTime()));
	query.m_out_hospital_e = GetAnyDayZeroTime(SysTime2Time(m_datOutHospitalEnd1->GetTime())) + 3600 * 24;

	CBusiness::GetInstance()->QueryOutHospitalAsyn(&query);

	SetQuery1Busy(TRUE);
}

// ��ѯ��Ժ��Ϣʱæ
void   CDuiFrameWnd::SetQuery1Busy(BOOL bBusy /*= TRUE*/) {
	if (bBusy) {
		m_q_waiting_bar1->SetVisible(true);
		m_q_waiting_bar1->Start();
		m_edQName1->SetEnabled(false);
		m_cmbQSex1->SetEnabled(false);
		m_edQAge1->SetEnabled(false);
		m_edQOutPatient1->SetEnabled(false);
		m_edQHospitalAdmissionNo1->SetEnabled(false);
		m_datInHospitalStart1->SetEnabled(false);
		m_datInHospitalEnd1->SetEnabled(false);
		m_datOutHospitalStart1->SetEnabled(false);
		m_datOutHospitalEnd1->SetEnabled(false);
		m_btnQuery1->SetEnabled(false);
		m_lstQueryRet1->SetEnabled(false);
		m_btnQPrev1->SetEnabled(false);
		m_btnQNext1->SetEnabled(false);
	}
	else {
		m_q_waiting_bar1->Stop();
		m_q_waiting_bar1->SetVisible(false);
		m_edQName1->SetEnabled(true);
		m_cmbQSex1->SetEnabled(true);
		m_edQAge1->SetEnabled(true);
		m_edQOutPatient1->SetEnabled(true);
		m_edQHospitalAdmissionNo1->SetEnabled(true);
		m_datInHospitalStart1->SetEnabled(true);
		m_datInHospitalEnd1->SetEnabled(true);
		m_datOutHospitalStart1->SetEnabled(true);
		m_datOutHospitalEnd1->SetEnabled(true);
		m_btnQuery1->SetEnabled(true);
		m_lstQueryRet1->SetEnabled(true);
	}
}

// ��ѯ��Ժ��Ϣ���
void   CDuiFrameWnd::OnQueryOutHospitalRet(WPARAM wParam, LPARAM lParam) {
	std::vector<OutHospitalItem *> * pRet = (std::vector<OutHospitalItem*> *)wParam;
	ClearVector(m_vQRet1);
	std::copy(pRet->begin(), pRet->end(), std::back_inserter(m_vQRet1));

	SetQuery1Busy(FALSE);

	CDuiString  strText;
	strText.Format("(%lu����¼)", m_vQRet1.size());
	m_lblQueryRet1->SetText(strText);

	m_nQCurPageIndex1 = 0;
	ShowOutHospitalRetPage(m_nQCurPageIndex1);

	delete pRet;
}

// ��ʾ��Ժ����ڼ�ҳ
void  CDuiFrameWnd::ShowOutHospitalRetPage(int nPageIndex) {
	m_lstQueryRet1->RemoveAll();

	int nCnt = m_vQRet1.size();
	// û�м�¼
	if (0 == nCnt) {
		m_btnQPrev1->SetEnabled(false);
		m_btnQNext1->SetEnabled(false);
		return;
	}

	int nMaxPage = (m_vQRet1.size() - 1) / MAX_ITEMS_PER_PAGE;
	assert(nPageIndex >= 0 && nPageIndex <= nMaxPage);

	if (nPageIndex == 0) {
		if (nMaxPage == 0) {
			m_btnQPrev1->SetEnabled(false);
			m_btnQNext1->SetEnabled(false);
		}
		else {
			m_btnQPrev1->SetEnabled(false);
			m_btnQNext1->SetEnabled(true);
		}
	}
	else if (nPageIndex < nMaxPage) {
		m_btnQPrev1->SetEnabled(true);
		m_btnQNext1->SetEnabled(true);
	}
	else {
		m_btnQPrev1->SetEnabled(true);
		m_btnQNext1->SetEnabled(false);
	}

	CDuiString strText;
	char szDate[256];
	for (int i = nPageIndex * MAX_ITEMS_PER_PAGE; i < (nPageIndex + 1) * MAX_ITEMS_PER_PAGE && i < nCnt; i++) {
		CListTextElementUI* pItem = new CListTextElementUI;
		m_lstQueryRet1->Add(pItem);
		OutHospitalItem * pData = m_vQRet1[i];

		strText.Format("%d", i + 1);
		pItem->SetText(0, strText);

		pItem->SetText(1, pData->m_szPName);
		pItem->SetText(2, GetSexStr(pData->m_sex));
		pItem->SetText(3, pData->m_age);
		pItem->SetText(4, pData->m_szOutpatientNo);
		pItem->SetText(5, pData->m_szHospitalAdmissionNo);
		pItem->SetText(6, LmnFormatTime(szDate, sizeof(szDate), pData->m_in_hospital, "%Y-%m-%d %H:%M"));
		pItem->SetText(7, LmnFormatTime(szDate, sizeof(szDate), pData->m_out_hospital, "%Y-%m-%d %H:%M"));
		pItem->SetText(8, FormatEvents(pData->m_events, pData->m_events_cnt));
		pItem->SetTag((UINT_PTR)pData->m_szTagId);
	}
}

// ��Ժ��Ϣ��һҳ
void   CDuiFrameWnd::NextOutHospitalPage() {
	m_nQCurPageIndex1++;
	ShowOutHospitalRetPage(m_nQCurPageIndex1);
}

// ��Ժ��Ϣ��һҳ
void   CDuiFrameWnd::PrevOutHospitalPage() {
	m_nQCurPageIndex1--;
	ShowOutHospitalRetPage(m_nQCurPageIndex1);
}

// ��Ժ�б��Ҽ������˵�
void   CDuiFrameWnd::OnOutHospitalMenu(TNotifyUI& msg) {
	int nSel = m_lstQueryRet1->GetCurSel();
	if (nSel < 0)
		return;

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_outhos.xml"), m_lstQueryRet1);
	pMenu->Init(*this, msg.ptMouse);
	pMenu->ShowWindow(TRUE);
}



// ����������״̬֪ͨ
void   CDuiFrameWnd::OnLauchStatusNotify(CLmnSerialPort::PortStatus e) {
	::PostMessage( GetHWND(), UM_LAUNCH_STATUS, (WPARAM)e, 0);
}      

// ���������Զ�ȡ���ж��������¶�����
void   CDuiFrameWnd::OnTrySurReaderNotify(WORD wBed, BOOL bStart) {
	::PostMessage(GetHWND(), UM_TRY_SUR_READER, (WPARAM)wBed, bStart);
}

// ���յ����ж���������״̬��֪ͨ
void   CDuiFrameWnd::OnSurReaderStatusNotify(WORD wBed, BOOL bConnected) {
	::PostMessage(GetHWND(), UM_SUR_READER_STATUS, (WPARAM)wBed, bConnected);
}

// ���ж��������¶�����
void   CDuiFrameWnd::OnSurReaderTempNotify(WORD wBed, const TempItem & item) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));
	::PostMessage(GetHWND(), UM_SUR_READER_TEMP, (WPARAM)wBed, (LPARAM)pItem );
}

// ��ѯ���¶Ƚ��(����TagId)
void   CDuiFrameWnd::OnQueryTempRetNotify(const char * szTagId, WORD wBed, std::vector<TempItem*> * pvRet) {
	void ** pParam = new void *[3];

	char * pTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(pTagId, szTagId, MAX_TAG_ID_LENGTH);

	pParam[0] = (void *)pTagId;
	pParam[1] = (void *)wBed;
	pParam[2] = (void *)pvRet;

	::PostMessage(GetHWND(), UM_QUERY_TEMP_BY_TAG_ID_RET, (WPARAM)pParam, 0);
}

// �ֳֶ��������¶�����
void   CDuiFrameWnd::OnHandReaderTempNotify(const TempItem & item, const char * tag_patient_name) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));

	assert(tag_patient_name);
	char * new_tag_patient_name = new char[MAX_TAG_PNAME_LENGTH];
	STRNCPY(new_tag_patient_name, tag_patient_name, MAX_TAG_PNAME_LENGTH);

	::PostMessage(GetHWND(), UM_HAND_READER_TEMP, (WPARAM)pItem, (LPARAM)new_tag_patient_name);
}

// ���ݿ���Ҫ�������Ѿ���ѯ���
void  CDuiFrameWnd::OnPreparedNotify() {
	::PostMessage(GetHWND(), UM_PREPARED, 0, 0);
}

// ��ѯ���������ֳ�Tag�¶�����
void   CDuiFrameWnd::OnAllHandTagTempDataNotify(std::vector<HandTagResult *> * pvRet) {
	::PostMessage(GetHWND(), UM_ALL_HAND_TAG_TEMP_DATA, (WPARAM)pvRet, 0);
}

// ɾ������ʱ���ֳ�Tag
void   CDuiFrameWnd::OnHandTagErasedNotify(const char * szTagId) {
	assert(szTagId);

	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find(szTagId);
	// ����ҵ���
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		int nBindingGridIndex = pTagUI->GetBindingGridIndex();
		if (nBindingGridIndex > 0 ) {
			m_pGrids[nBindingGridIndex - 1]->OnReleaseTagBinding();
		}
		m_tags_ui.erase(it);

		if (pTagUI) {
			m_layTags->Remove(pTagUI);
		}
	}
}

// �󶨽����֪ͨ
void   CDuiFrameWnd::OnBindingRetNotify(const TagBindingGridRet & item) {
	TagBindingGridRet * pParam = new TagBindingGridRet;
	memcpy( pParam, &item, sizeof(TagBindingGridRet) );
	::PostMessage( GetHWND(), UM_BINDING_TAG_GRID_RET, (WPARAM)pParam, 0 );
}

// ��ѯ�ֳ�Tag���֪ͨ
void   CDuiFrameWnd::OnQueryHandTagRetNotify(const char * szTagId, int nGridIndex, std::vector<TempItem*> * pvRet) {
	void ** pParam = new void *[3];

	char * pTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(pTagId, szTagId, MAX_TAG_ID_LENGTH);

	pParam[0] = (void *)pTagId;
	pParam[1] = (void *)nGridIndex;
	pParam[2] = (void *)pvRet;

	::PostMessage(GetHWND(), UM_QUERY_HAND_TEMP_BY_TAG_ID_RET, (WPARAM)pParam, 0);
}

// ��ѯtag�İ�grid id
void  CDuiFrameWnd::OnQueryBindingByTagRetNotify(const TagBindingGridRet & ret) {
	TagBindingGridRet * pParam = new TagBindingGridRet;
	memcpy(pParam, &ret, sizeof(TagBindingGridRet));
	::PostMessage(GetHWND(), UM_QUERY_BINDING_BY_TAG_RET, (WPARAM)pParam, 0);
}

// ɾ��Tag
void  CDuiFrameWnd::OnDelTagRetNotify(const char * szDelTagId) {
	char * szTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(szTagId, szDelTagId, MAX_TAG_ID_LENGTH);
	::PostMessage(GetHWND(), UM_DEL_TAG_RET, (WPARAM)szTagId, 0);
}

void CDuiFrameWnd::OnQueryInHospitalNotify(const std::vector<InHospitalItem*>& vRet) {
	std::vector<InHospitalItem*>::const_iterator it;
	std::vector<InHospitalItem*> * pRet = new std::vector<InHospitalItem*>;
	for ( it = vRet.begin(); it != vRet.end(); ++it ) {
		InHospitalItem* pItem = *it;
		InHospitalItem* pNewItem = new InHospitalItem;
		memcpy(pNewItem, pItem, sizeof(InHospitalItem));
		pRet->push_back(pNewItem);
	}
	::PostMessage(GetHWND(), UM_QUERY_INHOSPITAL_RET, (WPARAM)pRet, 0);
}

// ��ѯ��Ժ��Ϣ�Ľ��֪ͨ
void CDuiFrameWnd::OnQueryOutHospitalNotify(const std::vector<OutHospitalItem*>& vRet) {
	std::vector<OutHospitalItem*>::const_iterator it;
	std::vector<OutHospitalItem*> * pRet = new std::vector<OutHospitalItem*>;
	for (it = vRet.begin(); it != vRet.end(); ++it) {
		OutHospitalItem* pItem = *it;
		OutHospitalItem* pNewItem = new OutHospitalItem;
		memcpy(pNewItem, pItem, sizeof(OutHospitalItem));
		pRet->push_back(pNewItem);
	}
	::PostMessage(GetHWND(), UM_QUERY_OUTHOSPITAL_RET, (WPARAM)pRet, 0);
}

bool CDuiFrameWnd::OnMain1Size(void * pParam) {
	int w = m_layMain1->GetWidth();
	m_layRight->SetMaxWidth(w);
	return true;
}

void   CDuiFrameWnd::OnChecComPortsRet(WPARAM wParam, LPARAM lParam) {
	std::vector<int> * pvRet = (std::vector<int> *)wParam;
	assert(pvRet);

	if (pvRet->size() == 0) {
		m_lblLaunchStatus->SetText("û�д��κν��ջ�վ");
	}
	else {
		std::vector<int>::iterator it;
		CDuiString strText = "����";
		int i = 0;
		for (it = pvRet->begin(); it != pvRet->end(); ++it, i++) {
			if (i > 0) {
				strText += ",";
			}
			CDuiString item;
			item.Format("com%d", *it);
			strText += item;
		}
		m_lblLaunchStatus->SetText(strText);
	}

	if (pvRet) {
		delete pvRet;
	}
}

void   CDuiFrameWnd::OnAddCubeItem() {
	CDuiString  strBedNo;
	CDuiString  strName;
	CDuiString  strPhone;

	strBedNo = m_edtBedNo1->GetText();
	strName = m_edtName1->GetText();
	strPhone = m_edtPhone1->GetText();

	strBedNo.Trim();
	strName.Trim();
	strPhone.Trim();

	int  nBedNo = 0;
	int ret = sscanf(strBedNo, " %d", &nBedNo);
	if ( 1 != ret ) {
		MessageBox(GetHWND(), "�����봲��!", "����", 0);
		return;
	}

	if (strName.GetLength() == 0) {
		MessageBox(GetHWND(), "����������!", "����", 0);
		return;
	}

	std::vector<CubeItem * >::iterator it = std::find_if(m_cube_items.begin(), 
		m_cube_items.end(), FindCubeItemObj(nBedNo));
	// �ҵ���ͬ���ŵ�
	if ( it != m_cube_items.end() ) {
		if (IDNO == MessageBox( GetHWND(), "�Ѿ�����ͬ���Ĵ�λ�š�Ҫ����ԭ��λ�ŵ�����������",
			                    "�����´�λ", MB_YESNO | MB_DEFBUTTON2)) {
			return;
		}
	}

	CBusiness::GetInstance()->SaveCubeBedAsyn(nBedNo, strName, strPhone);

	m_edtBedNo1->SetText("");
	m_edtName1->SetText("");
	m_edtPhone1->SetText("");
}

void   CDuiFrameWnd::OnGetAllCubeItems(WPARAM wParam, LPARAM  lParam) {
	std::vector<CubeItem*> * pvRet = (std::vector<CubeItem*> *)wParam;
	assert(pvRet);

	std::copy(pvRet->begin(), pvRet->end(), std::back_inserter(m_cube_items));
	UpdateCubeItems();  

	std::copy_if(pvRet->begin(), pvRet->end(), std::back_inserter(m_cube_items_high), CopyCubeHighItem(g_data.m_nCubeHighTemp));
	UpdateCubeItemsHigh();

	delete pvRet;
}

void   CDuiFrameWnd::OnAddCubeItemRet(WPARAM wParam, LPARAM  lParam) {
	CubeItem * pItem = (CubeItem *)wParam;

	std::vector<CubeItem * >::iterator it;
	it = std::find_if(m_cube_items.begin(), m_cube_items.end(), FindCubeItemObj(pItem->nBedNo));

	// ��������
	if ( it == m_cube_items.end() ) {
		m_cube_items.push_back(pItem);
	}
	else {
		CubeItem * p = *it;
		memcpy(p, pItem, sizeof(CubeItem));
		delete pItem;
	}

	UpdateCubeItems();

	it = std::find_if(m_cube_items_high.begin(), m_cube_items_high.end(), FindCubeItemObj(pItem->nBedNo));
	if ( it != m_cube_items_high.end() ) {
		CubeItem * p = *it;
		delete p;
		m_cube_items_high.erase(it);
		UpdateCubeItemsHigh();
	}
}

void   CDuiFrameWnd::UpdateCubeItemsHigh() {
	int nItemsCnt = m_cube_items_high.size();
	int nUiItemsCnt = m_CubeItems_high_temp->GetCount();
	int nDiff = 0;

	if (nItemsCnt < nUiItemsCnt) {
		nDiff = nUiItemsCnt - nItemsCnt;
		for (int i = 0; i < nDiff; i++) {
			m_CubeItems_high_temp->RemoveAt(0);
		}
	}
	else {
		nDiff = nItemsCnt - nUiItemsCnt;
		for (int i = 0; i < nDiff; i++) {
			CCubeItemUI * item = new CCubeItemUI;
			m_CubeItems_high_temp->Add(item);
		}
	}

	nUiItemsCnt = m_CubeItems_high_temp->GetCount();
	// ����һ����ͬ
	assert(nItemsCnt == m_CubeItems_high_temp->GetCount());

	for (int i = 0; i < nUiItemsCnt; i++) {
		CCubeItemUI * item = (CCubeItemUI *)m_CubeItems_high_temp->GetItemAt(i);
		CubeItem * p = m_cube_items_high[i];

		item->SetBedNo(p->nBedNo);
		item->SetPatientName(p->szName);
		item->SetPhone(p->szPhone);
		item->SetTemperature(p->nTemp);
		item->SetTime(p->time);
		item->SetBinding((p->szTagId[0] == '\0') ? FALSE : TRUE);

		if (i % 2 == 0) {
			item->SetBkColor(CUBE_ALTERNATIVE_BKCOLOR);
		}
		else {
			item->SetBkColor(0);
		}
	}
}

void   CDuiFrameWnd::UpdateCubeItems() {
	int nItemsCnt = m_cube_items.size();
	int nUiItemsCnt = m_CubeItems->GetCount();
	int nDiff = 0;

	if ( nItemsCnt < nUiItemsCnt ) {		
		nDiff = nUiItemsCnt - nItemsCnt;
		for (int i = 0; i < nDiff; i++) {
			m_CubeItems->RemoveAt(0);
		}
	}
	else {
		nDiff = nItemsCnt - nUiItemsCnt;
		for (int i = 0; i < nDiff; i++) {
			CCubeItemUI * item = new CCubeItemUI;
			m_CubeItems->Add(item);
		}
	}
	
	nUiItemsCnt = m_CubeItems->GetCount();
	// ����һ����ͬ
	assert(nItemsCnt == m_CubeItems->GetCount());
	
	for ( int i = 0; i < nUiItemsCnt; i++ ) {
		CCubeItemUI * item = (CCubeItemUI *)m_CubeItems->GetItemAt(i);
		CubeItem * p = m_cube_items[i];

		item->SetBedNo(p->nBedNo);
		item->SetPatientName(p->szName);
		item->SetPhone(p->szPhone);
		item->SetTemperature(p->nTemp);
		item->SetTime(p->time);
		item->SetBinding( (p->szTagId[0] == '\0') ? FALSE :TRUE);

		if ( i % 2 == 0 ) {
			item->SetBkColor(CUBE_ALTERNATIVE_BKCOLOR);
		}
		else {
			item->SetBkColor(0);
		}
	}

}

void   CDuiFrameWnd::UpdateNewTag() {
	m_CurTag->SetTemperature(m_cur_tag.nTemp);
	m_CurTag->SetTime(m_cur_tag.time);
	m_CurTag->m_strTagId = m_cur_tag.szTagId;
}

void   CDuiFrameWnd::OnCubeTempItem(WPARAM wParam, LPARAM  lParam) {
	TempItem * pItem = (TempItem *)wParam;

	std::vector<CubeItem * >::iterator it;
	it = std::find_if(m_cube_items.begin(), m_cube_items.end(), FindCubeItemByTag(pItem->m_szTagId));
	// �������Tag
	if ( it == m_cube_items.end() ) {
		m_cur_tag.nTemp = pItem->m_dwTemp;
		m_cur_tag.time = pItem->m_time;
		STRNCPY( m_cur_tag.szTagId, pItem->m_szTagId, sizeof(m_cur_tag.szTagId) );
		UpdateNewTag();
		m_CurTag->Refresh();
		SetTimer( GetHWND(), TIMER_NEW_TAG, INTERVAL_TIMER_NEW_TAG, 0 );
	}
	else {
		CubeItem * pFindItem = *it;
		pFindItem->nTemp = pItem->m_dwTemp;
		pFindItem->time = pItem->m_time;
		UpdateCubeItems();

		// ����Ǹ���
		if ( (int)pItem->m_dwTemp >= g_data.m_nCubeHighTemp ) {
			it = std::find_if( m_cube_items_high.begin(), m_cube_items_high.end(), 
				               FindCubeItemObj(pFindItem->nBedNo));
			// ���������û��
			if ( it == m_cube_items_high.end() ) {
				m_cube_items_high.push_back(pFindItem);
			}
		}
		// ����ǵ���
		else {
			it = std::find_if(m_cube_items_high.begin(), m_cube_items_high.end(),
				FindCubeItemObj(pFindItem->nBedNo));
			// �����������
			if (it != m_cube_items_high.end()) {
				m_cube_items_high.erase(it);
			}
		}
		UpdateCubeItemsHigh();

		// �����ǰ�����״̬
		if ( m_nMaxCubeBedNo > 0 && m_nMaxCubeBedNo == pFindItem->nBedNo ) {
			m_CubeImg->AddCubeTemp(pItem);
		}

		// ���浽���ݿ�
		CBusiness::GetInstance()->SaveCubeTempAsyn(pFindItem->nBedNo, pItem->m_dwTemp, pItem->m_time);
	}

	delete pItem;
}

void   CDuiFrameWnd::OnNewTagTimer() {
	m_CurTag->Fade();

	KillTimer(GetHWND(), TIMER_NEW_TAG);
}

void   CDuiFrameWnd::OnCubeBindingTag(WPARAM wParam, LPARAM  lParam) {
	int    nBedNo = wParam;
	char * szTagId = (char *)lParam;

	std::vector<CubeItem * >::iterator it;
	it = std::find_if(m_cube_items.begin(), m_cube_items.end(), FindCubeItemObj(nBedNo));
	assert(it != m_cube_items.end());
	if (it != m_cube_items.end()) {
		CubeItem * pItem = *it;
		STRNCPY(pItem->szTagId, szTagId, sizeof(pItem->szTagId));
		UpdateCubeItems();
	}

	m_cur_tag.nTemp = 0;
	m_cur_tag.szTagId[0] = '\0';
	m_cur_tag.time = 0;
	UpdateNewTag();

	delete szTagId;
}

void   CDuiFrameWnd::OnQueryCubeTempRet(WPARAM wParam, LPARAM  lParam) {
	std::vector<CubeTempItem *> * pvRet = (std::vector<CubeTempItem *> *)wParam;

	m_CubeImg->LoadCubeData(*pvRet);
	m_CubeImg->MyInvalidate();

	ClearVector(*pvRet);
	delete pvRet;
}

void   CDuiFrameWnd::UpdateSelectedCubeItem_high_temp() {
	int nCnt = m_CubeItems_high_temp->GetCount();
	for (int i = 0; i < nCnt; i++) {
		CCubeItemUI * pItem = (CCubeItemUI *)m_CubeItems_high_temp->GetItemAt(i);
		if (pItem == m_SelectedCubeItem_high_temp) {
			pItem->SetBkColor(0xFF007ACC);
		}
		else {
			if (i % 2 == 0) {
				pItem->SetBkColor(CUBE_ALTERNATIVE_BKCOLOR);
			}
			else {
				pItem->SetBkColor(0);
			}
		}
	}
}

void   CDuiFrameWnd::UpdateSelectedCubeItem() {
	int nCnt = m_CubeItems->GetCount();
	for (int i = 0; i < nCnt; i++) {
		CCubeItemUI * pItem = (CCubeItemUI *)m_CubeItems->GetItemAt(i);
		if ( pItem == m_SelectedCubeItem ) {
			pItem->SetBkColor(0xFF007ACC);
		}
		else {
			if (i % 2 == 0) {
				pItem->SetBkColor(CUBE_ALTERNATIVE_BKCOLOR);
			}
			else {
				pItem->SetBkColor(0);
			}
		}
	}
}

void   CDuiFrameWnd::OnCubeItemClick(TNotifyUI& msg) {
	CCubeItemUI * pSelectedItem = (CCubeItemUI *)msg.pSender;
	CControlUI * pParent = pSelectedItem->GetParent();
	if (pParent == m_CubeItems) {
		m_SelectedCubeItem = pSelectedItem;
		UpdateSelectedCubeItem();
	}
	else if (pParent == m_CubeItems_high_temp) {
		m_SelectedCubeItem_high_temp = pSelectedItem;
		UpdateSelectedCubeItem_high_temp();
	}
}

void   CDuiFrameWnd::OnCubeItemMenu(TNotifyUI& msg) {
	OnCubeItemClick(msg);
	
	m_MenuCuteItem = (CCubeItemUI *)msg.pSender;

	POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_cube_item.xml"), m_MenuCuteItem);

	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);

	//CCubeItemDlg * pDlg = new CCubeItemDlg;
	//pDlg->Create(this->m_hWnd, _T("�޸Ĳ���"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	//pDlg->CenterWindow();
	//int ret = pDlg->ShowModal();

	//// �������click ok
	//if (0 != ret) {
	//	delete pDlg;
	//	return;
	//}

	//delete pDlg;
}


CCubeItemDlg::CCubeItemDlg() {

}

void   CCubeItemDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void   CCubeItemDlg::InitWindow() {
	WindowImplBase::InitWindow();
}

                      

  
                                               
void PrintStatus(int nCnt, void * args[]) {
	CBusiness::GetInstance()->PrintStatusAsyn();
}
            
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	SetUnhandledExceptionFilter(pfnUnhandledExceptionFilter);

	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handleΪ������HANDLE���͵�ȫ�ֱ��� 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		::MessageBox(0, "�����Ѿ��򿪻�û�йر���ȫ�����ȹرջ�ȴ���ȫ�ر�!", "����", 0);
		return 0;
	}

	CoInitialize(NULL);
	JTelSvrRegCommand("status", "print status", PrintStatus, 0);
	DWORD  dwPort = 2019;
	JTelSvrStart((unsigned short)dwPort, 10);

	g_data.m_nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
	g_data.m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);


	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	CBusiness::GetInstance()->InitSigslot(duiFrame);


	// ����icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");
	g_data.m_bClosing = TRUE;

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();
	CoUninitialize();

	return 0;
}
            

