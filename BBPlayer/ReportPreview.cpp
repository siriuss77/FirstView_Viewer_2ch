#include "StdAfx.h"
#include "ReportPreview.h"

#include <CLUDE/version.h>

void CReportPreview::GetPrintOutDC(CDC* pDC, RECT* pRc,	PREPORT_ITEM pReportItem, PRINTINFO* pPrintInfo)
{
	// Print Dialog
	CPrintDialog dlg(FALSE);

	// Document Location
	HDC hdcPrinter;
	CDC dcPrinter;
	int nLogPixelsX;
	int nLogPixelsY;
	int nHorzRes;
	int nVertRes;
	if(!dlg.GetDefaults())
	{
		nLogPixelsX = 600;
		nLogPixelsY = 600;
		nHorzRes = 4672;
		nVertRes = 6774;
	}
	else
	{


	
/*		LPDEVMODE pDevMode = (LPDEVMODE) ::GlobalLock(dlg.m_pd.hDevMode); 
		if (NULL == pDevMode) 
		{ 
		  TRACE0 ("failed to GlobalLock DEVMODE\\n"); 
		  return; 
		} 

		// 현재 장치값을 다이얼로그에 전달    
		pDevMode->dmOrientation = 1; // 1:Portrait, 2:Landscape 
		dlg.m_pd.hDevMode = pDevMode; 
		AfxGetApp()->SelectPrinter(dlg.m_pd.hDevNames, dlg.m_pd.hDevMode); 
		          
		::GlobalUnlock(dlg.m_pd.hDevMode); //프린터 정보 초기화 끝 
*/


		hdcPrinter = dlg.GetPrinterDC();
		dcPrinter.Attach(hdcPrinter);
		nLogPixelsX = dcPrinter.GetDeviceCaps(LOGPIXELSX); 
		nLogPixelsY = dcPrinter.GetDeviceCaps(LOGPIXELSY); 
		nHorzRes = dcPrinter.GetDeviceCaps(HORZRES); 
		nVertRes = dcPrinter.GetDeviceCaps(VERTRES); 
	}
	int nScaleX		= (nHorzRes - nLogPixelsX * 2) / 4;
	int nScaleY		= (nVertRes - nLogPixelsY * 2) / 3;
	int nPrintWidth, nPrintHeight, nPrintLeft, nPrintTop;

	nPrintWidth		= (nHorzRes - nLogPixelsX);
	nPrintHeight	= (nVertRes - nLogPixelsY);
	nPrintLeft		= (nHorzRes - nPrintWidth) / 2;
	nPrintTop		= (nVertRes - nPrintHeight) / 2;

	int nW = pRc->right - pRc->left;
	int nH = pRc->bottom - pRc->top;

	int nLine = 40;
	int nFontHeight = 18;

	int nPaperWidth, nPaperHeight;
	if(nHorzRes < nVertRes)
	{
		nPaperHeight = nH - 20;
		nPaperWidth = (int)(((float)nHorzRes / (float)nVertRes) * (float)nPaperHeight);
	}
	else
	{
		nPaperWidth = nW - 20;
		nPaperHeight = (int)(((float)nVertRes / (float)nHorzRes) * (float)nPaperWidth);
	}

	// DRAW BACKGROUND
	pDC->SetROP2(R2_COPYPEN);
	pDC->SelectObject(GetStockObject(GRAY_BRUSH));
	pDC->Rectangle(pRc);

	// DRAW PAPER
	RECT rcPaper;
	rcPaper.top		= ((nH - nPaperHeight) / 2) + 2 + pRc->top;
	rcPaper.left	= ((nW - nPaperWidth) / 2) + 2 + pRc->left;
	rcPaper.bottom	= (rcPaper.top + nPaperHeight) + 2;
	rcPaper.right	= (rcPaper.left + nPaperWidth) + 2;
	pDC->SetROP2(R2_BLACK);
	pDC->Rectangle(&rcPaper);
	rcPaper.top		= ((nH - nPaperHeight) / 2) + pRc->top;
	rcPaper.left	= ((nW - nPaperWidth)/ 2) + pRc->left;
	rcPaper.bottom	= rcPaper.top + nPaperHeight;
	rcPaper.right	= rcPaper.left + nPaperWidth;
	pDC->SetROP2(R2_WHITE);
	pDC->Rectangle(&rcPaper);

	float nRatio = (float)nHorzRes / (float)nPaperWidth;
	nPrintTop	= (int)((float)nPrintTop / nRatio);
	nPrintLeft	= (int)((float)nPrintLeft / nRatio);
	nPrintWidth	= (int)((float)nPrintWidth / nRatio);
	nPrintHeight= (int)((float)nPrintHeight / nRatio);
	nLogPixelsY = (int)((float)nLogPixelsY / nRatio);

	pDC->SetMapMode(MM_ANISOTROPIC);

	nPrintTop	+= rcPaper.top;
	nPrintLeft	+= rcPaper.left;

	int nGridX = nPrintWidth / 17;
	int nGridY = nPrintHeight / nLine;
	nFontHeight = nPrintHeight / 26;
	int nGap = nFontHeight/3;

	// TEXT ----------------------------------------------------------------------------
	CFont Font, *pFontOld;
	Font.CreateFont(nFontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Tahoma"));
	pFontOld = pDC->SelectObject(&Font);
	//1. title
	pDC->TextOut(nPrintLeft, nPrintTop, pPrintInfo->tzTitle, _tcslen(pPrintInfo->tzTitle));
	pDC->SelectObject(pFontOld);
	Font.DeleteObject();

	CFont Font1, *pFontOld1;
	Font1.CreateFont(nFontHeight / 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Tahoma"));
	pFontOld1 = pDC->SelectObject(&Font1);

	//2. date
	pDC->TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-13) * nGridY)-nGap*3,	   pPrintInfo->tzLine1,  _tcslen(pPrintInfo->tzLine1));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-12) * nGridY)-nGap*3, pPrintInfo->tzLine1C, _tcslen(pPrintInfo->tzLine1C));

	//3. GPS
	pDC->TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-11) * nGridY)-nGap*2,	   pPrintInfo->tzLine2,  _tcslen(pPrintInfo->tzLine2));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-10) * nGridY)-nGap*2, pPrintInfo->tzLine2C1, _tcslen(pPrintInfo->tzLine2C1));
	pDC->TextOut(nPrintLeft + (4 * nGridX) + 13, nPrintTop + ((nLine-10) * nGridY)-nGap*2, pPrintInfo->tzLine2C2, _tcslen(pPrintInfo->tzLine2C2));
	pDC->TextOut(nPrintLeft + (8 * nGridX) + 13, nPrintTop + ((nLine-10) * nGridY)-nGap*2, pPrintInfo->tzLine2C3, _tcslen(pPrintInfo->tzLine2C3));

	//4. G-Sensor
	pDC->TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-9) * nGridY)-nGap,	   pPrintInfo->tzLine3,  _tcslen(pPrintInfo->tzLine3));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-8) * nGridY)-nGap, pPrintInfo->tzLine3C, _tcslen(pPrintInfo->tzLine3C));

	//5. 
	pDC->TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-7) * nGridY),		   pPrintInfo->tzLine4, _tcslen(pPrintInfo->tzLine4));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-6) * nGridY),	   pPrintInfo->tzMemo1, _tcslen(pPrintInfo->tzMemo1));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-5) * nGridY),	   pPrintInfo->tzMemo2, _tcslen(pPrintInfo->tzMemo2));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-4) * nGridY),	   pPrintInfo->tzMemo3, _tcslen(pPrintInfo->tzMemo3));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-3) * nGridY),	   pPrintInfo->tzMemo4, _tcslen(pPrintInfo->tzMemo4));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-2) * nGridY),	   pPrintInfo->tzMemo5, _tcslen(pPrintInfo->tzMemo5));
	pDC->TextOut(nPrintLeft + (1 * nGridX) + 8, nPrintTop + ((nLine-1) * nGridY),	   pPrintInfo->tzMemo6, _tcslen(pPrintInfo->tzMemo6));

	pDC->TextOut(nPrintLeft + (nGridX * 7), nPrintTop + ((nLine-0) * nGridY),	   pPrintInfo->tzBottom, _tcslen(pPrintInfo->tzBottom));
	pDC->SelectObject(pFontOld1);
	Font1.DeleteObject(); 

	// LINE ----------------------------------------------------------------------------
	pDC->SetROP2(R2_BLACK);
	//pDC->MoveTo(nPrintLeft, nPrintTop + nGridY);
	//pDC->LineTo(nPrintLeft + (17 * nGridX), nPrintTop + nGridY);
	pDC->MoveTo(nPrintLeft, nPrintTop + nFontHeight);
	pDC->LineTo(nPrintLeft + (17 * nGridX), nPrintTop + nFontHeight);


	pDC->MoveTo(nPrintLeft, nPrintTop + ((nLine-0) * nGridY));
	pDC->LineTo(nPrintLeft + (17 * nGridX), nPrintTop + ((nLine-0) * nGridY));

	// IMAGE ----------------------------------------------------------------------------
	SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);

	int nX[MAX_CHANNEL], nY[MAX_CHANNEL];
	int nCount = 0;
	int xoffset = 0, yoffset = 0;

	nX[0] = nGridX*16;
	nY[0] = nX[0]*9/16;

	nX[1] = nGridX*8;
	nY[1] = nX[1]*2/3;

	int x = nPrintLeft + (1 * nGridX/2);
	int y = nPrintTop + (2 * nGridY);
