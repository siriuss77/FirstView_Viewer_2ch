/************************************************************************************
File Name		:	CGSensorView.cpp

Class Name		:	CGSensorView

Description		:	This file contains the functionalites for the Graph

Developed by	:	B.Manivannan

Date			:	24-10-2001
*************************************************************************************/
#include "stdafx.h"
#include <math.h>
#include "resource.h"
#include "GSensorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GraphPositionY(g) ( m_rectPlot.bottom - (long)((g - m_dLowerLimit) * m_dVerticalFactor) )	// gsensor
//#define GraphPositionY(g)	(m_rectPlot.CenterPoint().y  - (g / m_dXUpper) * (m_rectPlot.Height()/2))

#define GraphPositionY_S(s) ( m_rectPlot.bottom - (long)(s * m_dSpeedVerticalFactor) )	// speed

CGPoint::CGPoint()
{
}
CGPoint::CGPoint(int px, int pxy, int pyy, int pzy, int psy, int event)
{
	x = px;
	xy = pxy;
	yy = pyy;
	zy = pzy;
	sy = psy;
	evt = event;
}
CGPoint::CGPoint(const GPOINT& srcGPoint)
{
	x = srcGPoint.x;
	xy =  srcGPoint.xy;
	yy =  srcGPoint.yy;
	zy =  srcGPoint.zy;
	sy = srcGPoint.sy;
	evt = srcGPoint.evt;
}
CGPoint::CGPoint(LPGPOINT lpSrcGPoint)
{
	x = lpSrcGPoint->x;
	xy =  lpSrcGPoint->xy;
	yy =  lpSrcGPoint->yy;
	zy =  lpSrcGPoint->zy;
	sy = lpSrcGPoint->sy;
	evt = lpSrcGPoint->evt;
}
void CGPoint::SetGPoint(int px, int pxy, int pyy, int pzy, int psy, int event) 
{
	x = px;
	xy = pxy;
	yy = pyy;
	zy = pzy;
	sy = psy;
	evt = event;
}
void CGPoint::operator=(const GPOINT& srcGPoint)
{
	x = srcGPoint.x;
	xy =  srcGPoint.xy;
	yy =  srcGPoint.yy;
	zy =  srcGPoint.zy;
	sy =  srcGPoint.sy;
	evt = srcGPoint.evt;
}


/************************************************************************************
Function Name	:	CGSensorView

Class Name		:	CGSensorView

Description		:	This constructor initialises the attributes

Argument		:	Nothing

Return type		:	void
*************************************************************************************/
#define TIMER_LBUTTON_DOWN		1000
#define TIMER_MOUSE_ON_CHECK	1001

CGSensorView::CGSensorView()
	:m_oGValue(NULL)
{
	int i;
	m_bRedrawBlocked = FALSE;
	m_nLBtnFlag = 0;
	m_nMouseOnFlag = 0;
	m_SeekMessageID = 0;
	m_nDrawIndex = 0;
	m_nLastIndexPoint = 0;
	m_nLastRefreshPoint = 0;
	m_nLastIndex = 0;
	m_nYDecimals = 0 ;
	m_nXDecimals = 0 ;
	m_nGDecimals = 2 ;
	m_dScaleX = 1;
    m_dLowerLimit = 0 ;
	m_dUpperLimit =  100 ;
	m_nMaxShowIndexCnt = 100;
	m_dGridInterval = 0;
	m_dRange      =  m_dUpperLimit - m_dLowerLimit ;   // protected member variable

	m_dXLower = 0 ;
	m_dXUpper =  100 ;
	m_XRange      =  m_dXUpper - m_dXLower;   // protected member variable
	m_nTrackbarPosition = (int)m_XRange;

	m_nGValueSize = MAX_GPOINT;
	m_oGValue = new GVALUE[m_nGValueSize];
	
		
	SetSpeedRange(SPEED_KM_P_H);
	
	m_dLastPoint.SetGPoint(0, 0, 0, 0, 0);
	m_dNewPoint.SetGPoint(0, 0, 0, 0, 0);
	
	// background, grid and data colors
	// these are public variables and can be set directly
	m_crBackColor  = RGB(  0,   0,   0) ;  // see also SetBackgroundColor
	m_crGridColor  = RGB(  0, 255, 255) ;  // see also SetGridColor
	m_crGridTextColor = m_crGridColor;
	
	m_crTrackBarColor = RGB(255, 255, 255);
	for( i = 0; i < 4; i++ )
	{
		m_crPlotColor[i]  = RGB(255, 255, 255) ;  // see also SetPlotColor

		// protected variables
		m_penPlot[i].CreatePen(PS_SOLID, 0, m_crPlotColor[i]) ;
	}
	
	for( i = 0; i < MAX_EVENT_TYPE; i++)
	{
		m_crEventColor[i] = RGB(0, 0, 0);
	}
	m_brushEvent.CreateSolidBrush(m_crEventColor[0]);
		
	m_brushBack.CreateSolidBrush(m_crBackColor) ;
	m_tracbarBrush.CreateSolidBrush(m_crTrackBarColor);
	// public member variables, can be set directly 
	
	m_strXUnitsString.Format(_T("Time in seconds")) ;  // can also be set with SetXUnits

	m_nAxisXFontSixe = 14;
	m_nGValueFontSize = 14;
	
	m_fontGValue.CreateFont (m_nGValueFontSize, 0, 0, 0, 300,
                       FALSE, FALSE, 0, ANSI_CHARSET,
                       OUT_DEFAULT_PRECIS, 
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, 
                       DEFAULT_PITCH|FF_SWISS, _T("Arial")) ;
	 
	m_fontAxisX.CreateFont (m_nAxisXFontSixe, 0, 0, 0, 300,
                       FALSE, FALSE, 0, ANSI_CHARSET,
                       OUT_DEFAULT_PRECIS, 
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, 
                       DEFAULT_PITCH|FF_SWISS, _T("Arial")) ;

	// protected bitmaps to restore the memory DC's
	m_pbitmapOldGrid = NULL ;
	m_pbitmapOldPlot = NULL ;

	m_pSensorData = NULL;
	m_dwCTS = 0;
}

CGSensorView::~CGSensorView()
{
	if(m_oGValue)
		delete []m_oGValue;
}


BEGIN_MESSAGE_MAP(CGSensorView, CWnd)
	//{{AFX_MSG_MAP(CGSensorView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/************************************************************************************
Function Name	:	ErasePoint

Class Name		:	CGSensorView

Description		:	

Argument		:	Nothing

Return			:	Nothing
*************************************************************************************/

