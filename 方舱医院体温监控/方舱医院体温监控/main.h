#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

#include "GridUI.h"
#include "DragDropUI.h"
#include "Launch.h"
#include "TagUI.h"
#include "WaitingBarUI.h"

class CDuiFrameWnd : public WindowImplBase , public sigslot::has_slots<>
{
public:
	enum ENUM_GRID_STATUS {
		GRID_STATUS_GRIDS = 0,
		GRID_STATUS_MAXIUM
	};

	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
#endif

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void     OnFinalMessage(HWND hWnd);
	virtual LRESULT  OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT  OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

private:
	enum  HandTagSortType {
		HandTagSortType_Default = 0,
		HandTagSortType_Time
	};

private:
	void   RemoveAllGrids();
	// ����gridsһҳ(��������"��һҳ", "��һҳ"����ʼ���ȵ�)
	void   RefreshGridsPage();
	// ����grids size
	void   RefreshGridsSize(int width, int height);
	void   RefreshGridsSize();
	// ��һҳ
	void   NextPage();
	// ��һҳ
	void   PrevPage();
	// ˫���¼�
	void   OnDbClick(BOOL & bHandled);
	// �ƶ����ӣ�����˳��
	void   MoveGrid(const POINT & pt);
	// ֹͣ�ƶ����ӣ�ȷ������λ��
	void   StopMoveGrid();
	// �����ƶ������У������ĸ���Ҫ����
	void   OnMoveGrid(const POINT & pt);
	// �϶����ӹ����У��ϵ���������ң����·�ҳ
	void   OnFlipPage();
	// ����ˡ��˵�����ť
	void   OnBtnMenu(TNotifyUI& msg);
	// ����ˡ����á�
	void   OnSetting();
	// ����ˡ����ڡ�
	void   OnAbout();
	// ����ˡ�������
	void   OnHelp();
	// �����������
	void   SaveGrid(DWORD  dwIndex);
	// ɾ����������
	void   RemoveGridCfg(DWORD  dwIndex);
	// Ӳ���豸�䶯�������д��ڱ䶯
	void   CheckDevice();
	// ������״̬
	void   OnLaunchStatus(WPARAM wParam, LPARAM  lParam);
	// ���ж��������ڶ�������ʾ
	void   OnTrySurReader(WPARAM wParam, LPARAM  lParam);
	// ���ж�����״̬
	void   OnSurReaderStatus(WPARAM wParam, LPARAM  lParam);
	// ���ж������¶�
	void   OnSurReaderTemp(WPARAM wParam, LPARAM  lParam);
	// ��ѯ�¶Ƚ��(�����¶�����)
	void   OnQueryTempRet(WPARAM wParam, LPARAM  lParam);
	// �����¶������С
	void   OnGridSizeChanged(const SIZE & s);
	//  1���Ӷ�ʱ��( 1. ��ʱ������ȥʱ��; 2. ɾ��һ��ǰ���¶����� )
	void   On60SecondsTimer();	
	// �Ҽ������˵�
	void   OnImageMenu(DWORD  dwGridIndex, const POINT & pt, CControlUI * pParent);
	// ����Excel
	void   OnExportExcel(DWORD  dwIndex);
	// ��ӡExcelͼ��
	void   OnPrintExcel(DWORD  dwIndex);
	// �ֳֶ������¶�
	void   OnHandReaderTemp(WPARAM wParam, LPARAM  lParam);
	// ���ݿ��������ݲ�ѯ���
	void   OnPrepared(WPARAM wParam, LPARAM  lParam);
	// ��õ�ǰ���ֳ�Tag����
	HandTagSortType GetHandTagSortType();
	// ��������ֳ�Tag�¶�����
	void   OnAllHandTagTempData(WPARAM wParam, LPARAM  lParam);
	// ѡ�����ֳ�Tag
	void   OnHandTagSelected(CControlUI * pTagUI);
	// ����˰�ʱ������
	void   OnHandTagTimeOrder();
	// �����Ĭ������
	void   OnHandTagDefaultOrder();
	void   OnHandTagTimeOrder(CTagUI * pTagUI);
	// �ƶ�Tag UI���󶨴���
	void   MoveTagUI(const POINT & pt);
	// ֹͣ�ƶ�Tag UI��ȷ������λ��
	void   StopMoveTagUI();
	// Tag UI�ƶ������У������ĸ���Ҫ����
	void   OnMoveTagUI(const POINT & pt);
	// ��������LayoutGrids��ͼ
	void   ResetDragdropGrids(int w = 0, int h = 0);
	// ��ȡDragdropGrids��ͼ���У���
	void   GetDragDropGridsParams(int & nCol, int & nRow );
	// ����Size��С��ȡLabel�����С
	int    GetFontBySize(const SIZE & s);
	// tag �� grid index���
	void   OnTagBindingGridRet(WPARAM wParam, LPARAM  lParam);
	// ��ѯ�¶Ƚ��(�ֳ�Tag�¶�����)
	void   OnQueryHandTempRet(WPARAM wParam, LPARAM  lParam);
	// tag name changed
	void   OnTagNameChanged(WPARAM wParam, LPARAM  lParam);
	// �Ҽ������˵�
	void   OnHandImageMenu(const POINT & pt, CControlUI * pParent);
	// ����Excel
	void   OnHanxExportExcel();
	// ��ӡExcelͼ��
	void   OnHandPrintExcel();
	// �¶ȴ�ӡ
	void   OnBtnPrint(DWORD  dwIndex);
	void   OnBtnPrint1(TNotifyUI& msg);
	// סԺ�¶ȴ�ӡ
	void   OnBtnPrint2(TNotifyUI& msg);
	void   OnBtnPrint3(TNotifyUI& msg);
	// ��ѯ����tag�İ�grid
	void   OnQueryBindingByTag(WPARAM, LPARAM);
	// �Ҽ������˵�
	void   OnTagMenu(TNotifyUI& msg);
	// ɾ��Tag
	void   OnDeleteTag(TNotifyUI& msg);
	// ɾ��Tag���ݽ��
	void   OnDelTagRet(WPARAM, LPARAM);
	// ��ѯסԺ��Ϣ
	void   OnQueryInHospital();
	// ��ѯסԺ��Ϣʱæ
	void   SetQueryBusy(BOOL bBusy = TRUE);
	// ��ѯסԺ��Ϣ���
	void   OnQueryInHospitalRet(WPARAM, LPARAM);
	// ��ʾסԺ����ڼ�ҳ
	void   ShowInHospitalRetPage(int nPageIndex);
	// ��ʽ�������¼���Ϣ
	CDuiString  FormatEvents(PatientEvent * events, int nEventCnt);
	// סԺ��Ϣ��һҳ
	void   NextInHospitalPage();
	// סԺ��Ϣ��һҳ
	void   PrevInHospitalPage();
	// סԺ�б��Ҽ������˵�
	void   OnInHospitalMenu(TNotifyUI& msg);
	// ��ѯ��Ժ��Ϣ
	void   OnQueryOutHospital();
	// ��ѯ��Ժ��Ϣʱæ
	void   SetQuery1Busy(BOOL bBusy = TRUE);
	// ��ѯ��Ժ��Ϣ���
	void   OnQueryOutHospitalRet(WPARAM, LPARAM);
	// ��ʾ��Ժ����ڼ�ҳ
	void   ShowOutHospitalRetPage(int nPageIndex);
	// ��Ժ��Ϣ��һҳ
	void   NextOutHospitalPage();
	// ��Ժ��Ϣ��һҳ
	void   PrevOutHospitalPage();
	// ��Ժ�б��Ҽ������˵�
	void   OnOutHospitalMenu(TNotifyUI& msg);

public:
	// ����������״̬֪ͨ
	void   OnLauchStatusNotify(CLmnSerialPort::PortStatus e);
	// ���������Զ�ȡ���ж��������¶�����
	void   OnTrySurReaderNotify(WORD wBed, BOOL bStart);
	// ���յ����ж���������״̬��֪ͨ
	void   OnSurReaderStatusNotify(WORD wBed, BOOL bConnected);
	// ���ж��������¶�����
	void   OnSurReaderTempNotify(WORD wBed, const TempItem & item);
	// ��ѯ���¶Ƚ��(����TagId)
	void   OnQueryTempRetNotify(const char *, WORD, std::vector<TempItem*> *);
	// �ֳֶ��������¶�����
	void   OnHandReaderTempNotify(const TempItem & item, const char * tag_patient_name);
	// ���ݿ���Ҫ�������Ѿ���ѯ���
	void   OnPreparedNotify();
	// ��ѯ���������ֳ�Tag�¶�����
	void   OnAllHandTagTempDataNotify(std::vector<HandTagResult *> * pvRet);
	// ɾ������ʱ���ֳ�Tag
	void   OnHandTagErasedNotify(const char * szTagId);	
	// �󶨽����֪ͨ
	void   OnBindingRetNotify(const TagBindingGridRet & item);
	// ��ѯ�ֳ�Tag���֪ͨ
	void   OnQueryHandTagRetNotify(const char *, int, std::vector<TempItem*> *);
	// ��ѯtag�İ�grid id
	void   OnQueryBindingByTagRetNotify(const TagBindingGridRet &);
	// ɾ��Tag
	void   OnDelTagRetNotify(const char * szTagId);
	// ��ѯסԺ��Ϣ�Ľ��֪ͨ
	void OnQueryInHospitalNotify(const std::vector<InHospitalItem*>&);
	// ��ѯ��Ժ��Ϣ�Ľ��֪ͨ
	void OnQueryOutHospitalNotify(const std::vector<OutHospitalItem*>&);


private:	
	CDialogBuilderCallbackEx                    m_callback;
	DWORD                                       m_dwCurSelGridIndex;      // ��ǰѡ��ĸ������
	ENUM_GRID_STATUS                            m_eGridStatus;            // �����̬��������󻯵ĵ�������

