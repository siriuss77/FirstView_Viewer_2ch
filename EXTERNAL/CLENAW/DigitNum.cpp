// DigitNum.cpp : implementation file
//

#include "stdafx.h"
#include "DigitNum.h"

#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDigitNum

IMPLEMENT_DYNAMIC(CDigitNum, CStatic)

CDigitNum::CDigitNum()
{
	// Default number of digits
	m_nPrecision = 8;

	// Default value
	m_nValue = 999;
	m_dValue = 0.0;
	m_cstrValue = TEXT("");

	m_nWidth = 0;
	m_nHeight = 0;

	m_cstrDigitText = TEXT("0123456789NEWS");
	m_cstrDotText = TEXT("-:.`\'\"");
	m_nUnit	= UNIT_KM;

	m_nDigitDotOffsetX = 0;

	this->m_bIsButtonDown = FALSE;

	INT nSkinIndex;
	for( nSkinIndex = 0 ; nSkinIndex < SKIN_INVALID; nSkinIndex ++ )
	{
		this->m_pSkinList[nSkinIndex] = NULL;
		this->m_anSkinPiece[nSkinIndex] = 0;
	}
}


CDigitNum::~CDigitNum()
{
	INT nSkinIndex;
	for( nSkinIndex = 0 ; nSkinIndex < SKIN_INVALID; nSkinIndex ++ )
	{
		if( this->m_pSkinList[nSkinIndex] )
			delete this->m_pSkinList[nSkinIndex];
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDigitNum message handlers

BEGIN_MESSAGE_MAP(CDigitNum, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CDigitNum::OnPaint()
{
	CPaintDC dc(this);

	DrawDigits(&dc);
	return;
}


BOOL CDigitNum::OnEraseBkgnd( CDC* pDC )
{
	UNREFERENCED_PARAMETER( pDC );

	return FALSE;
}


void CDigitNum::OnLButtonDown( UINT nFlags, CPoint point )
{
	this->m_bIsButtonDown = TRUE;
	SetCapture();

	this->m_nMouseOffset = 0;
	OnMouseMove( nFlags, point );

	CStatic::OnLButtonDown( nFlags, point );
}


void CDigitNum::OnMouseMove( UINT nFlags, CPoint point )
{
	if( !this->m_bIsButtonDown )
		return;

	CStatic::OnMouseMove( nFlags, point );
}


void CDigitNum::OnLButtonUp( UINT nFlags, CPoint point )
{
	this->m_bIsButtonDown = FALSE;
	this->m_nMouseOffset = 0;
	ReleaseCapture();

	CStatic::OnLButtonUp( nFlags, point );
}


void CDigitNum::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	CStatic::OnKeyDown( nChar, nRepCnt, nFlags );
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL

void CDigitNum::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	UNREFERENCED_PARAMETER( lpDrawItemStruct );
	return;
}


VOID CDigitNum::TransferBackground( CDC* pDC )
{
	this->m_dcBackground.DeleteDC();

	CBitmap bmp, *pOldBitmap;
	CRect rtWindow;
	GetWindowRect(rtWindow);
	this->GetParent()->ScreenToClient( rtWindow );

	m_dcBackground.CreateCompatibleDC( pDC );
	bmp.CreateCompatibleBitmap( pDC, rtWindow.Width(), rtWindow.Height() );
	pOldBitmap = m_dcBackground.SelectObject( &bmp );
	m_dcBackground.BitBlt( 0, 0, rtWindow.Width(), rtWindow.Height(), pDC, rtWindow.left, rtWindow.top, SRCCOPY );
	bmp.DeleteObject();

//	INT nCount;
//	for( nCount = (INT)SKIN_DIGIT; nCount < (INT)SKIN_INVALID; nCount++ )
//		if( this->m_pSkinList[nCount] ) this->m_pSkinList[nCount]->setUpdate();
		
	return;
}

BOOL CDigitNum::CalcGeometry( int nSkinType, int nSkinPos, int nTargetPos, int nValueLength, ClGeometry& geoTarget, ClGeometry& geoSource )
{
	if(nSkinType < 0 || nSkinType >= SKIN_INVALID || nTargetPos > m_nPrecision)
		return FALSE;

	CRect rect;
	GetClientRect(rect);
	int nStartPos = m_nPrecision - nValueLength;
	
	geoTarget = ClGeometry( 0, 0, rect.Width(), rect.Height() );

	if(m_anSkinPiece[SKIN_UNIT])
		geoTarget.width = geoTarget.width / (m_nPrecision + 1);
	else
		geoTarget.width = geoTarget.width / m_nPrecision;
	
	geoTarget.x = geoTarget.x + (geoTarget.width * (nStartPos + nTargetPos)) + m_nDigitDotOffsetX;

	if(nSkinType && nSkinType != SKIN_UNIT)
	{
		double dMultiSkinWidthRate = this->m_pSkinList[0]->getGeometry().width / this->m_pSkinList[1]->getGeometry().width;
		int nOffsetWidth = (geoTarget.width - (geoTarget.width / dMultiSkinWidthRate));

		geoTarget.width -= nOffsetWidth;
		m_nDigitDotOffsetX -= nOffsetWidth;
	}
		
	geoSource = this->m_pSkinList[nSkinType]->getGeometry();

	geoSource.width = geoSource.width / m_anSkinPiece[nSkinType];
	geoSource.x = geoSource.x + (geoSource.width * nSkinPos);
	return TRUE;
}


BOOL CDigitNum::SetSkin( CDigitNum::SKIN_TYPE type, int nSkinPiece, const HashedString& imagePath, Color color, BOOL bUseTpColor, Color tpColor,
					   ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
					   DWORD dwGeoBase )
{
	if( type >= SKIN_INVALID )
		return FALSE;

	m_anSkinPiece[type] = nSkinPiece;
	
	if( !this->m_pSkinList[type] )
	{
		this->m_pSkinList[type] = new ClDrawImage();
		if( !this->m_pSkinList[type] )
			return FALSE;
	}

	if( !this->m_pSkinList[type]->loadImage(imagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		delete this->m_pSkinList[type];
		this->m_pSkinList[type] = NULL;
		return FALSE;
	}

	return TRUE;
}


ClDrawImage* CDigitNum::GetSkin( CDigitNum::SKIN_TYPE type )
{
	if( type == SKIN_INVALID )
		return NULL;

	return this->m_pSkinList[type];
}


VOID CDigitNum::ClearSkins( VOID )
{
	INT nSkinIndex;
	for( nSkinIndex = (INT)SKIN_DIGIT ; nSkinIndex < SKIN_INVALID; nSkinIndex ++ )
	{
		if( this->m_pSkinList[nSkinIndex] )
			delete this->m_pSkinList[nSkinIndex];
	}

	return;
}


BOOL CDigitNum::SetBackground( const HashedString& imagePath, Color color, BOOL bUseTpColor, Color tpColor,
								ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
								DWORD dwGeoBase )
{
	ClDrawImage* pNewBackground = new ClDrawImage();
	if( !pNewBackground )
	{
		CLD_FL( DF_ERROR, "Out of Memory");
		return FALSE;
	}

	if( !pNewBackground->loadImage(imagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		CLDW_FL( DF_ERROR, TEXT("Cannot load background image %s"), imagePath.c_str() );
		delete pNewBackground;
		return FALSE;
	}

	this->m_BackgroundList.push_back( pNewBackground );

	return TRUE;
}


ClDrawImage* CDigitNum::GetBackground( INT nIndex )
{
	if( nIndex < 0 )
		return NULL;

	ITER_BG iBG = this->m_BackgroundList.begin();
	std::advance( iBG, nIndex );
	if( iBG == this->m_BackgroundList.end() )
		return NULL;

	return (*iBG);
}


VOID CDigitNum::ClearBackgrounds( VOID )
{
	ITER_BG iBG = this->m_BackgroundList.begin();
	ITER_BG iBGEnd = this->m_BackgroundList.end();
	for( ; iBG != iBGEnd; iBG++ )
		if( *iBG ) delete (*iBG);

	this->m_BackgroundList.clear();

	return;
}

void CDigitNum::SetValue(int nValue, BOOL bRepaint)
{
	// Set new value
	m_nValue = nValue;
	if(bRepaint)
		DrawDigits(m_nValue);
} // End of SetValue


int CDigitNum::GetValue()
{
	return m_nValue;
} // End of GetValue


void CDigitNum::SetPrecision(int nPrecision)
{
	// Set number of digits
	// Some security
	if (nPrecision >= ST_MIN_PRECISION && nPrecision <= ST_MAX_PRECISION)
		m_nPrecision = nPrecision;
	
} // End of SetPrecision


int CDigitNum::GetPrecision()
{
	return m_nPrecision;
} // End of GetPrecision

void CDigitNum::Inc(BOOL bRepaint)
{
	SetValue(GetValue() + 1, bRepaint);
} // End of Inc


void CDigitNum::Dec(BOOL bRepaint)
{
	SetValue(GetValue() - 1, bRepaint);
} // End of Dec

void CDigitNum::DrawDigits(int nValue, BOOL redraw)
{
	if(nValue != m_nValue || redraw)
	{
		m_nValue = nValue;

		switch(m_nUnit)
		{
		case UNIT_MI: 	m_cstrValue.Format(TEXT("%0d"), (int)(m_nValue * 0.621371));	break;
		case UNIT_KN: 	m_cstrValue.Format(TEXT("%0d"), (int)(m_nValue * 0.539957));	break;		
		
		case UNIT_KM: 	
		default: 		m_cstrValue.Format(TEXT("%0d"),  m_nValue);	break;
		}

		this->SetWindowText(m_cstrValue);
		
		//DrawDigits();
		Invalidate();
	}
}
void CDigitNum::DrawDigits(double dValue, BOOL redraw)
{
	if(m_dValue != dValue || redraw)
	{
		m_dValue = dValue;
		m_nValue = (int)dValue;
		switch(m_nUnit)
		{
		case UNIT_MI: 	m_cstrValue.Format(TEXT("%.1f"), (m_dValue * 0.621371));	break;
		case UNIT_KN: 	m_cstrValue.Format(TEXT("%.1f"), (m_dValue * 0.539957));	break;		
		
		case UNIT_KM: 	
		default: 		m_cstrValue.Format(TEXT("%.1f"),  m_dValue);	break;
		}

		this->SetWindowText(m_cstrValue);
		//DrawDigits();
		Invalidate();
	}
}
void CDigitNum::DrawDigits(CString cstrValue)
{
	if(m_cstrValue != cstrValue)
	{
		m_cstrValue = cstrValue;
		this->SetWindowText(m_cstrValue);
		//DrawDigits();

		Invalidate();
	}
}
void CDigitNum::DrawDigits(CDC* pDC /* = NULL */ )
{
	char szValue[ST_MAX_PRECISION+1];

	int nLoop;
	int nSkinPos;
	CPaintDC aDC(this);
	
	if(pDC == NULL)
		pDC = &aDC;
	
	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( pDC, &rect, TRUE );

	TCHAR buffer[128] = TEXT("");
	this->GetWindowText(buffer, 128);
	m_cstrValue.Format(TEXT("%s"), buffer);
	if(memDC.m_hDC)
	{
		int nValueOffset = 0;
		TCHAR* pValue = m_cstrValue.GetBuffer();
		int nValueLangth = m_cstrValue.GetLength();

		memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );

		ITER_BG iBG = this->m_BackgroundList.begin();
		ITER_BG iBGEnd = this->m_BackgroundList.end();
		for( ; iBG != iBGEnd; iBG++ )
		{
			(*iBG)->prepareImage( &memDC, rect );
			(*iBG)->drawImage( &memDC, rect );
		}

		
		if(nValueLangth > m_nPrecision)
		{
			nValueOffset = nValueLangth - m_nPrecision;
			nValueLangth = m_nPrecision;
		}	

		m_nDigitDotOffsetX = 0;
			
		for (nLoop = 0; nLoop < nValueLangth; nLoop++)
		{
			INT nSkinType = -1;
			
			if (m_bZeroPadding == TRUE && m_nValue >= 0)
				nSkinPos = 0;
			else
				nSkinPos = 10;

			nSkinPos = m_cstrDigitText.Find(pValue[nLoop + nValueOffset]);
			if(nSkinPos >= 0)
			{
				nSkinType = SKIN_DIGIT;
			}
			else
			{
				nSkinPos = m_cstrDotText.Find(pValue[nLoop + nValueOffset]);
				if(nSkinPos >= 0)
					nSkinType = SKIN_DOT;
			}

			if(nSkinType >= 0 && nSkinType < SKIN_INVALID)
			{
				if( this->m_pSkinList[nSkinType] && nSkinPos <  m_anSkinPiece[nSkinType])
				{
					ClGeometry geoTarget, geoSource;
					this->CalcGeometry( nSkinType,nSkinPos, nLoop, nValueLangth, geoTarget, geoSource );


					this->m_pSkinList[nSkinType]->prepareImage( &memDC, rect, &this->m_dcBackground );
					this->m_pSkinList[nSkinType]->drawImage( &memDC, rect, &geoTarget, &geoSource );
				}
			}
			
		}

		//draw unit km/h, mi/h, knot 
		if( nValueLangth && this->m_pSkinList[SKIN_UNIT] && m_nUnit <  m_anSkinPiece[SKIN_UNIT])
		{
			ClGeometry geoTarget, geoSource;
			this->CalcGeometry( SKIN_UNIT, m_nUnit, nLoop, nValueLangth, geoTarget, geoSource );
					
			this->m_pSkinList[SKIN_UNIT]->prepareImage( &memDC, rect, &this->m_dcBackground );
			this->m_pSkinList[SKIN_UNIT]->drawImage( &memDC, rect, &geoTarget, &geoSource );
		}
	}
} // End of DrawDigits

#undef ST_MIN_PRECISION
#undef ST_MAX_PRECISION
#undef BORDER_SPACE