//	pDC->Rectangle(x, y, x + nX[nCount], y + nY[nCount]);

#ifdef REAR_CAM_OFF      // jun 170406

         if(pReportItem->cpStreamImage[nCount])
		{
			//xoffset = (nGridX*16 - nX[nCount]) / 2;
			pReportItem->cpStreamImage[nCount]->Draw(pDC->GetSafeHdc(), x + xoffset, y + yoffset, nX[nCount], nY[nCount]);
			yoffset = nY[nCount] + 3;
		}
	
	if(pReportItem->cpMapImage){
		xoffset = (nGridX*16 - nX[1]);
		yoffset = nY[0] + 3;
		pReportItem->cpMapImage->Draw(pDC->GetSafeHdc(), x + xoffset, y + yoffset, nX[1], nY[1]);
	}
#else
	for(nCount=0; nCount < pReportItem->streamCount; nCount++){
		if(pReportItem->cpStreamImage[nCount])
		{
			//xoffset = (nGridX*16 - nX[nCount]) / 2;
			pReportItem->cpStreamImage[nCount]->Draw(pDC->GetSafeHdc(), x + xoffset, y + yoffset, nX[nCount], nY[nCount]);
			yoffset = nY[nCount] + 3;
		}
	}
	if(pReportItem->cpMapImage){
		xoffset = (nGridX*16 - nX[1]);
		yoffset = nY[0] + 3;
		pReportItem->cpMapImage->Draw(pDC->GetSafeHdc(), x + xoffset, y + yoffset, nX[1], nY[1]);
	}