	DuiLib::CTabLayoutUI *                      m_tabs;
	DuiLib::CTileLayoutUI *                     m_layMain;
	CGridUI *                                   m_pGrids[MAX_GRID_COUNT];
	CHorizontalLayoutUI *                       m_layPages;
	CButtonUI *                                 m_btnPrevPage;
	CButtonUI *                                 m_btnNextPage;

	CMyHandImage *                              m_cstHandImg;            // �ֳֶ�������ͼ
	CVerticalLayoutUI *                         m_layTags;               // �ֳֶ�������tag�б�

	/**** drag drop����  ****/
	int                                         m_nDgSourceIndex;         // drag drop ��ʼ�϶���grid index
	int                                         m_nDgDestIndex;           // drag drop �϶���Ŀ��grid index
	DWORD                                       m_dwDgStartTick;          // drag drop ��ʼ��ʱ��
	CControlUI *                                m_dragdropGrid;
	// ��Ӷ�ʱ������ɿ�ҳ��ĸ����϶�
	BOOL                                        m_bDragTimer;
	BOOL                                        m_bFlipPrevPage;

	// �˵���ť
	DuiLib::CButtonUI *                         m_btnMenu;

	/******* ״̬�� ******/
	CLabelUI *                                  m_lblBarTips;
	CLabelUI *                                  m_lblLaunchStatus;
	CLabelUI *                                  m_lblSurReaderTry;

