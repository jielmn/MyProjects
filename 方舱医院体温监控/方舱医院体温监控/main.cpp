// win32_1.cpp : 定义应用程序的入口点。
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

// 1分钟定时器
#define   TIMER_60_SECONDS                       1002
#define   INTERVAL_TIMER_60_SECONDS              60000

// 更新新tag颜色定时器
#define   TIMER_NEW_TAG                          1101
#define   INTERVAL_TIMER_NEW_TAG                 5000

//自定义事件排序
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
	// 清空子控件
	int nCnt = m_layMain->GetCount();
	for (int i = 0; i < nCnt; i++) {
		m_layMain->RemoveAt(0, true);
	}
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	RemoveAllGrids();

	// 销毁grids
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

	/*************  获取控件 *****************/
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

	// 添加窗格
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

	// 住院信息
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

	// 出院信息
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


	/////////////////// 方舱
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
		s.Format("病人%d", i + 1);
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
		item->SetPatientName("张三1");
		item->SetPhone("123xyz");
		item->SetTemperature(3600 + i * 10);
		item->SetTime(now);
		m_CubeItems_high_temp->Add(item);
	}
#endif
                  
	//////////// 

	
	/*************  end 获取控件 *****************/

	m_cstHandImg->m_sigTagErased.connect(this, &CDuiFrameWnd::OnHandTagErasedNotify);

	RefreshGridsPage();

	m_hand_tabs->SelectItem(1);	 

	// 放在prepared后
	//CheckDevice();
	CBusiness::GetInstance()->PrepareAsyn();

	/***** 启动定时器 ****/
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
			// 如果是默认排序，把绑定的放在前面
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
	// 根据手持读卡器的tag打印体温单
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
	// layWindow的左右margin
	DWORD dwWidth = LOWORD(lParam) - 10 * 2;           
	// 30是底部status的高度, 32是标题栏高度, 85是tab页高度
	DWORD dwHeight = HIWORD(lParam) - 30 - 1 * 2 - 32 - 85;
	RefreshGridsSize(dwWidth, dwHeight);

	// 以后可以用以下这种方式，添加控件OnSize回调函数
	// m_Control->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMySize);
	// 函数原型：bool OnMySize(void * pParam);
	ResetDragdropGrids(dwWidth - 500, dwHeight);

	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(0, pt);
	// 如果没有点击到任何控件
	if (0 == pCtl) {
		return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
	}

	// 保存原始点击的控件
	CControlUI* pOriginalCtl = pCtl;
	int nSelTab = m_tabs->GetCurSel();

	// 如果是第一Tab页
	if ( nSelTab == TAB_INDEX_MONITOR ) {
		//// 如果是多格子状态
		//if (m_eGridStatus == GRID_STATUS_GRIDS) {
		//	while (pCtl) {
		//		if (pCtl->GetName() == GRID_NAME) {
		//			// 如果不是点击修改名字按钮
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
		//// 如果是最大化状态
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
				// 如果当前有新tag，才开始拖动
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
				// 不是点击了按钮
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
	//// 正在拖动tag ui
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
	//// 正在拖动tag ui
	//else if (m_dragdrop_tag.GetLength() > 0) {
	//	dwCurTick = LmnGetTickCount();
	//	// 超过1秒
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

// 移动格子，调整顺序
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

	// 高亮经过的格子
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

	// 高亮经过的格子
	OnMoveNewTag(pt);
}