void CGSensorView::ErasePoint(void)
{
	m_bRedrawBlocked = FALSE;
	m_nLastRefreshPoint = 0;
	m_nLastIndexPoint=0;
	m_nLastIndex=0;
	m_nDrawIndex = 0;
//	m_dcPlot.FillRect(CRect(m_rectClient.left, m_rectClient.top, m_rectClient.right, m_rectPlot.bottom), &m_brushBack) ;
	InvalidateCtrl();
//	Invalidate();
}
/************************************************************************************
Function Name	:	AppendPoint

Class Name		:	CGSensorView

Description		:	It is appending the points to the graph. It is being called from the OnInitDialog

Argument		:	double dX, double dY, double dZ, bool bRedraw, int event, DWORD time

Return			:	Nothing
*************************************************************************************/
void CGSensorView::AppendPoint(double dX, double dY, double dZ, double dS, bool bRedraw, int event, DWORD time)
{
	DWORD pos = m_nLastIndexPoint;

	m_nLastIndexPoint++;

	if(m_nLastIndexPoint >= m_nGValueSize)
	{
		PGVALUE pGValue = NULL;
		
		
		pGValue = new GVALUE[m_nGValueSize + MAX_GPOINT];

		if(pGValue && m_oGValue && m_nGValueSize)
			memcpy((void *)pGValue, (void *)m_oGValue, sizeof(GVALUE) * m_nGValueSize);

		delete []m_oGValue;
		m_oGValue = pGValue;
		m_nGValueSize += MAX_GPOINT;
	}
	m_oGValue[pos].GX = dX;
	m_oGValue[pos].GY = dY;
	m_oGValue[pos].GZ = dZ;
	m_oGValue[pos].speed= dS;
	m_oGValue[pos].time = time;
	
	if(bRedraw)
	{
		DrawPoints(pos);

		if(time)
		{
			if(pos < m_nMaxShowIndexCnt)
				DrawAxisXText(0, (double)time/1000);
			else
				DrawAxisXText((double)m_oGValue[pos - m_nMaxShowIndexCnt].time/1000, (double)time/1000);
		}
		DrawGValueText(&m_oGValue[pos]);
		Invalidate();
	}
	
}
void CGSensorView::AppendPoint(double dPointX, double dPointY, double dPointZ, double dSpeed, int event, DWORD time)
{
	AppendPoint(dPointX, dPointY, dPointZ, dSpeed, 0, event, time);
}

/************************************************************************************
Function Name	:	RefreshDraw

Class Name		:	CGSensorView

Description		:	It draws the curve to given points and identify the proper pixels

Argument		:	int nRefreshPoint

Return			:	int m_nLastIndexPoint
*************************************************************************************/
int CGSensorView::RefreshDraw(int nRefreshPoint)
{
  	int drawCnt = 0, i = 0, drawStart = 0;
	int trackbarOffset = 0;
	static int drawEnd = 0;

	if(nRefreshPoint >= m_nLastIndexPoint)
		return false;

	if( nRefreshPoint == m_nLastRefreshPoint && nRefreshPoint)
		return true;

#if DEF_DISPLAY_TRACKBAR
	if(m_rectPlot.Width() > m_nTrackbarPointXFactor)
		trackbarOffset = (int)((m_rectPlot.Width() - m_nTrackbarPointXFactor) / m_dScaleX);
#else
	trackbarOffset = m_rectPlot.Width();
#endif
	
	if(m_nLastRefreshPoint == 0 || nRefreshPoint < m_nLastRefreshPoint ||  (nRefreshPoint - m_nLastRefreshPoint) >= m_nMaxShowIndexCnt)
	{
		drawStart = nRefreshPoint - m_nMaxShowIndexCnt;
		drawCnt = m_nMaxShowIndexCnt;
		m_nLastRefreshPoint = 0;
		m_nDrawIndex = 0;
		if(drawStart < 0)
		{
			drawStart = 0;
		}
	}
	else
	{	
		drawStart = drawEnd;

		if(m_nLastRefreshPoint < trackbarOffset)
			drawCnt = nRefreshPoint - trackbarOffset;
		else
			drawCnt = nRefreshPoint - m_nLastRefreshPoint;
		
	}

	if(trackbarOffset >= nRefreshPoint)
	{
		if(m_nLastRefreshPoint != 0)
			drawCnt = 0;
	}
	else if(m_nMaxShowIndexCnt == drawCnt)
	{
		if(nRefreshPoint >= trackbarOffset * 2)
			drawStart += trackbarOffset;
		else if(nRefreshPoint > trackbarOffset)
			drawStart += (nRefreshPoint - trackbarOffset);
	}

	for(i = 0; i < drawCnt; i++)
	{
		int point = drawStart + i ;
		
		if(point  >= m_nLastIndexPoint)
		{
			if(m_bRedrawBlocked)
				return false;
			else
				AppendPoint(0, 0, 0, 0, 0, 0, m_oGValue[m_nLastIndexPoint-1].time + 100);
		}
		DrawPoints(point);
	}
	
	m_nLastRefreshPoint = nRefreshPoint ;
	drawEnd = drawStart + drawCnt;

	if(m_nDrawIndex >= m_nMaxShowIndexCnt && drawEnd >= m_nMaxShowIndexCnt)
		DrawAxisXText((double)m_oGValue[drawEnd - m_nMaxShowIndexCnt].time/1000, (double)m_oGValue[drawEnd].time/1000, trackbarOffset ? (double)m_oGValue[drawEnd - m_nMaxShowIndexCnt + trackbarOffset].time/1000: 0) ;

	DrawGValueText(&m_oGValue[m_nLastRefreshPoint]);
	Invalidate() ;

	return m_nLastIndexPoint ;
}