#endif

}

void CReportPreview::PrintOut(PREPORT_ITEM pReportItem, PRINTINFO*	pPrintInfo)
{	
	// Print Dialog
	CPrintDialog dlg(FALSE);
	if (dlg.DoModal() != IDOK) return;

	int nMaxChannel = 1;

	// Document Location
	HDC hdcPrinter = dlg.GetPrinterDC();
	CDC dcPrinter;
	dcPrinter.Attach(hdcPrinter);
	int nLogPixelsX	= dcPrinter.GetDeviceCaps(LOGPIXELSX);		// pixel per inch
	int nLogPixelsY = dcPrinter.GetDeviceCaps(LOGPIXELSY);		// pixel per inch
	int nHorzRes	= dcPrinter.GetDeviceCaps(HORZRES);			// millimeters		
	int nVertRes	= dcPrinter.GetDeviceCaps(VERTRES);			// millimeters
	int nScaleX		= (nHorzRes - nLogPixelsX * 2) / 4;
	int nScaleY		= (nVertRes - nLogPixelsY * 2) / 3;
	int nPrintWidth, nPrintHeight, nPrintLeft, nPrintTop;

	nPrintWidth		= (nHorzRes - nLogPixelsX);
	nPrintHeight	= (nVertRes - nLogPixelsY);
	nPrintLeft		= (nHorzRes - nPrintWidth) / 2;
	nPrintTop		= (nVertRes - nPrintHeight) / 2;

	DOCINFO docinfo;
	memset(&docinfo, 0, sizeof(DOCINFO));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = _T("Dvr Print");

	// Print
	if (dcPrinter.StartDoc(&docinfo) >= 0)
	{
		if (dcPrinter.StartPage() >= 0)
		{
			int nLine = 40;
			int nGridX = nPrintWidth / 17;
			int nGridY = nPrintHeight / nLine;
			int nFontHeight = nPrintHeight / 26;
			int nGap = nFontHeight/3;

			// TEXT ----------------------------------------------------------------------------
			CFont Font, *pFontOld;
			Font.CreateFont(nFontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Tahoma"));
			pFontOld = dcPrinter.SelectObject(&Font);
			dcPrinter.TextOut(nPrintLeft, nPrintTop, pPrintInfo->tzTitle, _tcslen(pPrintInfo->tzTitle));
			dcPrinter.SelectObject(pFontOld);
			Font.DeleteObject();

			CFont Font1, *pFontOld1;
			Font1.CreateFont(nFontHeight / 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Tahoma"));
			pFontOld1 = dcPrinter.SelectObject(&Font1);

			dcPrinter.TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-13) * nGridY)-nGap*3,	   pPrintInfo->tzLine1,  _tcslen(pPrintInfo->tzLine1));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-12) * nGridY)-nGap*3, pPrintInfo->tzLine1C, _tcslen(pPrintInfo->tzLine1C));

			dcPrinter.TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-11) * nGridY)-nGap*2,	   pPrintInfo->tzLine2,  _tcslen(pPrintInfo->tzLine2));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-10) * nGridY)-nGap*2, pPrintInfo->tzLine2C1, _tcslen(pPrintInfo->tzLine2C1));
			dcPrinter.TextOut(nPrintLeft + (6 * nGridX), nPrintTop + ((nLine-10) * nGridY)-nGap*2, pPrintInfo->tzLine2C2, _tcslen(pPrintInfo->tzLine2C2));
			dcPrinter.TextOut(nPrintLeft + (10 * nGridX), nPrintTop + ((nLine-10) * nGridY)-nGap*2, pPrintInfo->tzLine2C3, _tcslen(pPrintInfo->tzLine2C3));

			dcPrinter.TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-9) * nGridY)-nGap,	   pPrintInfo->tzLine3,  _tcslen(pPrintInfo->tzLine3));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX) + 8, nPrintTop + ((nLine-8) * nGridY)-nGap, pPrintInfo->tzLine3C, _tcslen(pPrintInfo->tzLine3C));

			dcPrinter.TextOut(nPrintLeft + (1 * nGridX), nPrintTop + ((nLine-7) * nGridY),		   pPrintInfo->tzLine4, _tcslen(pPrintInfo->tzLine4));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-6) * nGridY),	   pPrintInfo->tzMemo1, _tcslen(pPrintInfo->tzMemo1));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-5) * nGridY),	   pPrintInfo->tzMemo2, _tcslen(pPrintInfo->tzMemo2));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-4) * nGridY),	   pPrintInfo->tzMemo3, _tcslen(pPrintInfo->tzMemo3));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-3) * nGridY),	   pPrintInfo->tzMemo4, _tcslen(pPrintInfo->tzMemo4));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-2) * nGridY),	   pPrintInfo->tzMemo5, _tcslen(pPrintInfo->tzMemo5));
			dcPrinter.TextOut(nPrintLeft + (2 * nGridX), nPrintTop + ((nLine-1) * nGridY),	   pPrintInfo->tzMemo6, _tcslen(pPrintInfo->tzMemo6));


			dcPrinter.TextOut(nPrintLeft + (nGridX * 7), nPrintTop + ((nLine-0) * nGridY), pPrintInfo->tzBottom, _tcslen(pPrintInfo->tzBottom));
			dcPrinter.SelectObject(pFontOld1);
			Font1.DeleteObject(); 

			// LINE ----------------------------------------------------------------------------
			CPen Pen, *pPenOld;
			Pen.CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
			pPenOld = dcPrinter.SelectObject(&Pen);
			dcPrinter.MoveTo(nPrintLeft, nPrintTop + nFontHeight);
			dcPrinter.LineTo(nPrintLeft + (17 * nGridX), nPrintTop + nFontHeight);

			dcPrinter.MoveTo(nPrintLeft, nPrintTop + ((nLine-0) * nGridY));
			dcPrinter.LineTo(nPrintLeft + (17 * nGridX), nPrintTop + ((nLine-0) * nGridY));
			dcPrinter.SelectObject(pPenOld);

			// IMAGE ----------------------------------------------------------------------------
			int nX[MAX_CHANNEL], nY[MAX_CHANNEL];
			int nCount = 0;

			nX[0] = nGridX*16;
			nY[0] = nX[0]*9/16;

			nX[1] = nGridX*8;
			nY[1] = nX[1]*2/3;

			int x = nPrintLeft + (1 * nGridX/2);
			int y = nPrintTop + (2 * nGridY);

			#define _DEV 6
			int xoffset = 0, yoffset = 0;
			
			
