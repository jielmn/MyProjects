#pragma once

#include "common.h"
#include "ModeButtonUI.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI();
	virtual ~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;
	virtual void MyInvalidate();
	// Ϊlabel����
	void   PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect);
	virtual void   SetRemark(DuiLib::CDuiString & strRemark);	

protected:
	virtual void   DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void   DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	// ���㱾�ؼ���ռλ�ط��Ŀ��
	int    GetMyWidth();
	int    GetMyScrollX();
	virtual DWORD  GetGridIndex();
	virtual DWORD  GetReaderIndex();
	const  std::vector<TempItem * > & GetTempData(DWORD j);
	virtual CModeButton::Mode   GetMode();
	CControlUI * GetGrid();
	int    GetCelsiusHeight(int height, int nCelsiusCount,int nVMargin = MIN_MYIMAGE_VMARGIN);
	// ��ˮƽ�̶���
	void   DrawScaleLine( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		                  const RECT & rectScale, const RECT & rect );
	// ���߿�
	void   DrawBorder(HDC hDC, const RECT & rectScale, int width);
	// ���̶�ֵ
	void   DrawScale( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
		              const RECT & rectScale, int width, BOOL bDrawRectangle = TRUE, DWORD dwTextColor = RGB(255, 255, 255) );
	// ��������
	void   DrawWarning( HDC hDC, DWORD i, DWORD j, int nMaxTemp, int nHeightPerCelsius, int nMaxY,
		                const RECT & rectScale, int width );
	// 7����ͼ�м�������
	int     GetDayCounts(DWORD i, DWORD j, CModeButton::Mode mode);
	time_t  GetFirstTime(DWORD i, DWORD j, CModeButton::Mode mode);
	// �����ӷָ���
	void    DrawDaySplit( HDC hDC, int nDayCounts, const RECT & rectScale, int nDayWidth, int nMaxY,
		                  int nCelsiusCnt, int nHeightPerCelsius, time_t  tFirstDayZeroTime);
	// ������ͼ ��Χ[tFirstTime, tLastTime]
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime,      float fSecondsPerPixel,
		                  int    nMaxTemp,   int nHeightPerCelsius, POINT  tTopLeft,    Graphics & graphics,
		                  BOOL  bDrawPoints, DWORD i, DWORD j, CModeButton::Mode mode );
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		                  int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
		                  BOOL  bDrawPoints, const  std::vector<TempItem * > & vTempData, 
		                  Pen * pen, SolidBrush * brush);
	void    DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius);
	// ��ʱ���ı�
	void    DrawTimeText(HDC hDC, time_t  tFirstTime, time_t tLastTime, float fSecondsPerPixel, POINT  top_left, const RECT & rValid);
	// ˫���¼�
	virtual void    OnDbClick();
	// �¶����ݸ���
	virtual DWORD   GetTempCount(DWORD i, DWORD j, CModeButton::Mode mode);
	// ����˫���˵ڼ���
	virtual int     GetClickDayIndex(DWORD i, DWORD j, CModeButton::Mode mode);
	// ���single day����ʼʱ��ͽ���ʱ��
	virtual BOOL    GetSingleDayTimeRange(time_t & start, time_t & end, DWORD i, DWORD j, CModeButton::Mode mode);
	BOOL    GetTimeRange(const std::vector<TempItem * > & v, time_t & start, time_t & end);
	// ���ֻ���
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);
	// �õ�������С��ʾ�¶�
	virtual void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, DWORD i, DWORD j, CModeButton::Mode mode);
	virtual void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, BOOL & bFirst, const std::vector<TempItem * > & v );
	// ��ʮ����
	void   DrawCrossLine( HDC hDC, const RECT & rValid, const POINT & cursor_point,
		                  time_t tFirstTime, float fSecondsPerPixel, int nMaxTemp, int nHeightPerCelsius,
		                  POINT  top_left );
	// �ϷŲ�����ʼ
	void  BeginDragDrop();
	// �������Ƿ���Ҫ�ı�ָ��
	virtual void  CheckCursor(const POINT & pt);
	// �����ϷŲ���
	void  DragDropIng(const POINT & pt);
	// �����ϷŲ���
	void  EndDragDrop(const POINT & pt);
	// ����Ƿ�����ע��
	virtual void  CheckRemark(const POINT & pt);
	virtual BOOL  CheckRemark( const POINT & pt, const std::vector<TempItem * > & v, 
		               time_t tFirstTime, float fSecondsPerPixel,
		               int    nMaxTemp,   int nHeightPerCelsius, POINT  top_left );
	// ����ע��
	BOOL  SetRemark( const std::vector<TempItem * > & v, DWORD  dwDbId, DuiLib::CDuiString & strRemark );
	// ��ע��
	void  DrawRemark( HDC hDC, Graphics & g, time_t tFirstTime, float fSecondsPerPixel,
		              int nMaxTemp, int nHeightPerCelsius, POINT  top_left,
		              DWORD i, DWORD j, CModeButton::Mode mode, const RECT & rValid );
	void  DrawRemark( HDC hDC, Graphics & g, time_t tFirstTime, float fSecondsPerPixel,
		              int nMaxTemp, int nHeightPerCelsius, POINT  top_left,
		              const std::vector<TempItem * > & v, const RECT & rValid);