// 格子移动过程中，经过的格子要高亮
void   CDuiFrameWnd::OnMoveGrid(const POINT & pt) {
	RECT rect   = m_layMain->GetPos();
	int nWidth  = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;

	if ( nWidth <= 0 || nHeight <= 0 ) {
		return;
	}

	assert(g_data.m_CfgData.m_dwLayoutColumns > 0);
	assert(g_data.m_CfgData.m_dwLayoutRows > 0);

	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 当前页的最大grid index
	DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage < g_data.m_CfgData.m_dwLayoutGridsCnt
		? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

	// 超出了范围
	if ( pt.x <= rect.left || pt.x >= rect.right || pt.y <= rect.top || pt.y >= rect.bottom ) {	
		if (m_nDgDestIndex >= 0) {
			m_pGrids[g_data.m_CfgData.m_GridOrder[m_nDgDestIndex]]->SetBorderColor(GRID_BORDER_COLOR);
		}
		m_nDgDestIndex = -1;

		// 如果是垂直方向超出范围，水平方向没有超出
		if ( pt.x > rect.left && pt.x < rect.right ) {
			// 进入了确定区域，关闭Timer
			if (m_bDragTimer) {
				KillTimer( GetHWND(), TIMER_DRAG_DROP_GRID );
				m_bDragTimer = FALSE;
			}
		}
		// 如果是水平方向超出范围，垂直方向没有超出
		else if (pt.y > rect.top && pt.y < rect.bottom) {
			// 可能上下翻页，需要开启定时器
			// 向前翻页
			if ( pt.x <= rect.left ) {
				// 如果可以向前翻页
				if ( dwGridPageIndex > 0) {
					// 如果没有开启定时器
					if (!m_bDragTimer) {
						SetTimer(GetHWND(), TIMER_DRAG_DROP_GRID, INTERVAL_TIMER_DRAG_DROP_GRIDS, 0);
						m_bDragTimer = TRUE;
						m_bFlipPrevPage = TRUE;
					}
				}				
			}
			// 向后翻页
			else if (pt.x >= rect.right) {
				// 可以向后翻页
				if ( dwMaxGridIndex < g_data.m_CfgData.m_dwLayoutGridsCnt - 1 ) {
					// 如果没有开启定时器
					if (!m_bDragTimer) {
						SetTimer(GetHWND(), TIMER_DRAG_DROP_GRID, INTERVAL_TIMER_DRAG_DROP_GRIDS, 0);
						m_bDragTimer = TRUE;
						m_bFlipPrevPage = FALSE;
					}
				}
			}
		}
		// 水平和垂直方向都超出范围
		else {
			// 进入了确定区域，关闭Timer
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

// 停止移动格子，确定最终位置
void  CDuiFrameWnd::StopMoveGrid() {
	m_dragdropGrid->SetVisible(false);
	KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
	m_bDragTimer = FALSE;

	// 没有选定要拖动的位置
	if (m_nDgDestIndex < 0) {
		m_nDgSourceIndex = -1;
		m_nDgDestIndex = -1;
		return;
	}

	// 取消高亮的border
	assert(m_nDgDestIndex < (int)g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[g_data.m_CfgData.m_GridOrder[m_nDgDestIndex]]->SetBorderColor(GRID_BORDER_COLOR);

	// 如果source和dest相等
	if (m_nDgSourceIndex == m_nDgDestIndex) {
		m_nDgSourceIndex = -1;
		m_nDgDestIndex = -1;
		return;
	}

	// 例如：grid 2拖到grid 4位置
	// 新位置排序: 1, 3, 4, 2
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

	// 如果没有移动到任何控件上
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

// 拖动格子过程中，拖到最左或最右，导致翻页
void  CDuiFrameWnd::OnFlipPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 当前页的最大grid index
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

// 更新grids一页(比如点击了"上一页", "下一页"，初始化等等)
void   CDuiFrameWnd::RefreshGridsPage() {
	// 先清空所有的grids
	RemoveAllGrids();

	/**** 再添加需要的grids  ***/

	// 多格子状态
	if (m_eGridStatus == GRID_STATUS_GRIDS) {
		// 每页多少grids
		DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
		// 当前页index
		DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
		// 当前页的第一个grid index
		DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
		// 当前页的最大grid index
		DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage  < g_data.m_CfgData.m_dwLayoutGridsCnt
			? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

		// 设定固定列数
		m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);
		for (DWORD i = dwFirstGridIndexCurPage; i < dwMaxGridIndex; i++) {
			assert(g_data.m_CfgData.m_GridOrder[i] < g_data.m_CfgData.m_dwLayoutGridsCnt);
			m_layMain->Add(m_pGrids[g_data.m_CfgData.m_GridOrder[i]]);
			// m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetVisible(true);
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetInternVisible(true);
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetView(0);
		}

		// 如果需要多页显示
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
		// 设定固定列数
		m_layMain->SetFixedColumns(1);
		// 添加单个窗格
		m_layMain->Add(m_pGrids[g_data.m_CfgData.m_GridOrder[m_dwCurSelGridIndex]]);
		// 关闭上下页
		m_layPages->SetVisible(false);
	}
	
}

// 更新grids size
// width, height: m_layMain的parent的
void   CDuiFrameWnd::RefreshGridsSize(int width, int height) {
	if (0 == m_layMain)
		return;

	SIZE s;
	CContainerUI * pParent = (CContainerUI *)m_layMain->GetParent();

	if ( GRID_STATUS_GRIDS == m_eGridStatus ) { 

		// 如果需要多页显示
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
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 下一页的第一grid index，并修改当前的grid index
	m_dwCurSelGridIndex = dwFirstGridIndexCurPage + dwCntPerPage;

	RefreshGridsPage();
}

void   CDuiFrameWnd::PrevPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;

	// 上一页的第一grid index，并修改当前的grid index
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
	// 如果双击一些按钮，滑动条类的，不处理
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
				// 找到序号
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
			// 清空image的温度数据
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

// 点击了“菜单”按钮
void   CDuiFrameWnd::OnBtnMenu(TNotifyUI& msg) {
	RECT r = m_btnMenu->GetPos();
	POINT pt = { r.left, r.bottom };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

// 点击了“设置”
void  CDuiFrameWnd::OnSetting() {
	CDuiString  strText;
	DWORD  dwValue = 0;
	BOOL   bValue = FALSE;

	CfgData  oldData = g_data.m_CfgData;
	CSettingDlg * pSettingDlg = new CSettingDlg;
	pSettingDlg->m_data = g_data.m_CfgData;
	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// 如果不是click ok
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

	// 报警声音开关
	bValue = DEFAULT_ALARM_VOICE_SWITCH;
	g_data.m_cfg->SetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, &bValue);

	// 自动保存excel
	bValue = FALSE;
	g_data.m_cfg->SetBooleanConfig(CFG_AUTO_SAVE_EXCEL, g_data.m_CfgData.m_bAutoSaveExcel, &bValue);

	// 十字锚
	bValue = FALSE;
	g_data.m_cfg->SetBooleanConfig(CFG_CROSS_ANCHOR, g_data.m_CfgData.m_bCrossAnchor, &bValue);

	// 手持最低显示温度
	dwValue = DEFAULT_MIN_TEMP_IN_SHOW;
	g_data.m_cfg->SetConfig(CFG_HAND_MIN_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMinTemp, &dwValue);
	// 手持最高显示温度
	dwValue = DEFAULT_MAX_TEMP_IN_SHOW;
	g_data.m_cfg->SetConfig(CFG_HAND_MAX_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMaxTemp, &dwValue);

	// 手持低温报警
	dwValue = DEFAULT_LOW_TEMP_ALARM;
	g_data.m_cfg->SetConfig(CFG_HAND_LOW_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderLowTempAlarm, &dwValue);
	// 手持高温报警
	dwValue = DEFAULT_HIGH_TEMP_ALARM;
	g_data.m_cfg->SetConfig(CFG_HAND_HIGH_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderHighTempAlarm, &dwValue);

	for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
		SaveGrid(i);
	}

	// 多余的grid配置删除
	for (DWORD i = g_data.m_CfgData.m_dwLayoutGridsCnt; i < MAX_GRID_COUNT; i++) {
		RemoveGridCfg(i);
	}

	// 格子数目改变
	if ( oldData.m_dwLayoutGridsCnt != g_data.m_CfgData.m_dwLayoutGridsCnt ) {
		g_data.m_cfg->RemoveConfig(CFG_GRIDS_ORDER);

		// 多余的格子的数据要删除
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
	// 格子数目不变
	else {
		// 行，列改变
		if (oldData.m_dwLayoutColumns != g_data.m_CfgData.m_dwLayoutColumns
			|| oldData.m_dwLayoutRows != g_data.m_CfgData.m_dwLayoutRows) {
			m_eGridStatus = GRID_STATUS_GRIDS;
			RefreshGridsPage();
			RefreshGridsSize();
		}		

		DWORD  dwDelay = 0;
		for ( DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++ ) {
			// 如果定时采集间隔改变
			if (oldData.m_GridCfg[i].m_dwCollectInterval != g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval) {
				CBusiness::GetInstance()->GetGridTemperatureAsyn(i, dwDelay);
				dwDelay += 200;
			}
		}
	}

	g_data.m_cfg->Save();
	::InvalidateRect(GetHWND(), 0, TRUE);
	delete pSettingDlg;


	// 多余的grid删除手持tag绑定
	for ( DWORD i = g_data.m_CfgData.m_dwLayoutGridsCnt; i < oldData.m_dwLayoutGridsCnt; i++ ) {
		CBusiness::GetInstance()->RemoveGridBindingAsyn(i+1);		
	}

	// 界面上删除绑定
	int nTagCnt = m_layTags->GetCount();
	for (int i = 0; i < nTagCnt; i++) {
		CTagUI * pTagUI = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pTagUI->m_nBindingGridIndex > (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
			pTagUI->SetBindingGridIndex(0);
			break;
		}
	}
}

// 保存格子配置
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

// 删除格子配置
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
 
// 点击了“关于”
void  CDuiFrameWnd::OnAbout() {   
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	pAboutDlg->ShowModal();

	delete pAboutDlg;
}

// 点击了“帮助”
void   CDuiFrameWnd::OnHelp() {
	CHelpDlg * pHelpDlg = new CHelpDlg;

	pHelpDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pHelpDlg->CenterWindow();
	pHelpDlg->ShowModal();

	delete pHelpDlg;
}

// 硬件设备变动，可能有串口变动
void  CDuiFrameWnd::CheckDevice() {
	char szComPort[16];
	int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

	// 单个串口
	if (!g_data.m_bMultipleComport) {
		if (nFindCount > 1) {
			m_lblBarTips->SetText("存在多个USB-SERIAL CH340串口，请只连接一个发射器");
		}
		else if (0 == nFindCount) {
			m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}
	else {
		if (0 == nFindCount) { 
			m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}

	CBusiness::GetInstance()->CheckLaunchAsyn();
}

// 接收器状态
void   CDuiFrameWnd::OnLaunchStatus(WPARAM wParam, LPARAM  lParam) {
	CLmnSerialPort::PortStatus e = (CLmnSerialPort::PortStatus)wParam;
	if (e == CLmnSerialPort::OPEN) {
		m_lblLaunchStatus->SetText("发射器连接成功");
	}
	else {
		m_lblLaunchStatus->SetText("发射器连接断开");
	}
}

// 术中读卡器正在读数的提示
void    CDuiFrameWnd::OnTrySurReader(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	BOOL   bStart = (BOOL)lParam;

	CDuiString  strText;
	if (bStart) {
		strText.Format("%lu%c读卡器正在测温...", i + 1, (char)(j + 'A'));
		m_lblSurReaderTry->SetText(strText);
	}
	else {
		m_lblSurReaderTry->SetText("");
	}
}

// 术中读卡器状态
void  CDuiFrameWnd::OnSurReaderStatus(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	BOOL   bConnected = (BOOL)lParam;

	m_pGrids[i]->SetSurReaderStatus(j, bConnected);
}

// 术中读卡器温度
void   CDuiFrameWnd::OnSurReaderTemp(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	TempItem * pItem = (TempItem *)lParam;
	m_pGrids[i]->OnSurReaderTemp(j, *pItem);
	delete pItem;
}

// 设置温度字体大小
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

//  1分钟定时器( 1. 定时更新逝去时间; 2. 删除一周前的温度数据 )
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
	//	// 如果该tag已经删除
	//	if ( it == m_tags_ui.end() ) {
	//		// 如果还有tag
	//		if ( m_layTags->GetCount() > 0 ) {
	//			CControlUI * pChildUI = m_layTags->GetItemAt(0);
	//			OnHandTagSelected(pChildUI);
	//		}
	//		// 如果没有tag
	//		else {
	//			m_cstHandImg->SetCurTag("");
	//		}
	//	}
	//}

	///*********** 定时保存excel *************/
	//time_t now = time(0);
	//struct tm  tmp;
	//localtime_s(&tmp, &now);

	//// CBusiness::GetInstance()->SaveExcelAsyn();   用于调试用

	//// 实际用法
	//if ((tmp.tm_hour == 8 || tmp.tm_hour == 12 || tmp.tm_hour == 18 || tmp.tm_hour == 0)
	//	&& (tmp.tm_min >= 0 && tmp.tm_min <= 1)) {		
	//	// 两次间隔时间最少30分钟
	//	if (now - m_LastSaveExcelTime >= 1800) {
	//		// 保存
	//		CBusiness::GetInstance()->SaveExcelAsyn();
	//		m_LastSaveExcelTime = now;
	//	}
	//}		
}

// 查询温度结果
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

// 右键弹出菜单
void   CDuiFrameWnd::OnImageMenu(DWORD  dwGridIndex, const POINT & pt, CControlUI * pParent ) {
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_image.xml"), pParent, dwGridIndex, 0 );
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE); 
}

// 导出Excel
void   CDuiFrameWnd::OnExportExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[dwIndex]->ExportExcel();
}

// 打印Excel图表
void   CDuiFrameWnd::OnPrintExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[dwIndex]->PrintExcel();
}
 
// 手持读卡器温度
void   CDuiFrameWnd::OnHandReaderTemp(WPARAM wParam, LPARAM  lParam) {
	TempItem * pItem            = (TempItem*)wParam;
	char *     tag_patient_name = (char *)lParam;
	assert(pItem);
	assert(tag_patient_name);

	BOOL  bNewTag = FALSE;
	m_cstHandImg->OnHandTemp(pItem, bNewTag);

	CTagUI * pTagUI = 0;

	assert(pItem->m_szTagId[0] != '\0');
	// 新Tag
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

		// 是否时间排序
		HandTagSortType eSortType = GetHandTagSortType();
		if ( eSortType == HandTagSortType_Time ) {
			CTagUI * pItem = (CTagUI *) m_layTags->GetItemAt(0);
			// 如果不是第一位
			if (pItem != pTagUI) {
				m_layTags->Remove(pTagUI, true);
				m_layTags->AddAt(pTagUI, 0);
			}			
		}
	}

	// 如果绑定了床位号
	int nBindingGridIndex = pTagUI->GetBindingGridIndex();
	if ( nBindingGridIndex > 0 ) {
		assert(nBindingGridIndex <= MAX_GRID_COUNT );
		m_pGrids[nBindingGridIndex - 1]->OnHandReaderTemp(*pItem);
	}

	// 已经保存在m_cstHandImg对象内
	// delete pItem;
	delete[] tag_patient_name;
}

// 数据库的相关数据查询完毕
void   CDuiFrameWnd::OnPrepared(WPARAM wParam, LPARAM  lParam) {
	CheckDevice();
}

// 获得当前的手持Tag排序
CDuiFrameWnd::HandTagSortType CDuiFrameWnd::GetHandTagSortType() {
	if (m_optDefaultSort->IsSelected())
		return HandTagSortType_Default;
	else
		return HandTagSortType_Time;
}

// 获得所有手持Tag温度数据
void   CDuiFrameWnd::OnAllHandTagTempData(WPARAM wParam, LPARAM  lParam) {
	std::vector<HandTagResult *> * pvRet = (std::vector<HandTagResult *> *)wParam;
	std::vector<HandTagResult *>::iterator it;

	assert( m_layTags->GetCount() == 0 );
	assert(m_tags_ui.size() == 0);

	for (it = pvRet->begin(); it != pvRet->end(); ++it) {
		HandTagResult * pItem = *it;
		// 没有数据
		if (0 == pItem->m_pVec || pItem->m_pVec->size() == 0)
			continue;

		// 保存数据
		m_cstHandImg->OnHandTempVec(pItem->m_pVec, pItem->m_szTagId);

		CTagUI * pTagUI = new CTagUI;
		m_layTags->Add(pTagUI);
		pTagUI->SetFixedHeight(TAG_UI_HEIGHT);

		TempItem * pSubItem = pItem->m_pVec->at(pItem->m_pVec->size() - 1);
		pTagUI->OnHandTemp(pSubItem, pItem->m_szTagPName);
		pTagUI->SetBindingGridIndex(pItem->m_nBindingGridIndex);

		// 如果绑定了床位号
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


	/**** 回收内存 ****/
	for ( it = pvRet->begin(); it != pvRet->end(); ++it) {
		HandTagResult * pItem = *it;
		if ( 0 == pItem->m_pVec )
			continue;
		// 已经保存在m_cstHandImg
		//ClearVector(*pItem->m_pVec);
		//delete pItem->m_pVec;
	}
	ClearVector(*pvRet);
	delete pvRet;
}

// 选中了手持Tag
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

// 点击了按时间排序
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

// 点击了默认排序
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

// 移动Tag UI，绑定窗格
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

	// TagUI 高亮经过的格子
	OnMoveTagUI(pt);
}

// 停止移动Tag UI，确定最终位置
void   CDuiFrameWnd::StopMoveTagUI() {
	if ( m_dragdrop_tag_dest_index >= 0 ) {
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)m_dragdrop_tag);
		if ( it != m_tags_ui.end() ) {
			CTagUI * pTag = it->second;
			assert( 0 == strcmp( pTag->m_item.m_szTagId, m_dragdrop_tag) );
			// 如果绑定的grid index改变
			if ( pTag->GetBindingGridIndex() != m_dragdrop_tag_dest_index + 1) {
				// 等到答复后，再设置界面
				CBusiness::GetInstance()->TagBindingGridAsyn(m_dragdrop_tag, m_dragdrop_tag_dest_index + 1);
			}			
		}
	}

	m_dragdrop_tag = "";
	m_dragdrop_tag_dest_index = -1;
	//m_hand_tabs->SelectItem(0);
	m_dragdropGrid->SetVisible(false);
}