	/*�ֳ�Reader������*/
	COptionUI *                                 m_optDefaultSort;
	COptionUI *                                 m_optTimeSort;
	DuiLib::CTabLayoutUI *                      m_hand_tabs;            // ��tag�Ĺ������õ�
	CTileLayoutUI *                             m_layDragDropGrids;     // ��tag�Ĺ������õ�

	std::map<std::string, CTagUI *>             m_tags_ui;
	CDuiString                                  m_dragdrop_tag;
	int                                         m_dragdrop_tag_dest_index;
	int                                         m_dragdrop_tag_cur_index;
	DWORD                                       m_dragdrop_tag_timetick;

	CDuiString                                  m_cur_selected_tag;     // ��ǰѡ�е��ֳֶ�����Tag

	// ��ʱ����excel
	time_t                                      m_LastSaveExcelTime;

// ��ѯסԺ��Ϣ
private:
	CEditUI *                                   m_edQName;
	CComboUI *                                  m_cmbQSex;
	CEditUI *                                   m_edQAge;
	CEditUI *                                   m_edQOutPatient;
	CEditUI *                                   m_edQHospitalAdmissionNo;
	CDateTimeUI *                               m_datInHospitalStart;
	CDateTimeUI *                               m_datInHospitalEnd;
	CButtonUI *                                 m_btnQuery;
	CWaitingBarUI *                             m_q_waiting_bar;
	CLabelUI *                                  m_lblQueryRet;
	CListUI *                                   m_lstQueryRet;
	CButtonUI *                                 m_btnQPrev;
	CButtonUI *                                 m_btnQNext;
	std::vector<InHospitalItem *>               m_vQRet;
	int                                         m_nQCurPageIndex;

	// ��ѯ��Ժ��Ϣ
private:
	CEditUI *                                   m_edQName1;
	CComboUI *                                  m_cmbQSex1;
	CEditUI *                                   m_edQAge1;
	CEditUI *                                   m_edQOutPatient1;
	CEditUI *                                   m_edQHospitalAdmissionNo1;
	CDateTimeUI *                               m_datInHospitalStart1;
	CDateTimeUI *                               m_datInHospitalEnd1;
	CDateTimeUI *                               m_datOutHospitalStart1;
	CDateTimeUI *                               m_datOutHospitalEnd1;
	CButtonUI *                                 m_btnQuery1;
	CWaitingBarUI *                             m_q_waiting_bar1;
	CLabelUI *                                  m_lblQueryRet1;
	CListUI *                                   m_lstQueryRet1;
	CButtonUI *                                 m_btnQPrev1;
	CButtonUI *                                 m_btnQNext1;
	std::vector<OutHospitalItem *>              m_vQRet1;
	int                                         m_nQCurPageIndex1;

private:
	COptionUI *                                 m_optInHospital;
	COptionUI *                                 m_optOutHospital;

