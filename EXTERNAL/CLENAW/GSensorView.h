#pragma once
#include "afxwin.h"

#include <BLACKBOX/TextData.h>

#define DEF_DISPLAY_TIME_INFO		0
#define DEF_DISPLAY_TRACKBAR		1

typedef struct tagGPOINT
{
    int x;		// x point
	int xy;		// gx y point
	int yy;		// gy y point
	int zy;		// gz y point
	int sy;		// speed y point
	int evt; 	// event type
} GPOINT, *PGPOINT, NEAR *NPGPOINT, FAR *LPGPOINT;

typedef const GPOINT FAR* LPCGPOINT;

class CGPoint;

class CGPoint: public tagGPOINT
{
// Constructors
public:
	// uninitialized 
	CGPoint() throw();
	// from left, top, right, and bottom
	CGPoint(int px, int pxy, int pyy, int pzy, int psy, int event = NULL) throw();
	// copy constructor
	CGPoint(const GPOINT& srcGPoint) throw();
	// from a pointer to another rect
	CGPoint(LPGPOINT lpSrcGPoint) throw();

	void SetGPoint(int px, int pxy, int pyy, int pzy, int psy, int event = NULL) throw();
#if 0
// Attributes (in addition to GPOINT members)

// convert between CRect and LPGPOINT/LPCGPOINT (no need for &)
	operator LPGPOINT() throw();
	operator LPCGPOINT() const throw();
// Operations

	// set rectangle from left, top, right, and bottom
	void SetGPoint(int px, int pxy, int pyy, int pzy) throw();
	// copy from another rectangle
	void CopyGPoint(LPGPOINT lpSrcGPoint) throw();
	// TRUE if exactly the same as another rectangle
	BOOL EqualGPoint(LPGPOINT lpGPoint) const throw();


	// absolute position of rectangle
	void MoveToX(int x) throw();
	void MoveToXY(int xy) throw();
	void MoveToYY( int yy) throw();
	void MoveToZY(int zy) throw();
#endif
	// Additional Operations
	void operator=(const GPOINT& srcGPoint) throw();
#if 0
	BOOL operator==(const GPOINT& gpoint) const throw();
	BOOL operator!=(const GPOINT& gpoint) const throw();
	void operator+=(LPGPOINT lpGPoint) throw();
	void operator-=(LPGPOINT lpGPoint) throw();
	void operator&=(const GPOINT& gpoint) throw();
	void operator|=(const GPOINT& gpoint) throw();

// Operators returning CGPoint values

	CGPoint operator+(LPGPOINT lpGPoint) const throw();
	CGPoint operator-(LPGPOINT lpGPoint) const throw();
	CGPoint operator&(const GPOINT& gpoint2) const throw();
	CGPoint operator|(const GPOINT& gpoint2) const throw();
//	CGPoint MulDiv(int nMultiplier, int nDivisor) const throw();
#endif
};

typedef struct tagGValue{
	double	GX;
	double	GY;
	double	GZ;
	double	speed; //knot
	DWORD	time;
}GVALUE, *PGVALUE;