// Tag UI移动过程中，经过的格子要高亮
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

			// 在总Grids范围内
			if ( nIndex < cnt ) {
				CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(nIndex);
				// 在矩形范围内
				if ((pt.x > rect.left + s.cx * i) && (pt.x < rect.left + s.cx * (i + 1))
					&& (pt.y > rect.top + s.cy * j) && (pt.y < rect.top + s.cy * (j + 1))) {
					pChild->SetBorderColor(HILIGHT_BINDING_GRID_BORDERCOLOR);
					pChild->SetTextColor(HILIGHT_BINDING_GRID_TEXTCOLOR);
					pChild->SetBkColor(HILIGHT_BINDING_GRID_BKCOLOR);
					m_dragdrop_tag_dest_index = nIndex;
				}
				else {
					// 如果不是当前绑定块
					if (nIndex != m_dragdrop_tag_cur_index - 1) {
						pChild->SetBorderColor(UNBINDING_GRID_BORDERCOLOR);
						pChild->SetTextColor(UNBINDING_GRID_TEXTCOLOR);
						pChild->SetBkColor(UNBINDING_GRID_BKCOLOR);
					}
					// 是当前绑定块
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

// 重新设置LayoutGrids视图
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

// 获取DragdropGrids视图的行，列
void  CDuiFrameWnd::GetDragDropGridsParams(int & nCol, int & nRow) {
	for ( int i = 0; i < 8; i++ ) {
		if ( (i + 1) * (i + 1) >= (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
			nCol = nRow = i + 1;
			return;
		}
	}
	nCol = nRow = 8;
}

// 根据Rect大小获取Label字体大小
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

// tag 绑定 grid index结果
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

			// 如果当前选中的tag是修改绑定的tag
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

	// 旧绑定窗格的图像和数据要清空
	if (pParam->m_nOldGridIndex > 0 ) {
		m_pGrids[pParam->m_nOldGridIndex - 1]->OnReleaseTagBinding();
	}

	delete pParam;
}

// 查询温度结果(手持Tag温度数据)
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

// 右键弹出菜单
void   CDuiFrameWnd::OnHandImageMenu(const POINT & pt, CControlUI * pParent) {
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_hand_image.xml"), pParent);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

// 导出Excel
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

// 打印Excel图表
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

// 温度打印
void   CDuiFrameWnd::OnBtnPrint(DWORD dwIndex) {
	CDuiString strTagId = m_pGrids[dwIndex]->GetCurTagId();
	if ( strTagId.GetLength() == 0 ) {
		::MessageBox(GetHWND(), "没有选中体温贴(tag)", "错误", 0);
		return;
	}

	CDuiString strPatientName = m_pGrids[dwIndex]->GetCurPatientName();

	CPatientDataDlg * pDlg = new CPatientDataDlg;
	CBusiness::GetInstance()->m_sigPatientInfo.connect(pDlg, &CPatientDataDlg::OnPatientInfo);
	CBusiness::GetInstance()->m_sigPatientData.connect(pDlg, &CPatientDataDlg::OnPatientData);

	STRNCPY(pDlg->m_szTagId,   strTagId,       MAX_TAG_ID_LENGTH);
	STRNCPY(pDlg->m_szUIPName, strPatientName, MAX_TAG_PNAME_LENGTH);

	pDlg->Create(this->m_hWnd, _T("打印体温单"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果名字改变
	if ( strPatientName != pDlg->m_szUIPName ) {
		m_pGrids[dwIndex]->SetCurPatientName(pDlg->m_szUIPName);
	}

	// 如果出院了,不显示	
	if ( pDlg->m_out_hospital_time > 0 ) {

		// 如果是手持模式
		if (m_pGrids[dwIndex]->GetMode() == CModeButton::Mode_Hand) {
			OnHandTagErasedNotify(strTagId);
			m_cstHandImg->DelTag(strTagId);
			m_cstHandImg->SetCurTag("");

			// 如果删除了选中的tag
			if (m_cur_selected_tag == strTagId) {
				// 如果还有tag
				if (m_layTags->GetCount() > 0) {
					CControlUI * pChildUI = m_layTags->GetItemAt(0);
					OnHandTagSelected(pChildUI);
				}
			}
		}			
		// 如果非手持模式(单点)
		else if (m_pGrids[dwIndex]->GetMode() == CModeButton::Mode_Single) {
			m_pGrids[dwIndex]->OnOutHospital();
		}
	}

	// 如果不是click ok
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

	pDlg->Create(this->m_hWnd, _T("打印体温单"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果名字改变
	if (strPatientName != pDlg->m_szUIPName) {
		pTagUI->m_cstPatientName->SetText(pDlg->m_szUIPName);
	}

	// 如果出院了,不显示	
	if (pDlg->m_out_hospital_time > 0) {

		// 如果是手持模式
		OnHandTagErasedNotify(strTagId);
		m_cstHandImg->DelTag(strTagId);
		m_cstHandImg->SetCurTag("");

		// 如果删除了选中的tag
		if (m_cur_selected_tag == strTagId) {
			// 如果还有tag
			if (m_layTags->GetCount() > 0) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
		}
	}

	// 如果不是click ok
	if (0 != ret) {
		delete pDlg;
		return;
	}

	delete pDlg;
}

// 住院温度打印
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

	pDlg->Create(this->m_hWnd, _T("打印体温单"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果名字改变
	if (strPatientName != pDlg->m_szUIPName) {
		BOOL  bFind = FALSE;
		// 从手持部分找
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strTagId);
		// 如果找到了
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

		// 从窗格部分找
		if (!bFind) {
			for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
				if ( m_pGrids[i]->GetCurTagId() == strTagId ) {
					m_pGrids[i]->SetCurPatientName(pDlg->m_szUIPName);
					break;
				}
			}
		}
	}

	// 如果出院了,不显示	
	if (pDlg->m_out_hospital_time > 0) {

		// 从手持模式找
		OnHandTagErasedNotify(strTagId);
		m_cstHandImg->DelTag(strTagId);
		m_cstHandImg->SetCurTag("");

		// 如果删除了选中的tag
		if (m_cur_selected_tag == strTagId) {
			// 如果还有tag
			if (m_layTags->GetCount() > 0) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
		}

		// 从窗格模式找
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

	pDlg->Create(this->m_hWnd, _T("打印体温单"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果名字改变
	if (strPatientName != pDlg->m_szUIPName) {
		BOOL  bFind = FALSE;
		// 从手持部分找
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strTagId);
		// 如果找到了
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

		// 从窗格部分找
		if (!bFind) {
			for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
				if (m_pGrids[i]->GetCurTagId() == strTagId) {
					m_pGrids[i]->SetCurPatientName(pDlg->m_szUIPName);
					break;
				}
			}
		}
	}

	// 如果出院了,不显示	
	if (pDlg->m_out_hospital_time > 0) {

		// 从手持模式找
		OnHandTagErasedNotify(strTagId);
		m_cstHandImg->DelTag(strTagId);
		m_cstHandImg->SetCurTag("");

		// 如果删除了选中的tag
		if (m_cur_selected_tag == strTagId) {
			// 如果还有tag
			if (m_layTags->GetCount() > 0) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
		}

		// 从窗格模式找
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

// 查询到的tag的绑定grid
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

			// 如果当前选中的tag是修改绑定的tag
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

// 右键弹出菜单
void   CDuiFrameWnd::OnTagMenu(TNotifyUI& msg) {
	CTagUI * pTagUI = (CTagUI *)msg.pSender->GetParent();
	CDuiString s = pTagUI->GetTagId();

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_tag.xml"), pTagUI);
	pMenu->Init(*this, msg.ptMouse);
	pMenu->ShowWindow(TRUE);      
}

// 删除Tag
void   CDuiFrameWnd::OnDeleteTag(TNotifyUI& msg) {
	if ( IDYES == ::MessageBox(GetHWND(), "确定要删除该Tag全部数据吗？", "删除Tag", MB_YESNO | MB_DEFBUTTON2 ) ) {
		CTagUI * pTagUI = (CTagUI *)msg.pSender;
		CDuiString  strTagId = pTagUI->GetTagId();
		CBusiness::GetInstance()->DelTagAsyn(strTagId);
	}
}

// 删除Tag数据结果
void  CDuiFrameWnd::OnDelTagRet(WPARAM wParam, LPARAM lParam) {
	char * szTagId = (char *)wParam;

	OnHandTagErasedNotify(szTagId);
	m_cstHandImg->DelTag(szTagId);
	m_cstHandImg->SetCurTag("");

	// 如果还有tag
	if (m_layTags->GetCount() > 0) {
		CControlUI * pChildUI = m_layTags->GetItemAt(0);
		OnHandTagSelected(pChildUI);
	}

	assert(szTagId);
	delete[] szTagId;
}

// 查询住院信息
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

// 查询住院信息时忙
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

// 查询住院信息结果
void  CDuiFrameWnd::OnQueryInHospitalRet(WPARAM wParam, LPARAM lParam) {
	std::vector<InHospitalItem *> * pRet = (std::vector<InHospitalItem*> *)wParam;
	ClearVector(m_vQRet);
	std::copy(pRet->begin(), pRet->end(), std::back_inserter(m_vQRet));

	SetQueryBusy(FALSE);

	CDuiString  strText;
	strText.Format("(%lu条记录)", m_vQRet.size());
	m_lblQueryRet->SetText(strText);

	m_nQCurPageIndex = 0;
	ShowInHospitalRetPage(m_nQCurPageIndex);

	delete pRet;
}

// 显示住院结果第几页
void  CDuiFrameWnd::ShowInHospitalRetPage(int nPageIndex) {
	m_lstQueryRet->RemoveAll();

	int nCnt = m_vQRet.size();
	// 没有记录
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

// 格式化手术事件信息
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

// 住院信息下一页
void   CDuiFrameWnd::NextInHospitalPage() {
	m_nQCurPageIndex++;
	ShowInHospitalRetPage(m_nQCurPageIndex);
}

// 住院信息上一页
void  CDuiFrameWnd::PrevInHospitalPage() {
	m_nQCurPageIndex--;
	ShowInHospitalRetPage(m_nQCurPageIndex);
}

// 住院列表右键弹出菜单
void  CDuiFrameWnd::OnInHospitalMenu(TNotifyUI& msg) {
	int nSel = m_lstQueryRet->GetCurSel();
	if (nSel < 0)
		return;

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_inhos.xml"), m_lstQueryRet);
	pMenu->Init(*this, msg.ptMouse);
	pMenu->ShowWindow(TRUE);
}

// 查询出院信息
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

// 查询出院信息时忙
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

// 查询出院信息结果
void   CDuiFrameWnd::OnQueryOutHospitalRet(WPARAM wParam, LPARAM lParam) {
	std::vector<OutHospitalItem *> * pRet = (std::vector<OutHospitalItem*> *)wParam;
	ClearVector(m_vQRet1);
	std::copy(pRet->begin(), pRet->end(), std::back_inserter(m_vQRet1));

	SetQuery1Busy(FALSE);

	CDuiString  strText;
	strText.Format("(%lu条记录)", m_vQRet1.size());
	m_lblQueryRet1->SetText(strText);

	m_nQCurPageIndex1 = 0;
	ShowOutHospitalRetPage(m_nQCurPageIndex1);

	delete pRet;
}

// 显示出院结果第几页
void  CDuiFrameWnd::ShowOutHospitalRetPage(int nPageIndex) {
	m_lstQueryRet1->RemoveAll();

	int nCnt = m_vQRet1.size();
	// 没有记录
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

// 出院信息下一页
void   CDuiFrameWnd::NextOutHospitalPage() {
	m_nQCurPageIndex1++;
	ShowOutHospitalRetPage(m_nQCurPageIndex1);
}

// 出院信息上一页
void   CDuiFrameWnd::PrevOutHospitalPage() {
	m_nQCurPageIndex1--;
	ShowOutHospitalRetPage(m_nQCurPageIndex1);
}

// 出院列表右键弹出菜单
void   CDuiFrameWnd::OnOutHospitalMenu(TNotifyUI& msg) {
	int nSel = m_lstQueryRet1->GetCurSel();
	if (nSel < 0)
		return;

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_outhos.xml"), m_lstQueryRet1);
	pMenu->Init(*this, msg.ptMouse);
	pMenu->ShowWindow(TRUE);
}



// 接收器连接状态通知
void   CDuiFrameWnd::OnLauchStatusNotify(CLmnSerialPort::PortStatus e) {
	::PostMessage( GetHWND(), UM_LAUNCH_STATUS, (WPARAM)e, 0);
}      

// 接收器尝试读取术中读卡器的温度数据
void   CDuiFrameWnd::OnTrySurReaderNotify(WORD wBed, BOOL bStart) {
	::PostMessage(GetHWND(), UM_TRY_SUR_READER, (WPARAM)wBed, bStart);
}

// 接收到术中读卡器连接状态的通知
void   CDuiFrameWnd::OnSurReaderStatusNotify(WORD wBed, BOOL bConnected) {
	::PostMessage(GetHWND(), UM_SUR_READER_STATUS, (WPARAM)wBed, bConnected);
}

// 术中读卡器的温度数据
void   CDuiFrameWnd::OnSurReaderTempNotify(WORD wBed, const TempItem & item) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));
	::PostMessage(GetHWND(), UM_SUR_READER_TEMP, (WPARAM)wBed, (LPARAM)pItem );
}

// 查询到温度结果(根据TagId)
void   CDuiFrameWnd::OnQueryTempRetNotify(const char * szTagId, WORD wBed, std::vector<TempItem*> * pvRet) {
	void ** pParam = new void *[3];

	char * pTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(pTagId, szTagId, MAX_TAG_ID_LENGTH);

	pParam[0] = (void *)pTagId;
	pParam[1] = (void *)wBed;
	pParam[2] = (void *)pvRet;

	::PostMessage(GetHWND(), UM_QUERY_TEMP_BY_TAG_ID_RET, (WPARAM)pParam, 0);
}

// 手持读卡器的温度数据
void   CDuiFrameWnd::OnHandReaderTempNotify(const TempItem & item, const char * tag_patient_name) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));

	assert(tag_patient_name);
	char * new_tag_patient_name = new char[MAX_TAG_PNAME_LENGTH];
	STRNCPY(new_tag_patient_name, tag_patient_name, MAX_TAG_PNAME_LENGTH);

	::PostMessage(GetHWND(), UM_HAND_READER_TEMP, (WPARAM)pItem, (LPARAM)new_tag_patient_name);
}

// 数据库需要的数据已经查询完毕
void  CDuiFrameWnd::OnPreparedNotify() {
	::PostMessage(GetHWND(), UM_PREPARED, 0, 0);
}

// 查询到的所有手持Tag温度数据
void   CDuiFrameWnd::OnAllHandTagTempDataNotify(std::vector<HandTagResult *> * pvRet) {
	::PostMessage(GetHWND(), UM_ALL_HAND_TAG_TEMP_DATA, (WPARAM)pvRet, 0);
}

// 删除掉过时的手持Tag
void   CDuiFrameWnd::OnHandTagErasedNotify(const char * szTagId) {
	assert(szTagId);

	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find(szTagId);
	// 如果找到了
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

// 绑定结果的通知
void   CDuiFrameWnd::OnBindingRetNotify(const TagBindingGridRet & item) {
	TagBindingGridRet * pParam = new TagBindingGridRet;
	memcpy( pParam, &item, sizeof(TagBindingGridRet) );
	::PostMessage( GetHWND(), UM_BINDING_TAG_GRID_RET, (WPARAM)pParam, 0 );
}

// 查询手持Tag结果通知
void   CDuiFrameWnd::OnQueryHandTagRetNotify(const char * szTagId, int nGridIndex, std::vector<TempItem*> * pvRet) {
	void ** pParam = new void *[3];

	char * pTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(pTagId, szTagId, MAX_TAG_ID_LENGTH);

	pParam[0] = (void *)pTagId;
	pParam[1] = (void *)nGridIndex;
	pParam[2] = (void *)pvRet;

	::PostMessage(GetHWND(), UM_QUERY_HAND_TEMP_BY_TAG_ID_RET, (WPARAM)pParam, 0);
}

// 查询tag的绑定grid id
void  CDuiFrameWnd::OnQueryBindingByTagRetNotify(const TagBindingGridRet & ret) {
	TagBindingGridRet * pParam = new TagBindingGridRet;
	memcpy(pParam, &ret, sizeof(TagBindingGridRet));
	::PostMessage(GetHWND(), UM_QUERY_BINDING_BY_TAG_RET, (WPARAM)pParam, 0);
}

// 删除Tag
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

// 查询出院信息的结果通知
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
		m_lblLaunchStatus->SetText("没有打开任何接收基站");
	}
	else {
		std::vector<int>::iterator it;
		CDuiString strText = "打开了";
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
		MessageBox(GetHWND(), "请输入床号!", "错误", 0);
		return;
	}

	if (strName.GetLength() == 0) {
		MessageBox(GetHWND(), "请输入姓名!", "错误", 0);
		return;
	}

	std::vector<CubeItem * >::iterator it = std::find_if(m_cube_items.begin(), 
		m_cube_items.end(), FindCubeItemObj(nBedNo));
	// 找到了同床号的
	if ( it != m_cube_items.end() ) {
		if (IDNO == MessageBox( GetHWND(), "已经存在同样的床位号。要覆盖原床位号的所有数据吗？",
			                    "创建新床位", MB_YESNO | MB_DEFBUTTON2)) {
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

	// 如果是添加
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
	// 数量一定相同
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
	// 数量一定相同
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
	// 如果是新Tag
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

		// 如果是高温
		if ( (int)pItem->m_dwTemp >= g_data.m_nCubeHighTemp ) {
			it = std::find_if( m_cube_items_high.begin(), m_cube_items_high.end(), 
				               FindCubeItemObj(pFindItem->nBedNo));
			// 如果高温区没有
			if ( it == m_cube_items_high.end() ) {
				m_cube_items_high.push_back(pFindItem);
			}
		}
		// 如果是低温
		else {
			it = std::find_if(m_cube_items_high.begin(), m_cube_items_high.end(),
				FindCubeItemObj(pFindItem->nBedNo));
			// 如果高温区有
			if (it != m_cube_items_high.end()) {
				m_cube_items_high.erase(it);
			}
		}
		UpdateCubeItemsHigh();

		// 如果当前是最大化状态
		if ( m_nMaxCubeBedNo > 0 && m_nMaxCubeBedNo == pFindItem->nBedNo ) {
			m_CubeImg->AddCubeTemp(pItem);
		}

		// 保存到数据库
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
	//pDlg->Create(this->m_hWnd, _T("修改病号"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	//pDlg->CenterWindow();
	//int ret = pDlg->ShowModal();

	//// 如果不是click ok
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

	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handle为声明的HANDLE类型的全局变量 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		::MessageBox(0, "程序已经打开或没有关闭完全，请先关闭或等待完全关闭!", "错误", 0);
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


	// 设置icon
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
            

