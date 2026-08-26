#if !defined(AFX_ANALOGMETER_H__7B6C2C14_9681_11D2_9136_0020AF9FE21E__INCLUDED_)
#define AFX_ANALOGMETER_H__7B6C2C14_9681_11D2_9136_0020AF9FE21E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AnalogMeter.h : header file
//

#include <CLENAW/ClIBGTransfer.h>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClHashedString.H>
#include <HELPER/ClDrawImage.H>

/////////////////////////////////////////////////////////////////////////////
// CAnalogMeter command target

class CAnalogMeter : public CStatic, IBGTransfer
{
	DECLARE_DYNCREATE(CAnalogMeter)

	CAnalogMeter() ;
	virtual ~CAnalogMeter();

// Attributes
public:
	COLORREF m_colorGrid;
	COLORREF m_colorNeedle;
	COLORREF m_colorValue;

// Operations
public:
	virtual VOID					TransferBackground	( CDC* pDC );

	afx_msg	void					OnPaint				();
	afx_msg	BOOL					OnEraseBkgnd		( CDC* pDC );
	
	BOOL					SetBackground		( const HashedString& imagePath,
													Color color = Color((ARGB)Color::White),
													BOOL bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
													ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
													const ClGeometry& geoImage = GEO_FULL_SIZE,
													const ClGeometry& geoSubImage = GEO_FULL_SIZE,
													DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
	ClDrawImage*			GetBackground		( VOID );
	

// function
public:		
	void ShowMeter(void);
	void UpdateNeedle(double dPos, bool bRedraw = false);
	void SetMeterScale(int nDivision);
	void SetColor(COLORREF grid, COLORREF value, COLORREF needle);
	void SetRange (double dMin, double dMax);
	void SetRangeDecimals(int nRangeDecimals);
	void SetValueDecimals(int nValueDecimals);
	void SetTitle(CString strTitle);
	double GetMaxRange()   {return m_dMaxScale; } ;
	double GetMinRange()   {return m_dMinScale; } ;
	int GetRangeDecimals() {return m_nRangeDecimals; } ;
	int GetValueDecimals() {return m_nValueDecimals; } ;
	CString GetTitle()     {return m_strTitle; } ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalogMeter)
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CalculationValue () ;
	void DrawGrid ();
	void DrawNeedle();
	void ShowMeterImage(CDC *pDC);

	bool m_boolUseBitmaps ;
	bool m_boolForceRedraw;
	bool m_boolUseGrid;
	bool m_boolUseBG;
	
	int m_nRangeDecimals;
	int m_nValueDecimals;
	int m_nRectWidth;
	int m_nRectHeight;
	int m_nCXPix;
	int m_nCYPix;
	int m_nRadiusPix;
	int m_nLeftLimitXPix;
	int m_nLeftLimitYPix;
	int m_nRightLimitXPix;
	int m_nRightLimitYPix;
	int m_nHalfBaseWidth;
	int m_nTextBaseSpacing;

	int m_nMeterScale;		// Division value(ex: 1000 is "rpm x1000")
	
	double m_dLimitAngleDeg;
	double m_dPI;
	double m_dLimitAngleRad;
	double m_dRadiansPerValue;
	double m_dNeedlePos;
	double m_dMinScale; 
	double m_dMaxScale;

	CString m_strTitle;

	CDC m_dcGrid;
	CBitmap m_bitmapGrid;
	CBitmap *m_pbitmapOldGrid;

	CDC m_dcNeedle ;
	CBitmap m_bitmapNeedle;
	CBitmap *m_pbitmapOldNeedle;
	
	CRect m_rectDraw;
	CRect m_rectOwner;
	CRect m_rectMinValue;
	CRect m_rectMaxValue;
	CRect m_rectValue;
	
	CFont m_fontValue;
	CBrush m_brushErase;
	CPen m_penErase;

	ClDrawImage*			m_pBackground;

	// Generated message map functions
	//{{AFX_MSG(CAnalogMeter)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALOGMETER_H__7B6C2C14_9681_11D2_9136_0020AF9FE21E__INCLUDED_)
