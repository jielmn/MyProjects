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
};