#define MAX_GPOINT		10000 // 30 frame x 60 sec * 5 Min + 1000
#define MAX_EVENT_TYPE	10
class CGSensorView :
	public CWnd
{
// Construction
public:
	CGSensorView(void);

	enum LINE_TYPE
	{
		LINE_X		= 0	,
		LINE_Y			,
		LINE_Z			,
		LINE_S			,	//speed
		LINE_TRACK		,
		LINE_INVALID
	};

	enum SPEED_TYPE
	{
		SPEED_KM_P_H		= 0 	,
		SPEED_MILE_P_H		,
		SPEED_KNOT				,

		SPEED_INVALID	
	};
	
// Attributes
public:
		CGPoint m_dNewPoint, m_dLastPoint;
		PGVALUE m_oGValue;
		int m_nGValueSize;
		
		int m_nLastIndexPoint;
		int m_nLastIndex;
		int m_nLastRefreshPoint;
		int m_nMaxShowIndexCnt;
		BOOL m_bRedrawBlocked;
		
		CBrush m_tracbarBrush;
		CWnd	m_wndTrackbar;
		int m_nTrackbarPosition;
		int m_nTrackbarPointXFactor;
		int m_nTrackbarPointXCurrent;
		int m_nDrawIndex;

		COLORREF m_crTrackBarColor;
		COLORREF m_crBackColor;        // background color
		COLORREF m_crGridColor;        // grid color
		COLORREF m_crGridTextColor;        // grid color
		COLORREF m_crPlotColor[4];        // data color  
		COLORREF m_crEventColor[MAX_EVENT_TYPE];

		int m_nClientHeight;
       	int m_nClientWidth;
    		int m_nPlotHeight;
    		int m_nPlotWidth;

		double m_dLowerLimit, m_dXLower;        // lower bounds
		double m_dUpperLimit, m_dXUpper;        // upper bounds
		double m_dRange, m_XRange;
		double m_dVerticalFactor, m_dHorizontalFactor;
  		double m_dGridInterval;

		double m_dSpeedFactor; //knot ==> km or mile
		double m_dSpeedRange, m_dSpeedVerticalFactor;
			
		CRect  m_rectClient;
		CRect  m_rectPreClient;
		CRect  m_rectPlot;
		CRect  m_rectRangeG;
		CRect  m_rectRangeSpeed;
		CRect  m_rectRangeScale;
		
		CPen   m_penPlot[4];
		CBrush m_brushBack;
		CBrush m_brushEvent;

		CDC     m_dcGrid;
		CDC     m_dcPlot;
		CBitmap *m_pbitmapOldGrid;
		CBitmap *m_pbitmapOldPlot;
		CBitmap m_bitmapGrid;
		CBitmap m_bitmapPlot;

		int m_nYDecimals, m_nXDecimals, m_nGDecimals;
		double m_dScaleX;
		
		CString m_strXUnitsString;
		CString m_strYUnitsString;
		CFont 	m_fontAxisX, m_fontGValue;
		int 	m_nAxisXFontSixe, m_nGValueFontSize;
		int 	m_nLBtnFlag;
		int		m_nMouseOnFlag;
		int		m_nLBtnDnIndex;
		int 	m_nLBtnMoveOffset;
		CPoint  m_cpLBtnDnPoint;
		UINT	m_SeekMessageID;

		CTextData* 	m_pSensorData;
		DWORD	    	m_dwCTS;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGSensorView)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID=NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSeekMessageId(UINT id) { m_SeekMessageID = id; };
	void SetXRange(double dLower, double dUpper, int nDecimalPlaces);
	void SetSpeedRange(int speed_type, double dSpeedRange = 0.0);
	void SetSpeedRange(double dSpeedRange);
	void InvalidateCtrl();
	int RefreshDraw(int nRefreshPoint = NULL);
	void DrawPoints(DWORD pos);
	void ErasePoint(void);
	void AppendPoint(double dPointX, double dPointY, double dPointZ,  double dSpeed = NULL, bool bRedraw = NULL, int event = NULL, DWORD time = NULL);
	void AppendPoint(double dPointX, double dPointY, double dPointZ, double dSpeed,  int event, DWORD time);
	void DrawTrackBar(void);
	int TimeFormatText(CString *pcstr, double time);
	void DrawAxisXText(double min, double max, double trackbar = NULL);
	void DrawGValueText(PGVALUE pG);
	void SetTrackBarPosition(int position);
	void SetRange(double dLower, double dUpper, int nDecimalPlaces, double gridInterval = NULL, int speed_type = SPEED_KM_P_H, double dSpeedRange = 0.0);
	void SetXUnits(CString string);
	void SetYUnits(CString string);
	void SetGridColor(COLORREF color, COLORREF textColor = NULL);
	void SetTrackBarColor(COLORREF color);
	void SetGridScaleX(double dScaleX);
	void SetPlotColor(COLORREF colorX, COLORREF colorY, COLORREF colorZ, COLORREF colorS);
	void SetEventColor(COLORREF* pColor, int cnt);
	void SetBackgroundColor(COLORREF color);
	void SendSeekMsg(int nSeekPoint, int pauseFlay);

	void Synchronize			( DWORD dwCTS );
	BOOL SetSensorData		( CTextData* pSensorData );
	BOOL GetCurrentSensorData (PGVALUE pGValue);
	virtual ~CGSensorView(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CGSensorView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
