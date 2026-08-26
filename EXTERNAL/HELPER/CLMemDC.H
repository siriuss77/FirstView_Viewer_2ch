//=================================================================================================
// This class implements a memory Device Context which allows
// flicker free drawing.
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-2002, Keith Rule
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. CLASS

class ClMemDC : public CDC
{
public:
	
	ClMemDC( CDC* pDC, const CRect* pRect = NULL, BOOL bIsBackground = FALSE ) : CDC()
	{
		ASSERT(pDC != NULL); 

		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		if( pRect == NULL )	pDC->GetClipBox( &m_rect );
		else				m_rect = *pRect;

		if( m_bMemDC)
		{
			CreateCompatibleDC(pDC);
			
			if(m_hAttribDC == NULL)
			{
				HWND hWndViewer;
			
				hWndViewer = ::FindWindow(NULL, L"Blackbox Viewer");
				if(hWndViewer)
					::PostMessage(hWndViewer,WM_USER + 0x102, 0, 0); //CM_MAIN_REDRAW
				return;
			}
			
			pDC->LPtoDP(&m_rect);

			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);

			SetMapMode(pDC->GetMapMode());

			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());

			pDC->DPtoLP(&m_rect);
			SetWindowOrg(m_rect.left, m_rect.top);

		} else
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}

		if( bIsBackground )
		{
			BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				m_pDC, m_rect.left, m_rect.top, SRCCOPY);
		}
		else
		{
			FillSolidRect(m_rect, pDC->GetBkColor());
		}
	}
	
	~ClMemDC()	
	{		
		if (m_bMemDC)
		{
			if(m_hAttribDC)
				m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					this, m_rect.left, m_rect.top, SRCCOPY);
			
			if(m_oldBitmap)
				SelectObject(m_oldBitmap);
		}
		else
		{
			m_hDC = m_hAttribDC = NULL;
		}	
	}
	
	ClMemDC* operator->() 
	{
		return this;
	}	

	operator ClMemDC*() 
	{
		return this;
	}

private:

		CBitmap						m_bitmap;
		CBitmap*					m_oldBitmap;
		CDC*						m_pDC;
		CRect						m_rect;
		BOOL						m_bMemDC;

};

//================================================================================================= E. CLASS