/************************************************************************************
Function Name	:	DrawPoints

Class Name		:	CGSensorView

Description		:	It draws the curve to given points and identify the proper pixels

Argument		:	Nothing

Return			:	Nothing
*************************************************************************************/
void CGSensorView::DrawPoints(DWORD pos)
{
  CPen *oldPen ;
  CRect rectCleanUp ;
  if (m_dcPlot.GetSafeHdc() != NULL)
  {

	if(pos < m_nLastIndexPoint)
	{
	    	int x = ((int)(pos * m_dScaleX) % m_rectPlot.Width());

		if(pos && x == 0)
			x = m_rectPlot.Width();
			
		m_dNewPoint.x = x + m_rectPlot.left;
		
		m_dNewPoint.xy 	= (int)GraphPositionY(m_oGValue[pos].GX);
		m_dNewPoint.yy 	= (int)GraphPositionY(m_oGValue[pos].GY);
		m_dNewPoint.zy 	= (int)GraphPositionY(m_oGValue[pos].GZ);

		m_dNewPoint.sy 	= (int)GraphPositionY_S(m_oGValue[pos].speed * m_dSpeedFactor);
	}

	
    if(m_nDrawIndex == 0 || m_dLastPoint.x > m_dNewPoint.x)
	{
		m_dLastPoint.x =  m_rectPlot.left;
		if(m_nDrawIndex == 0 )
		{
			m_dLastPoint.xy = m_rectPlot.CenterPoint().y;
			m_dLastPoint.yy = m_rectPlot.CenterPoint().y;
			m_dLastPoint.zy = m_rectPlot.CenterPoint().y;
			m_dLastPoint.sy = m_rectPlot.bottom;
		}
	}
	 
	 if ((m_dLastPoint.x >= m_rectPlot.left) &&  (m_dNewPoint.x >= m_rectPlot.left))
	{
		 // fill the cleanup area with the background
		m_dcPlot.FillRect(CRect(m_dLastPoint.x, m_rectClient.top, m_dNewPoint.x, m_rectPlot.bottom), &m_brushBack) ;

		// grab the plotting pen
		oldPen = m_dcPlot.SelectObject(&m_penPlot[0]) ;
		// draw the next line segement
		m_dcPlot.MoveTo (m_dLastPoint.x, m_dLastPoint.xy) ;
		m_dcPlot.LineTo (m_dNewPoint.x, m_dNewPoint.xy) ;

		m_dcPlot.SelectObject(&m_penPlot[1]) ;
		m_dcPlot.MoveTo (m_dLastPoint.x, m_dLastPoint.yy) ;
		m_dcPlot.LineTo (m_dNewPoint.x, m_dNewPoint.yy) ;

		m_dcPlot.SelectObject(&m_penPlot[2]) ;
		m_dcPlot.MoveTo (m_dLastPoint.x, m_dLastPoint.zy) ;
		m_dcPlot.LineTo (m_dNewPoint.x, m_dNewPoint.zy) ;

		if(m_crPlotColor[3] != 0xffffff){
			m_dcPlot.SelectObject(&m_penPlot[3]) ;
			m_dcPlot.MoveTo (m_dLastPoint.x, m_dLastPoint.sy) ;
			m_dcPlot.LineTo (m_dNewPoint.x, m_dNewPoint.sy) ;
		}
		// restore the pen 
		m_dcPlot.SelectObject(oldPen) ;
	}
		
	m_dLastPoint = m_dNewPoint;
	m_nDrawIndex++;
  }
}
/************************************************************************************
Function Name	:	InvalidateCtrl

Class Name		:	CGSensorView

Description		:	Redraws all the points. It can be called from any function

Argument		:	Nothing

Return			:	Nothing
*************************************************************************************/
void CGSensorView::InvalidateCtrl()
{

	int nCharacters ;

	CPen *oldPen ;
	CPen solidPen(PS_SOLID, 0, m_crGridColor) ;
	CFont axisFont, yUnitFont, *oldFont ;
	CString strTemp ;

	if(this->m_hWnd == NULL)
		return;
	// in case we haven't established the memory dc's
	CClientDC dc(this) ;  

	GetClientRect(m_rectClient) ;
	// set some member variables to avoid multiple function calls
		
	// if we don't have one yet, set up a memory dc for the grid
	if (m_dcGrid.GetSafeHdc() == NULL || m_rectPreClient != m_rectClient)
	{

		if(m_dcGrid.GetSafeHdc() == NULL)
		{
			m_dcGrid.CreateCompatibleDC(&dc) ;
		}
		else
		{
			m_dcGrid.SelectObject(&m_pbitmapOldGrid) ;
			m_bitmapGrid.DeleteObject();
		}
	
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid) ;
	}
  
	m_dcGrid.SetBkColor (m_crBackColor) ;

	// fill the grid background
	m_dcGrid.FillRect(m_rectClient, &m_brushBack) ;

	// draw the plot rectangle:
	// determine how wide the y axis scaling values are
	  nCharacters = abs((int)log10(fabs(m_dUpperLimit))) ;
	  nCharacters = max(nCharacters, abs((int)log10(fabs(m_dLowerLimit)))) ;

	  // add the units digit, decimal point and a minus sign, and an extra space
	  // as well as the number of decimal places to display
	  nCharacters = nCharacters + 4 + m_nYDecimals ;  

	  // adjust the plot rectangle dimensions
	  // assume 6 pixels per character (this may need to be adjusted)
	  m_rectPlot.left = m_rectClient.left + 11*(nCharacters) ; //10
	  if(m_crPlotColor[3] != 0xffffff)
			 m_rectPlot.right  = m_rectClient.right- (5*(nCharacters));	//10;
	  else
			 m_rectPlot.right  = m_rectClient.right - 2;
			 
	  m_rectPlot.right = m_rectPlot.right - (m_rectPlot.Width() % (int)(m_dScaleX + 0.99));
	  m_nPlotWidth	  = m_rectPlot.Width();

	  m_dLastPoint.x = m_rectPlot.left;

	  m_dVerticalFactor = (double)m_nPlotHeight / m_dRange ; 
	  m_dSpeedVerticalFactor = (double)m_nPlotHeight / m_dSpeedRange;

	m_nTrackbarPointXFactor = (int)(m_rectPlot.Width()/m_XRange * m_nTrackbarPosition);

	  if(m_nTrackbarPointXFactor > m_rectPlot.Width())
		m_nTrackbarPointXFactor = m_rectPlot.Width(); 

	  m_nMaxShowIndexCnt = (int)(m_rectPlot.Width() /m_dScaleX);
	  
	  //m_wndTrackbar.SetWindowPos(NULL, m_rectPlot.left+m_nTrackbarPointXFactor, m_rectPlot.top, 1, m_rectPlot.Height()+1, NULL);

	////
	m_rectRangeG		= m_rectClient;
	m_rectRangeG.right	= m_rectPlot.left;

	m_rectRangeSpeed		= m_rectClient;
	m_rectRangeSpeed.left		= m_rectPlot.right;

	m_rectRangeScale			= m_rectClient;
	m_rectRangeScale.left		= m_rectPlot.left;
	m_rectRangeScale.right		= m_rectPlot.right;
	
	////
	  int i, cnt, intervalY;
	 double interval, interval_speed;
	 if(m_dGridInterval == 0)
	 {
		 cnt = m_rectPlot.Height() / (nCharacters * 4);
		 interval = ((m_dUpperLimit + ((m_dLowerLimit < 0) ? (-m_dLowerLimit) : m_dLowerLimit)) / cnt);
	 }
	 else
	 {
		cnt = (int)((m_dUpperLimit + ((m_dLowerLimit < 0) ? (-m_dLowerLimit) : m_dLowerLimit)) / m_dGridInterval);
		interval = m_dGridInterval;
	 }

	 interval_speed = m_dSpeedRange / cnt;
	 
#if DEF_DISPLAY_TIME_INFO
	 m_rectPlot.bottom = m_rectClient.bottom-(28);
#else
	m_rectPlot.bottom = m_rectClient.bottom-5;