protected:
	enum   E_STATE {
		STATE_7_DAYS = 0,
		STATE_SINGLE_DAY
	};

	E_STATE                      m_state;                 // 7days or single day
	int                          m_nSingleDayIndex;       // ������ͼ���ڼ���
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
	HBRUSH                       m_hCommonBrush;
	HPEN                         m_hLowTempAlarmPen;
	HPEN                         m_hHighTempAlarmPen;
	HPEN                         m_hDaySplitThreadPen;
	Pen *                        m_temperature_pen[MAX_READERS_PER_GRID];
	SolidBrush *                 m_temperature_brush[MAX_READERS_PER_GRID];
	float                        m_fSecondsPerPixel;
	BOOL                         m_bSetSecondsPerPixel;

	/***** �ϷŲ��� ****/
	enum DragDropObj {
		DragDrop_None = 0,
		DragDrop_LowAlarm,
		DragDrop_HighAlarm
	};

	BOOL                  m_bDragDrop;                 // �Ƿ�ʼ�ϷŲ���
	DragDropObj           m_DragDropObj;               // �ϷŲ�����ʼ���ϷŵĶ���
	DragDropObj           m_CursorObj;                 // �ϷŲ�����ʼǰ����껬��ʱ�����Ķ���
	DWORD                 m_dwRemarkingIndex;          // ���ڱ༭�ĵ�

	Pen *                 m_remark_pen;
	SolidBrush *          m_remark_brush;
};

class CImageLabelUI : public DuiLib::CLabelUI
{
public:
	CImageLabelUI();
	~CImageLabelUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;

	// ����͸��ɫ
	void SetOpacityTextColor(DWORD  dwColor) {
		SetTextColor( (dwColor & 0xFFFFFF) | (GRID_LABLE_TRANSPARENT_PARAM << 24) );
	}

	void  SetMyImage(CMyImageUI * img) {
		m_image = img;
	}

private:
	CMyImageUI  *   m_image;
};

class CMyHandImage : public CMyImageUI {
public:
	CMyHandImage();
	~CMyHandImage();
	void  OnHandTemp( TempItem * pTemp, BOOL & bNewTag );
	void  OnHandTempVec(vector<TempItem *> * pVec, const char * szTagId );
	// ɾ����ʱ������
	void  PruneData();
	void  SetCurTag(const char * szTagId);
	CDuiString   GetCurTagId();
	void   SetRemark(DuiLib::CDuiString & strRemark);

	sigslot::signal1<const char *>              m_sigTagErased;

	// ����excel
	void  ExportExcel(const char * szPatientName);

	// ��ӡexcel���
	void  PrintExcel(const char * szPatientName);

	// ɾ��Tag
	void  DelTag(const char * szTagId);

private:
	virtual void   DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void   DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	// �������Ƿ���Ҫ�ı�ָ��
	virtual void  CheckCursor(const POINT & pt);
	// ˫���¼�
	virtual void    OnDbClick();

	/***** ������ *****/
	virtual DWORD  GetGridIndex();
	virtual DWORD  GetReaderIndex();
	virtual CModeButton::Mode   GetMode();	
	/* end ������  */

	void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp);
	int    GetDayCounts();
	int    GetClickDayIndex();
	void   MyInvalidate();
	DWORD  GetTempCount();
	BOOL   GetSingleDayTimeRange(time_t & start, time_t & end);
	void   CheckRemark(const POINT & pt);
	BOOL   CheckRemark(const POINT & pt, const std::vector<TempItem * > & v,
		time_t tFirstTime, float fSecondsPerPixel,
		int    nMaxTemp, int nHeightPerCelsius, POINT  top_left);

	void Clear();
	void PruneData(std::vector<TempItem*> & v, time_t t);

private:
	// ���е��ֳֶ����������ݣ�����tag id���з���
	//           tag id,   ��Ӧ�����ݼ���
	std::map<std::string, vector<TempItem *> *>        m_data;
	// ��ǰ��ʾ�����ĸ�����
	std::string                                        m_cur_tag;
};