#ifdef REAR_CAM_OFF     // jun 170406

				if(pReportItem->cpStreamImage[nCount])
				{
					//xoffset = (nGridX*16 - nX[nCount]) / 2;
					pReportItem->cpStreamImage[nCount]->Draw(dcPrinter.GetSafeHdc(), x + xoffset, y + yoffset, nX[nCount], nY[nCount]);
					yoffset = nY[nCount] + 20;
				}
			

			if(pReportItem->cpMapImage){
				xoffset = (nGridX*16 - nX[1]);
				yoffset = nY[0] + 20;
				pReportItem->cpMapImage->Draw(dcPrinter.GetSafeHdc(), x + xoffset, y + yoffset, nX[1], nY[1]);
			}
#else
			for(nCount=0; nCount < pReportItem->streamCount; nCount++){

				if(pReportItem->cpStreamImage[nCount])
				{
					//xoffset = (nGridX*16 - nX[nCount]) / 2;
					pReportItem->cpStreamImage[nCount]->Draw(dcPrinter.GetSafeHdc(), x + xoffset, y + yoffset, nX[nCount], nY[nCount]);
					yoffset = nY[nCount] + 20;
				}
			}

			if(pReportItem->cpMapImage){
				xoffset = (nGridX*16 - nX[1]);
				yoffset = nY[0] + 20;
				pReportItem->cpMapImage->Draw(dcPrinter.GetSafeHdc(), x + xoffset, y + yoffset, nX[1], nY[1]);
			}
#endif
					
			dcPrinter.EndPage();
		}
		else
		{
			dcPrinter.AbortDoc();
		}
		dcPrinter.EndDoc();
	}	
}