#endif

	 intervalY = m_rectPlot.Height() /cnt;
	 m_rectPlot.bottom = m_rectPlot.top + intervalY * cnt;

	 m_nPlotHeight = m_rectPlot.Height() ;
	m_nPlotWidth  = m_rectPlot.Width() ;
	
	  // draw the plot rectangle
	  oldPen = m_dcGrid.SelectObject (&solidPen) ; 
	  m_dcGrid.MoveTo (m_rectPlot.left, m_rectPlot.top) ;
	  m_dcGrid.LineTo (m_rectPlot.right+1, m_rectPlot.top) ;
	  m_dcGrid.LineTo (m_rectPlot.right+1, m_rectPlot.bottom) ;
	  m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.bottom) ;
	  m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.top) ;
	  m_dcGrid.SelectObject (oldPen) ;
	  
	  yUnitFont.CreateFont (14, 0, 900, 0, 300,
					   FALSE, FALSE, 0, ANSI_CHARSET,
					   OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY, 
					   DEFAULT_PITCH|FF_SWISS, _T("Arial")) ;
	  		
		// grab the horizontal font
	  oldFont = m_dcGrid.SelectObject(&m_fontAxisX) ;

	  
	// y max
	int oldTextAlign;
	oldTextAlign = m_dcGrid.GetTextAlign();
	oldPen = m_dcGrid.SelectObject (&solidPen) ; 
	
	for(i=0; i <= cnt; i++)
	{
		int top = m_rectPlot.top + (int)(intervalY* i);
		
		m_dcGrid.MoveTo (m_rectPlot.left, top) ;
		m_dcGrid.LineTo (m_rectPlot.right+1, top) ;

		if(m_nYDecimals <= 4)
		{
			int nYMaxLength ;

			m_dcGrid.SetTextColor (m_crGridTextColor) ;
			m_dcGrid.SetTextAlign(oldTextAlign | TA_RIGHT) ;
	
			strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dUpperLimit-(interval*i)) ;
			nYMaxLength = strTemp.GetLength();
			m_dcGrid.ExtTextOut((m_rectPlot.left - 4), top - nCharacters,ETO_CLIPPED|ETO_OPAQUE,NULL,strTemp,nYMaxLength,NULL);
		}

		// draw speed
		if(m_crPlotColor[3] != 0xffffff)
		{
			int nYMaxLength ;

			m_dcGrid.SetTextColor (m_crPlotColor[3]) ;
			m_dcGrid.SetTextAlign(oldTextAlign | TA_LEFT) ;
	
			strTemp.Format (_T("%d"), (int)(m_dSpeedRange - (interval_speed*i))) ;
			nYMaxLength = strTemp.GetLength();
			m_dcGrid.ExtTextOut((m_rectPlot.right + 4), top - nCharacters,ETO_CLIPPED|ETO_OPAQUE,NULL,strTemp,nYMaxLength,NULL);
		}
	}

	m_dcGrid.SetTextAlign(oldTextAlign);
	m_dcGrid.SelectObject (oldPen) ;

	DrawAxisXText(m_dXLower, m_dXUpper);

	// x units
	//m_dcGrid.SetTextAlign (TA_CENTER|TA_TOP) ;
	m_dcGrid.SetTextAlign(oldTextAlign | TA_CENTER) ;
	m_dcGrid.ExtTextOut (((m_rectPlot.left+m_rectPlot.right)/2), 
					m_rectPlot.bottom+14, ETO_CLIPPED,NULL,m_strXUnitsString,m_strXUnitsString.GetLength() ,NULL) ;
#if DEF_DISPLAY_TIME_INFO	
	// x scale
	strTemp.Format (_T("Scale:%.1lf"), m_dScaleX) ;
	m_dcGrid.ExtTextOut (m_rectPlot.left, 
					m_rectPlot.bottom+14, ETO_CLIPPED,NULL,strTemp,strTemp.GetLength() ,NULL) ;
#endif

	// restore the font
	m_dcGrid.SelectObject(oldFont) ;

	// y units
	oldFont = m_dcGrid.SelectObject(&yUnitFont) ;
	m_dcGrid.ExtTextOut ((m_rectClient.left+nCharacters), 
					((m_rectPlot.bottom+m_rectPlot.top)/2), ETO_CLIPPED,NULL,m_strYUnitsString,m_strYUnitsString.GetLength() ,NULL) ;
	m_dcGrid.SelectObject(oldFont) ;
	m_dcGrid.SetTextAlign(oldTextAlign) ;
	// at this point we are done filling the the grid bitmap, 
	// no more drawing to this bitmap is needed until the setting are changed

	// if we don't have one yet, set up a memory dc for the plot
	if (m_dcPlot.GetSafeHdc() == NULL || m_rectPreClient != m_rectClient)
	{

		if(m_dcPlot.GetSafeHdc() == NULL)
		{
			m_dcPlot.CreateCompatibleDC(&dc) ;
		}
		else
		{
			m_dcPlot.SelectObject(&m_pbitmapOldPlot) ;
			m_bitmapPlot.DeleteObject();
		}
		
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot) ;
	}	
	m_rectPreClient = m_rectClient;

	// make sure the plot bitmap is cleared
	m_dcPlot.SetBkColor (m_crBackColor) ;
	m_dcPlot.FillRect(m_rectClient, &m_brushBack) ;

	// finally, force the plot area to redraw
	InvalidateRect(m_rectClient) ;	
	
	if(this->m_dwCTS && m_nLastRefreshPoint)
		Synchronize(this->m_dwCTS-1);
}

/************************************************************************************
Function Name	:	Create

Class Name		:	CGSensorView

Description		:	It creates the Graph control

Argument		:	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID

Return			:	BOOL
*************************************************************************************/
BOOL CGSensorView::Create(DWORD dwStyle, const RECT& rect, 
                         CWnd* pParentWnd, UINT nID) 
{
  BOOL result ;
  static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW) ;
  result = CWnd::CreateEx(NULL/*WS_EX_CLIENTEDGE | WS_EX_STATICEDGE*/, 
                          className, NULL, dwStyle, 
                          rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
                          pParentWnd->GetSafeHwnd(), (HMENU)nID) ;
  
  if (result != 0)
  {
  	m_wndTrackbar.CreateEx(NULL,  AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW),  NULL, WS_VISIBLE | WS_CHILD,  0,  0,  1, 1, this->m_hWnd, NULL);
      	InvalidateCtrl() ;
  }
  return result ;
} // Create
/************************************************************************************
Function Name	:	OnSize

Class Name		:	CGSensorView

Description		:	Actually it is a message handle, the window size is being initialized from this handler

Argument		:	UINT nType, int cx, int cy

Return			:	void
*************************************************************************************/
void CGSensorView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	GetClientRect(m_rectClient) ;

	// set some member variables to avoid multiple function calls

	m_nClientHeight = m_rectClient.Height() ;
	m_nClientWidth  = m_rectClient.Width() ;

	// the "left" coordinate and "width" will be modified in 
	// InvalidateCtrl to be based on the width of the y axis scaling
	m_rectPlot.left   = 20 ;
	m_rectPlot.top    = 10 ;
	m_rectPlot.right  = m_rectClient.right-20 ;
	m_rectPlot.bottom = m_rectClient.bottom-25 ;
	
	m_dLastPoint.x = m_rectPlot.left;
	
	// set some member variables to avoid multiple function calls
	m_nPlotHeight = m_rectPlot.Height() ;
	m_nPlotWidth  = m_rectPlot.Width() ;

	// set the scaling factor for now, this can be adjusted 
	// in the SetRange functions
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange ; 
	m_dSpeedVerticalFactor = (double)m_nPlotHeight / m_dSpeedRange;
}