	// ����
private:
	CVerticalLayoutUI *                         m_CubeItems;
	CVerticalLayoutUI *                         m_CubeItems_high_temp;
	CTabLayoutUI *                              m_SubSwitch;
	BOOL                                        m_bNewTagDragDrop;
	CCubeItemUI *                               m_HightLightItem;
	CHorizontalLayoutUI *                       m_layRight;
	CHorizontalLayoutUI *                       m_layMain1;
	CEditUI *                                   m_edtBedNo1;
	CEditUI *                                   m_edtName1;
	CEditUI *                                   m_edtPhone1;
	CNewTagUI *                                 m_CurTag;
	CButtonUI *                                 m_btnCubeBed; 
	CLabelUI  *                                 m_lblCubePatientName;
	CLabelUI  *                                 m_lblCubePhone;
	CMyImageUI *                                m_CubeImg;


	std::vector<CubeItem * >                    m_cube_items;
	CurTagData                                  m_cur_tag;
	std::vector<CubeItem * >                    m_cube_items_high;
	int                                         m_nMaxCubeBedNo;    // ���ʱ�Ĵ�λ��
	CCubeItemUI *                               m_SelectedCubeItem;
	CCubeItemUI *                               m_SelectedCubeItem_high_temp;	
	CCubeItemUI *                               m_MenuCuteItem;

private:
	void   MoveNewTag(const POINT & pt);
	void   OnMoveNewTag(const POINT & pt);
	void   StopMoveNewTag();
	bool   OnMain1Size(void * pParam);

	void   OnChecComPortsRet(WPARAM, LPARAM);
	void   OnAddCubeItem();
	void   OnGetAllCubeItems(WPARAM wParam, LPARAM  lParam);
	void   OnAddCubeItemRet(WPARAM wParam, LPARAM  lParam);

	void   UpdateCubeItems();
	void   UpdateCubeItemsHigh();
	void   OnCubeTempItem(WPARAM wParam, LPARAM  lParam);
	void   UpdateNewTag();
	void   OnNewTagTimer();
	void   OnCubeBindingTag(WPARAM wParam, LPARAM  lParam);
	void   OnQueryCubeTempRet(WPARAM wParam, LPARAM  lParam);

	void   OnCubeItemMenu(TNotifyUI& msg);
	void   OnCubeItemClick(TNotifyUI& msg);
	void   UpdateSelectedCubeItem();
	void   UpdateSelectedCubeItem_high_temp();	

	void   OnModifyCuteItem();
	void   OnDismissBinding();
	void   OnDeleteCuteItem();

	void   OnUpdateCubeItemRet(WPARAM wParam, LPARAM  lParam);
	void   OnDismissBindingRet(WPARAM wParam, LPARAM  lParam);
	void   OnDeleteCuteItemRet(WPARAM wParam, LPARAM  lParam);
};




class  FindCubeItemObj {
public:
	FindCubeItemObj(int nBedNO) {
		m_nBedNo = nBedNO;
	}
	bool operator() (CubeItem * pItem) {
		if (pItem->nBedNo == m_nBedNo)
			return true;
		return false;
	}
private:
	int    m_nBedNo;
};

class  FindCubeItemByTag {
public:
	FindCubeItemByTag(const char * szTagId) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
	}
	bool operator() (CubeItem * pItem) {
		if ( 0 == strcmp(pItem->szTagId, m_szTagId) ) {
			return true;
		}
		return false;
	}
	char  m_szTagId[MAX_TAG_ID_LENGTH];
};

class  CopyCubeHighItem {
public:
	CopyCubeHighItem(int nHighTemp) {
		m_nHighTemp = nHighTemp;
	}
	bool operator() (CubeItem * pItem) {
		if (pItem->nTemp >= m_nHighTemp)
			return true;
		else
			return false;
	}
private:
	int     m_nHighTemp;
};


class CCubeItemDlg : public DuiLib::WindowImplBase
{
public:
	CCubeItemDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("CubeItemDlg"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("CubeItemDlg.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg); 
	virtual void   InitWindow();

public:
	CubeItem           m_data;

private:
	CLabelUI *         m_lblBedNo;
	CEditUI *          m_edtName;
	CEditUI *          m_edtPhone;

private:
	void    OnMyOk();
};