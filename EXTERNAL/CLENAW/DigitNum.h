#ifndef _DIGITNUM_H
#define _DIGITNUM_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DigitNum.h : header file
//

#include <CLENAW/ClIBGTransfer.h>
#include <HELPER/ClGeometry.H>
#include <HELPER/ClHashedString.H>
#include <HELPER/ClDrawImage.H>
#include <HELPER/ClGeometry.H>

#include <list>

// Min number of digits we can display
#define ST_MIN_PRECISION	1
// Max number of digits we can display (it can be increased)
#define ST_MAX_PRECISION	20
// Space left around the bitmap for border
#define BORDER_SPACE		3


class CDigitNum : public CStatic, IBGTransfer
{
	DECLARE_DYNAMIC(CDigitNum)
public:
	CDigitNum();
	virtual ~CDigitNum();

	enum SKIN_TYPE
	{
		SKIN_DIGIT		= 0	,
		SKIN_DOT		,
		SKIN_UNIT			,
		SKIN_INVALID
	};

	enum SKIN_UNIT
	{
		UNIT_KM	= 0 ,
		UNIT_MI,
		UNIT_KN,
		UNIT_INVALID
	};


	typedef std::list<ClDrawImage*>					BG_POOL;
	typedef BG_POOL::iterator							ITER_BG;
	
public:

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDigitNum)
	//}}AFX_VIRTUAL

public:
	BOOL 					CalcGeometry			( int nSkinType, int nSkinPos, int nTargetPos, int nValueLangth, ClGeometry& geoTarget, ClGeometry& geoSource );
	BOOL					SetSkin				( SKIN_TYPE type, int nSkinPiece, const HashedString& imagePath,
													Color color = Color((ARGB)Color::White),
													BOOL bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
													ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
													const ClGeometry& geoImage = GEO_FULL_SIZE,
													const ClGeometry& geoSubImage = GEO_FULL_SIZE,
													DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
	ClDrawImage*			GetSkin				( SKIN_TYPE type );
	VOID					ClearSkins				( VOID );

	BOOL					SetBackground		( const HashedString& imagePath,
													Color color = Color((ARGB)Color::White),
													BOOL bUseTpColor = FALSE, Color tpColor = Color((ARGB)Color::Black),
													ClDrawImage::DRAW_MODE drawMode = ClDrawImage::MODE_STRETCH,
													const ClGeometry& geoImage = GEO_FULL_SIZE,
													const ClGeometry& geoSubImage = GEO_FULL_SIZE,
													DWORD dwGeoBase = GEOX_LEFT | GEOY_TOP );
	ClDrawImage*			GetBackground		( INT nIndex );
	VOID					ClearBackgrounds	( VOID );
public:

	virtual void					DrawItem			( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual VOID					TransferBackground	( CDC* pDC );
//	virtual VOID					SetWindowText(CString cstrValue ) {m_cstrValue = cstrValue;}
	
	afx_msg	void					OnPaint				();
	afx_msg	BOOL					OnEraseBkgnd		( CDC* pDC );
	afx_msg void					OnLButtonDown		( UINT nFlags, CPoint point );
	afx_msg void					OnLButtonUp			( UINT nFlags, CPoint point );
	afx_msg void					OnMouseMove			( UINT nFlags, CPoint point );
	afx_msg void					OnKeyDown			( UINT nChar, UINT nRepCnt, UINT nFlags );


protected:

	DECLARE_MESSAGE_MAP()
		
protected:
	ClDrawImage*			m_pSkinList[SKIN_INVALID];
	INT						m_anSkinPiece[SKIN_INVALID];
	
	BG_POOL					m_BackgroundList;
	BOOL					m_bIsButtonDown;
	INT						m_nMouseOffset;	

public:
	void SetValue(int nValue, BOOL bRepaint = TRUE);
	int GetValue();

	void SetUnit(int nUnit) { if(m_nUnit != nUnit) { m_nUnit = nUnit;	DrawDigits(m_nValue, 1);/*Invalidate();*/} }
	int GetUnit (void) {return m_nUnit;}
	void SetPrecision(int nPrecision);
	int GetPrecision();

	void Inc(BOOL bRepaint = TRUE);
	void Dec(BOOL bRepaint = TRUE);
	void DrawDigits(int nValue, BOOL redraw = FALSE); //unit km/h
	void DrawDigits(double dValue, BOOL redraw = FALSE); //unit km/h
	void DrawDigits(CString cstrValue);
	void DrawDigits(CDC* pDC = NULL);

private:
	int m_nWidth;
	int m_nHeight;
	CRect m_rectImage;
	
	int m_nPrecision;
	int m_nValue;
	double m_dValue;
	CString m_cstrValue;

	BOOL m_bZeroPadding;
	DWORD m_dwResize;
	int		m_nDigitDotOffsetX;
	CString	m_cstrDigitText;
	CString	m_cstrDotText;
	int 		m_nUnit;	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