void CGSensorView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDC memDC ;
	CBitmap memBitmap ;
	CBitmap* oldBitmap ; // bitmap originally found in CMemDC
  
	memDC.CreateCompatibleDC(&dc) ;
	memBitmap.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;

	if (memDC.GetSafeHdc() != NULL)
	{
		int occurPoint;
		// first drop the grid on the memory dc
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			         &m_dcGrid, 0, 0, SRCCOPY) ;
		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
//		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
//			         &m_dcPlot, 0, 0, SRCPAINT) ;  //SRCPAINT

		if(m_nLastIndexPoint < m_nMaxShowIndexCnt)
			occurPoint = (m_dLastPoint.x + m_rectPlot.Width() - (int)(m_nLastIndexPoint*m_dScaleX)) % m_rectPlot.Width() + 1;
		else
			occurPoint = m_dLastPoint.x;

		memDC.BitBlt(m_rectPlot.left + 1, m_rectPlot.top, m_rectPlot.right -occurPoint, m_rectPlot.Height(),
			&m_dcPlot, occurPoint, m_rectPlot.top, SRCPAINT);

		memDC.BitBlt(m_rectPlot.left + 1 + m_rectPlot.right -occurPoint, m_rectPlot.top, occurPoint - m_rectPlot.left , m_rectPlot.Height(),
			&m_dcPlot, m_rectPlot.left, m_rectPlot.top, SRCPAINT);

		
		// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			      &memDC, 0, 0, SRCCOPY) ;
	}

	memDC.SelectObject(oldBitmap) ;

	if(m_nLastIndexPoint != m_nLastRefreshPoint) // AppendPoint  trackbar not paint
		DrawTrackBar();
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CGSensorView::DrawTrackBar(void)
{
#if DEF_DISPLAY_TRACKBAR
	static bool drawTrackbar;
	CPaintDC dc(&m_wndTrackbar); // device context for painting
	
	// TODO: Add your message handler code here
	CDC memDC ;
	CBitmap memBitmap ;
	CBitmap* oldBitmap ; // bitmap originally found in CMemDC
  
	memDC.CreateCompatibleDC(&dc) ;
	memBitmap.CreateCompatibleBitmap(&dc, 1, m_rectPlot.Height()+1) ;
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;
	
	if(m_nLastRefreshPoint <= (int)(m_nTrackbarPointXFactor / m_dScaleX))
	{
		m_nTrackbarPointXCurrent = (int)(m_nLastRefreshPoint*m_dScaleX);
		drawTrackbar = false;
		m_wndTrackbar.SetWindowPos(NULL, m_rectPlot.left+m_nTrackbarPointXCurrent, m_rectPlot.top, 1, m_rectPlot.Height()+1, NULL);
	}
	else if(drawTrackbar == false)
	{
		m_nTrackbarPointXCurrent = m_nTrackbarPointXFactor;
		drawTrackbar = true;
		m_wndTrackbar.SetWindowPos(NULL, m_rectPlot.left+m_nTrackbarPointXCurrent, m_rectPlot.top, 1, m_rectPlot.Height()+1, NULL);
	}
	
	if (memDC.GetSafeHdc() != NULL)
	{
		CBrush tracbarBrush;
		if(m_nLBtnFlag)
		{
			tracbarBrush.CreateSolidBrush(RGB(0,255,0)) ;
		}
		else if(m_nMouseOnFlag)
		{
			tracbarBrush.CreateSolidBrush(RGB(60, 120, 166)) ;
		}
		else
		{
			tracbarBrush.CreateSolidBrush(m_crTrackBarColor);
		}
			
		memDC.FillRect(CRect(0, 0, 1, m_rectPlot.Height()+1), &tracbarBrush) ;

		dc.BitBlt(0, 0, 1, m_rectPlot.Height()+1, 
			      &memDC, 0, 0, SRCCOPY) ;
	}
	memDC.SelectObject(oldBitmap) ;
//	m_wndTrackbar.ShowWindow(SW_SHOW);
//	m_wndTrackbar.Invalidate();
#endif
}

int CGSensorView::TimeFormatText(CString *pcstr, double time)
{
	int h, m, s, d;

	h = (int)time / 3600;
	m = ((int)time % 3600) / 60;
	s = ((int)time % 60);
	d = (int)(time * 100) % 100;
	
	if(h)
		pcstr->Format(TEXT("%2d:%02d:%02d.%02d"), h, m, s, d);
	else if(m)
		pcstr->Format(TEXT("%2d:%02d.%02d"), m, s, d);
	else
		pcstr->Format(TEXT("%2d.%02d"), s, d);
	
	return pcstr->GetLength();
}

void CGSensorView::DrawAxisXText(double min, double max, double trackbar)
{
#if DEF_DISPLAY_TIME_INFO	
	CFont *oldFont ;
	CString strTemp ;
	int nYMaxLength = 0;
	DWORD max_time = (DWORD)(max * 1000);
//	WCHAR formatText[10];

	int oldTextAlign;
	oldTextAlign = m_dcGrid.GetTextAlign();
	
//	if(m_nXDecimals == 0)
//	{
//		wsprintf( formatText, TEXT("%dlf "));
//	}
//	else
//		wsprintf( formatText, TEXT("%%.%dlf "), m_nXDecimals);

	do{
		nYMaxLength++;
		max_time /= 10;
	}while(max_time);

	if(nYMaxLength < 3)
		nYMaxLength = 3;
		
	m_dcGrid.SetTextColor (m_crGridTextColor) ;
	oldFont = m_dcGrid.SelectObject(&m_fontAxisX) ;

	m_dcGrid.FillRect(CRect(m_rectPlot.left + 2, m_rectPlot.bottom+2, m_rectPlot.right + 2, m_rectPlot.bottom + 1 + m_nAxisXFontSixe), &m_brushBack) ;

	// x min
	m_dcGrid.SetTextAlign(oldTextAlign | TA_LEFT) ;
	nYMaxLength =TimeFormatText(&strTemp, min);
	//strTemp.Format (formatText, min) ;
	//nYMaxLength = strTemp.GetLength() ;
	m_dcGrid.ExtTextOut(m_rectPlot.left+2,m_rectPlot.bottom+2,ETO_CLIPPED,NULL,strTemp,nYMaxLength,NULL) ;

	// x max
	m_dcGrid.SetTextAlign(oldTextAlign | TA_RIGHT) ;
	nYMaxLength = TimeFormatText(&strTemp, max);
	//strTemp.Format (formatText, max) ;
	//nYMaxLength = strTemp.GetLength() ;
	m_dcGrid.ExtTextOut(m_rectPlot.right-2,m_rectPlot.bottom+2,ETO_CLIPPED,NULL,strTemp,nYMaxLength,NULL) ;

	// x trackbar
	if( trackbar != NULL)
	{
		m_dcGrid.SetTextAlign(oldTextAlign | TA_CENTER) ;
		nYMaxLength = TimeFormatText(&strTemp, trackbar);
		//strTemp.Format (formatText, trackbar) ;
		//nYMaxLength = strTemp.GetLength() ;
		m_dcGrid.ExtTextOut(m_rectPlot.left+m_nTrackbarPointXFactor,m_rectPlot.bottom+2,ETO_CLIPPED,NULL,strTemp,nYMaxLength,NULL) ;
	}

	m_dcGrid.SetTextAlign(oldTextAlign);
	m_dcGrid.SelectObject(oldFont) ;
#endif	
}

void CGSensorView::DrawGValueText(PGVALUE pG)
{
	CFont *oldFont ;
	CString strTemp ;
	int nYMaxLength = 5;

	int oldTextAlign;
	oldTextAlign = m_dcGrid.GetTextAlign();
	m_dcGrid.SetTextAlign(oldTextAlign | TA_CENTER) ;
	
	if(m_nXDecimals == 0)
	{
		nYMaxLength = 3;
	}

	#define FORMATTEXT(a) (a>=0 ? _T(" %.*lf") : _T("%.*lf"))
	
	oldFont = m_dcGrid.SelectObject(&m_fontGValue) ;

	m_dcGrid.FillRect(CRect(m_rectClient.left, m_rectPlot.top, m_rectPlot.left - 23, m_rectPlot.bottom), &m_brushBack) ;
	
	int interval = m_rectPlot.Height()/4;
	int leftPoint = m_rectClient.left + 15;
	// x
	m_dcGrid.SetTextColor (m_crPlotColor[0]) ;
	strTemp.Format (FORMATTEXT(pG->GX), m_nGDecimals, pG->GX) ;
	//nYMaxLength = strTemp.GetLength() ;
	m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top,ETO_CLIPPED,NULL,_T("X"),1,NULL) ;
	m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + m_nGValueFontSize,ETO_CLIPPED,NULL,strTemp,nYMaxLength,NULL) ;

	// y
	m_dcGrid.SetTextColor (m_crPlotColor[1]) ;
	strTemp.Format (FORMATTEXT(pG->GY), m_nGDecimals, pG->GY) ;
	//nYMaxLength = strTemp.GetLength() ;
	m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + (interval),ETO_CLIPPED,NULL,_T("Y"),1,NULL) ;
	m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + (interval) + m_nGValueFontSize,ETO_CLIPPED,NULL,strTemp,nYMaxLength,NULL) ;

	// z 
	m_dcGrid.SetTextColor (m_crPlotColor[2]) ;
	strTemp.Format (FORMATTEXT(pG->GZ), m_nGDecimals, pG->GZ) ;
	//nYMaxLength = strTemp.GetLength() ;
	m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + (interval * 2),ETO_CLIPPED,NULL,_T("Z"),1,NULL) ;
	m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + (interval * 2) + m_nGValueFontSize,ETO_CLIPPED,NULL,strTemp,nYMaxLength,NULL) ;

	// s 
	if(m_crPlotColor[3] != 0xffffff){
		m_dcGrid.SetTextColor (m_crPlotColor[3]) ;
		strTemp.Format (_T("%d"), (int)(pG->speed * m_dSpeedFactor)) ;
		m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + (interval * 3),ETO_CLIPPED,NULL,_T("S"),1,NULL) ;
		m_dcGrid.ExtTextOut(leftPoint,m_rectPlot.top + (interval * 3) + m_nGValueFontSize,ETO_CLIPPED,NULL,strTemp,strTemp.GetLength(),NULL) ;
	}
	m_dcGrid.SetTextAlign(oldTextAlign);
	m_dcGrid.SelectObject(oldFont) ;
}


/************************************************************************************
Function Name	:	SetTrackBarPosition

Class Name		:	CGSensorView

Description		:	It sets the g graph start display position

Argument		:	int position

Return			:	void
*************************************************************************************/
void CGSensorView::SetTrackBarPosition(int position)
{
    m_nTrackbarPosition = position;
    // clear out the existing garbage, re-start with a clean plot
    InvalidateCtrl() ;

}
/************************************************************************************
Function Name	:	SetRange

Class Name		:	CGSensorView

Description		:	It sets the minimum, maximum and decimal ratios to the Y Axis

Argument		:	double dLower, double dUpper, int nDecimalPlaces, double gridInterval

Return			:	void
*************************************************************************************/
void CGSensorView::SetRange(double dLower, double dUpper, int nDecimalPlaces, double gridInterval, int speed_type, double dSpeedRange)
{
    ASSERT(dUpper > dLower) ;

    m_dLowerLimit     = dLower ;
	m_dUpperLimit     = dUpper ;
	m_nYDecimals      = nDecimalPlaces ;
	m_dRange          = m_dUpperLimit - m_dLowerLimit ;
	m_dGridInterval 	= gridInterval;
    // clear out the existing garbage, re-start with a clean plot

	if(dSpeedRange)
		SetSpeedRange(speed_type, dSpeedRange);
	
   	InvalidateCtrl() ;
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange ; 
	
	if(m_nLastIndexPoint)
	{
		int refreshPoint = m_nLastRefreshPoint;
		RefreshDraw(0);
		RefreshDraw(refreshPoint);
	}
}
/************************************************************************************
Function Name	:	SetXUnits

Class Name		:	CGSensorView

Description		:	It sets the  X axis unit as "Time in second"

Argument		:	CString string

Return			:	void
*************************************************************************************/
void CGSensorView::SetXUnits(CString string)
{
  m_strXUnitsString = string ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetXUnits

/************************************************************************************
Function Name	:	SetYUnits

Class Name		:	CGSensorView

Description		:	It sets the  Y axis unit as "Volume in ml"

Argument		:	CString string

Return			:	void
*************************************************************************************/
void CGSensorView::SetYUnits(CString string)
{
  m_strYUnitsString = string ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}
/************************************************************************************
Function Name	:	SetGridColor

Class Name		:	CGSensorView

Description		:	It sets the Grid or X and Y axis colors

Argument		:	COLORREF color, COLORREF textColor

Return			:	void
*************************************************************************************/
void CGSensorView::SetGridColor(COLORREF color, COLORREF textColor)
{
  m_crGridColor = color ;

  if(textColor == NULL)
	m_crGridTextColor = color ;
  else
  	m_crGridTextColor = textColor;
  
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetGridColor

/************************************************************************************
Function Name	:	SetTrackBarColor

Class Name		:	CGSensorView

Description		:	It sets the track bar colors

Argument		:	COLORREF color

Return			:	void
*************************************************************************************/
void CGSensorView::SetTrackBarColor(COLORREF color)
{
  m_crTrackBarColor = color ;
  m_tracbarBrush.DeleteObject() ;
  m_tracbarBrush.CreateSolidBrush(m_crTrackBarColor) ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetTrackBarColor
/************************************************************************************
Function Name	:	SetGridScaleX

Class Name		:	CGSensorView

Description		:	It sets the Grid  X scale(unit: pixel)

Argument		:	COLORREF color

Return			:	void
*************************************************************************************/
void CGSensorView::SetGridScaleX(double dScaleX)
{
	if( m_dScaleX == dScaleX || dScaleX <= 0.0)
		return;
	
  	m_dScaleX = dScaleX ;
  	// clear out the existing garbage, re-start with a clean plot
  	InvalidateCtrl() ;

  	// calculation x point
	if(m_nLastIndexPoint > 0)
	{
		int refreshPoint = m_nLastRefreshPoint;
		RefreshDraw(0);
		RefreshDraw(refreshPoint);
	}
}  // SetGridScaleX

/************************************************************************************
Function Name	:	SetPlotColor

Class Name		:	CGSensorView

Description		:	It sets the drawing color of the curve

Argument		:	COLORREF colorX,Y,X

Return			:	void
*************************************************************************************/
void CGSensorView::SetPlotColor(COLORREF colorX, COLORREF colorY, COLORREF colorZ, COLORREF colorS)
{
  int i;
  m_crPlotColor[0] = colorX ;
  m_crPlotColor[1] = colorY ;
  m_crPlotColor[2] = colorZ ;
  m_crPlotColor[3] = colorS ;
  for( i = 0; i < 4; i++ )
  {
	  m_penPlot[i].DeleteObject() ;
	  m_penPlot[i].CreatePen(PS_SOLID, 1, m_crPlotColor[i]) ;
  }
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetPlotColor

/************************************************************************************
Function Name	:	SetEventColor

Class Name		:	CGSensorView

Description		:	It sets the background color of the window for Event Type Display

Argument		:	COLORREF* pcolor, int cnt

Return			:	void
*************************************************************************************/
void CGSensorView::SetEventColor(COLORREF* pColor, int cnt)
{
	int i;
	for( i = 0; i < cnt ; i++)
	{
		m_crEventColor[i] = pColor[i];
	}
	m_brushEvent.DeleteObject() ;
  	m_brushEvent.CreateSolidBrush(m_crEventColor[0]) ;
  
 	// clear out the existing garbage, re-start with a clean plot
  	InvalidateCtrl() ;
}  // SetEventColor


/************************************************************************************
Function Name	:	SetBackgroundColor

Class Name		:	CGSensorView

Description		:	It sets the background color of the window

Argument		:	COLORREF color

Return			:	void
*************************************************************************************/
void CGSensorView::SetBackgroundColor(COLORREF color)
{
  m_crBackColor = color ;
  m_brushBack.DeleteObject() ;
  m_brushBack.CreateSolidBrush(m_crBackColor) ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetBackgroundColor


/************************************************************************************
Function Name	:	SetXRange

Class Name		:	CGSensorView

Description		:	It sets the X axis description values

Argument		:	double dLower, double dUpper, int nDecimalPlaces

Return			:	void
*************************************************************************************/
void CGSensorView::SetXRange(double dLower, double dUpper, int nDecimalPlaces)
{
	ASSERT(dUpper > dLower) ;
	m_dXLower     = dLower ;
	m_dXUpper     = dUpper ;
	m_nXDecimals  = nDecimalPlaces ;
	m_XRange          = m_dXUpper - m_dXLower ;
	//m_nTrackbarPosition = (int)m_XRange;
	m_dHorizontalFactor = (double)m_nPlotWidth / m_XRange ; 
    // clear out the existing garbage, re-start with a clean plot

	InvalidateCtrl() ;
}

void CGSensorView::SetSpeedRange(int speed_type, double dSpeedRange)
{
	switch(speed_type)
	{

		case SPEED_KM_P_H :
			m_dSpeedFactor = 1.853184; 	//Knots = 1.853184 km/h

			if(dSpeedRange == 0)
				m_dSpeedRange = 180;			
			break;
		case SPEED_MILE_P_H :
			m_dSpeedFactor = 1.150779;	// 1 Knots = 1.150779 mile/h

			if(dSpeedRange == 0)
				m_dSpeedRange = 120;
			break;
		case SPEED_KNOT :
			m_dSpeedFactor = 1.0;

			if(dSpeedRange == 0)
				m_dSpeedRange = 100;
			break;

		default :
			m_dSpeedFactor = 1.853184; 	//Knots = 1.853184 km/h

			if(dSpeedRange == 0)
				m_dSpeedRange = 180;		
			break;
	}

	SetSpeedRange(dSpeedRange);
}

void CGSensorView::SetSpeedRange(double dSpeedRange)
{
	if(dSpeedRange != 0.0)
		m_dSpeedRange = dSpeedRange;
	
	 InvalidateCtrl() ;
	 m_dSpeedVerticalFactor =  (double)m_nPlotHeight / m_dSpeedRange;
 
	if(m_nLastIndexPoint)
	{
		int refreshPoint = m_nLastRefreshPoint;
		RefreshDraw(0);
		RefreshDraw(refreshPoint);
	}
}


void CGSensorView::SendSeekMsg(int nSeekPoint, int pauseFlay)
{
	if(m_SeekMessageID > 0)
		GetParent()->SendMessage( m_SeekMessageID, nSeekPoint, pauseFlay);
}

BOOL CGSensorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	double	d_signed;
	CPoint point = pt;
	
	ScreenToClient(&point);
	
	if(zDelta < 0)
		d_signed = 1.0;
	else
		d_signed = -1.0;
	
	////////////
	if(m_rectRangeG.PtInRect(point))
	{
		double value = m_dUpperLimit + (0.5 * d_signed);
		
		if(value <= 2.5 && value >= 0.5) 
			SetRange(-value, value, m_nYDecimals, m_dGridInterval);
	}
	else if(m_rectRangeSpeed.PtInRect(point))
	{
		double value = m_dSpeedRange + (10.0 * d_signed);
		
		if(value <= 300.0 && value >= 50.0)
			SetSpeedRange(value);
	}

	else if(m_rectRangeScale.PtInRect(point))
	{
#if DEF_DISPLAY_TRACKBAR	
		double value = m_dScaleX + (0.5 * (-d_signed));
		
		if(value <= 10.0 && value >= 0.5)
			SetGridScaleX(value);
#endif		
	}

	return TRUE;
	//return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CGSensorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_nLBtnFlag)
	{
		if(m_rectPlot.PtInRect(point))
		{
			if(m_nLBtnDnIndex >= 0)
			{
				m_nLBtnMoveOffset = (int)((m_cpLBtnDnPoint.x - point.x) / m_dScaleX);
			}
		}
	}

	HWND hParent;

	if (GetCapture()!= this) {
		for (hParent=this->m_hWnd;::GetParent(hParent);hParent=::GetParent(hParent)) {;}
		
		SetTimer(TIMER_MOUSE_ON_CHECK,50,NULL);
		m_nMouseOnFlag = TRUE;
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CGSensorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_nLBtnFlag = nFlags;
	SetTimer(TIMER_LBUTTON_DOWN, 200, NULL);
	if(m_rectPlot.PtInRect(point))
	{
		m_nLBtnDnIndex = m_nLastRefreshPoint;

		m_cpLBtnDnPoint.x = point.x;
		m_cpLBtnDnPoint.y = point.y;

		m_nLBtnMoveOffset = 0;

		SendSeekMsg( -1, 1);
	}
	else
		m_nLBtnDnIndex = -1;

	
	CWnd::OnLButtonDown(nFlags, point);
}

void CGSensorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_nLBtnFlag = nFlags;

	if(m_rectPlot.PtInRect(point) && m_nLBtnDnIndex >= 0)
	{
		if(m_nLBtnMoveOffset == 0)
		{
			m_nLBtnMoveOffset = (int)((point.x - (m_rectPlot.left + m_nTrackbarPointXCurrent)) / m_dScaleX);

			if(m_nLBtnDnIndex + m_nLBtnMoveOffset < 0)
				SendSeekMsg( 0, 0);
			else if(m_nLBtnDnIndex + m_nLBtnMoveOffset < m_nLastIndexPoint)
				SendSeekMsg((int)m_oGValue[m_nLBtnDnIndex + m_nLBtnMoveOffset].time, 0);		
		}
		else
			SendSeekMsg(-1, 0);	
	}
	m_nLBtnDnIndex = -1;

	
	KillTimer(TIMER_LBUTTON_DOWN);
	CWnd::OnLButtonUp(nFlags, point);
}

void CGSensorView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CGSensorView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	switch ( nIDEvent )
	{
	case TIMER_LBUTTON_DOWN:
		{
			static int beforeMoveOffset = 0;
			SetTimer(TIMER_LBUTTON_DOWN, 100, NULL);

			if(beforeMoveOffset != m_nLBtnMoveOffset && m_nLBtnDnIndex >= 0)
			{
				beforeMoveOffset = m_nLBtnMoveOffset;
				if(m_nLBtnDnIndex + m_nLBtnMoveOffset > 0 && m_nLBtnDnIndex + m_nLBtnMoveOffset < m_nLastIndexPoint)
					SendSeekMsg((int)m_oGValue[m_nLBtnDnIndex + m_nLBtnMoveOffset].time, 1);		
			}
		}
		break;
	case TIMER_MOUSE_ON_CHECK:
		{
			POINT pt;
			CRect crt;

			GetCursorPos(&pt);
			GetWindowRect(&crt);
			if (crt.PtInRect(pt)==FALSE) 
			{
				KillTimer(TIMER_MOUSE_ON_CHECK);
				m_nMouseOnFlag = FALSE;
				m_nLBtnDnIndex = -1;

				if(m_nLBtnFlag)
					SendSeekMsg(-1, 0);	
			}
		}
		break;
	default :
			break;
	}
	CWnd::OnTimer(nIDEvent);
}


void	CGSensorView::Synchronize( DWORD dwCTS )
{
	int nRefreshPoint = this->m_nLastRefreshPoint;
	
	if(dwCTS < 2000)
		dwCTS /= 2;
	else
		dwCTS -= 500;
	
	if(this->m_dwCTS == dwCTS)
		return;
	
	if(this->m_dwCTS  < dwCTS)
	{
		for( ; nRefreshPoint < this->m_nLastIndexPoint; nRefreshPoint++)
		{
			if(this->m_oGValue[nRefreshPoint].time >= dwCTS)
			{
				break;
			}
		}
	}
	else
	{	
		for( ; nRefreshPoint >= 0; nRefreshPoint--)
		{
			if(this->m_oGValue[nRefreshPoint].time <= dwCTS)
			{
				break;
			}
		}
	}

	if(nRefreshPoint < 0)
		nRefreshPoint = 0;
	else if(nRefreshPoint >= this->m_nLastIndexPoint)
		nRefreshPoint = (this->m_nLastIndexPoint - 1);
	
//	if(this->m_oGValue[nRefreshPoint].GX != 0.0 || this->m_oGValue[nRefreshPoint].GY != 0.0 || this->m_oGValue[nRefreshPoint].GZ != 0.0)
		this->RefreshDraw(nRefreshPoint);
	
	//this->m_dwCTS = this->m_oGValue[nRefreshPoint].time;
        this->m_dwCTS = dwCTS;
}

BOOL CGSensorView::SetSensorData( CTextData* pSensorData )
{
	int k=0, count=0;
	DWORD last_cts=0;
	if( !pSensorData ) return FALSE;

	this->ErasePoint();
	this->m_pSensorData = pSensorData;

	m_bRedrawBlocked = TRUE;

	CTextData::ITER_GSENSOR iGSensor = this->m_pSensorData->m_GSensorList.begin();
	CTextData::ITER_GSENSOR iGSensorend = this->m_pSensorData->m_GSensorList.end();
	for( ; iGSensor != iGSensorend; iGSensor++ )
	{
		//if( iGSensor->m_cts <= this->m_dXUpper*1000)
		{
			this->AppendPoint(iGSensor->m_gseonsorX, iGSensor->m_gseonsorY, iGSensor->m_gseonsorZ, iGSensor->m_speed, 0, last_cts = iGSensor->m_cts);
			count++;
			if(count >= m_nGValueSize)
				break;
		}
	}
	
	//dumy
	for(k = count; k <count + 300; k++ )
	{
		this->AppendPoint((double)0.0, (double)0.0, (double)0.0,(double)0.0, (int)0, last_cts += 100);
	}

	m_bRedrawBlocked = FALSE;
	this->RefreshDraw(1);
	
	return TRUE;
}

BOOL CGSensorView::GetCurrentSensorData (PGVALUE pGValue)
{
	if(pGValue && m_oGValue) {
		*pGValue = m_oGValue[m_nLastRefreshPoint];
		return TRUE;
	}
	
	return FALSE;
}

