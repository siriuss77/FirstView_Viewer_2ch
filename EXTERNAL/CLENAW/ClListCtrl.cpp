//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClListCtrl.CPP
//
//	Description: CLENA-W List Control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include "ClListCtrl.h"
#include "BBoxUtil.h"

#include <CLENAW/ClDialog.h>
#include <HELPER/ClLog.H>

//================================================================================================= E. INCLUDE
//================================================================================================= S. CON/DCON
IMPLEMENT_DYNAMIC(ClListCtrl, CListCtrl)

ClListCtrl::ClListCtrl()
{
	this->m_pBackground		= NULL;
	this->m_hFont			= NULL;
	m_nSelectItemNo		= -1;
	m_nStartItemNo			= -1;    // jun 191023 shift choice
	m_dwfile_time = 0;
	m_bIsAllCheck = FALSE;
	m_nSortItem = 0;
}


ClListCtrl::~ClListCtrl()
{
	if( this->m_pBackground )	delete this->m_pBackground;
}

//================================================================================================= E. CON/DCON
//================================================================================================= S. MESSAGE

BEGIN_MESSAGE_MAP(ClListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


BOOL ClListCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	ClMemDC memDC( pDC, &rect );
	if(memDC.m_hDC)
		memDC.BitBlt( 0, 0, rect.Width(), rect.Height(), &this->m_dcBackground, 0, 0, SRCCOPY );
	
	return TRUE;
}

BOOL ClListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(this->GetFocus() == this)
	{
		this->GetParent()->SetFocus();
		return TRUE;
	}
	else
		return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void ClListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    //do some stuff here
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	this->InvalidateRect( NULL );
}

//================================================================================================= E. MESSAGE
//================================================================================================= S. VIRTUAL


void ClListCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	LV_ITEM item_data;
	TCHAR buffer[256];

	LV_COLUMN column_data, prev_column_data;
	memset(&column_data, 0, sizeof(LV_COLUMN));
	memset(&prev_column_data, 0, sizeof(LV_COLUMN));

	column_data.mask = LVCF_WIDTH | LVCF_FMT;
	prev_column_data.mask = LVCF_WIDTH | LVCF_FMT;

	CDC* pDC;
	pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( this->m_hFont == NULL ) //20170523 <== if(! this->m_hFont)
	{
		this->m_hFont = ClGdiFactory::createGDIFont( lpDrawItemStruct->hDC, this->m_cstrFont, this->m_nFontSize,
			!!(this->m_dwFormat & ClDrawText::FORMAT_BOLD),
			!!(this->m_dwFormat & ClDrawText::FORMAT_ITALIC),
			!!(this->m_dwFormat & ClDrawText::FORMAT_UNDERLINE),
			!!(this->m_dwFormat & ClDrawText::FORMAT_STRIKEOUT) );
	}

	HFONT hOldFont = (HFONT)pDC->SelectObject( this->m_hFont );
	for(int column_index = 0; GetColumn(column_index, &column_data); column_index++)
	{
		DWORD dwFormat = 0;
		if(column_index > 0 )
		{
			GetColumn(column_index-1, &prev_column_data);
			lpDrawItemStruct->rcItem.left += prev_column_data.cx + 2;
			lpDrawItemStruct->rcItem.right = lpDrawItemStruct->rcItem.left + this->GetColumnInfo( column_index ) - 2;

			item_data.mask = LVIF_TEXT | LVIF_PARAM;
			item_data.iItem = lpDrawItemStruct->itemID;
			item_data.iSubItem = column_index;
			item_data.pszText = buffer;
			item_data.cchTextMax = sizeof(buffer);
			VERIFY(GetItem(&item_data));
		}
		else
		{
			lpDrawItemStruct->rcItem.left += 2;
			lpDrawItemStruct->rcItem.right = lpDrawItemStruct->rcItem.left + column_data.cx - 2;

			wsprintf( buffer, TEXT("%s   %d"),GetCheck(lpDrawItemStruct->itemID) ? TEXT("V ") : TEXT("   "),  lpDrawItemStruct->itemID + 1);
		}
		
/*		if(lpDrawItemStruct->itemState & ODS_SELECTED)
			pDC->SetTextColor( RGB(this->m_color[1].GetR(),this->m_color[1].GetG(),this->m_color[1].GetB()) );
		else if(column_index == 1 && CBBoxUtil::GetEventType(GetItemText(lpDrawItemStruct->itemID, 1)) == CBBoxUtil::BB_EVENT_GSENSOR ){//if(lpDrawItemStruct->itemState & ODS_GRAYED)   jun 170511  jun 201021 추가이벤트 색상수정 
			CString CFileName;
			CFileName = CBBoxUtil::GetFileName(GetItemText(lpDrawItemStruct->itemID, 1));
			if(CFileName.Find(_T("A")) >= 0) 
				pDC->SetTextColor( RGB(128, 128, 255));
			else if(CFileName.Find(_T("B")) >= 0) 
				pDC->SetTextColor( RGB(255, 0, 255));
			else
				pDC->SetTextColor( RGB(this->m_color[2].GetR(),this->m_color[2].GetG(),this->m_color[2].GetB()) );
		}
		else if(column_index == 1 && CBBoxUtil::GetEventType(GetItemText(lpDrawItemStruct->itemID, 1)) == CBBoxUtil::BB_EVENT_BUTTON )//if(lpDrawItemStruct->itemState & ODS_GRAYED)    jun 170511
			pDC->SetTextColor( RGB(this->m_color[3].GetR(),this->m_color[3].GetG(),this->m_color[3].GetB()) );
		else if(column_index == 1 && CBBoxUtil::GetEventType(GetItemText(lpDrawItemStruct->itemID, 1)) == CBBoxUtil::BB_EVENT_MOTION )//if(lpDrawItemStruct->itemState & ODS_GRAYED)    jun 170511
			pDC->SetTextColor( RGB(this->m_color[4].GetR(),this->m_color[4].GetG(),this->m_color[4].GetB()) );
		else
			pDC->SetTextColor( RGB(this->m_color[0].GetR(),this->m_color[0].GetG(),this->m_color[0].GetB()) );

*/



if(lpDrawItemStruct->itemState & ODS_SELECTED) {
    pDC->SetTextColor(RGB(this->m_color[1].GetR(), this->m_color[1].GetG(), this->m_color[1].GetB()));
} else if(column_index == 1 && CBBoxUtil::GetEventType(GetItemText(lpDrawItemStruct->itemID, 1)) == CBBoxUtil::BB_EVENT_GSENSOR) {
    CString CFileName = CBBoxUtil::GetFileName(GetItemText(lpDrawItemStruct->itemID, 1));
    
    // 두 번째 언더스코어 뒤의 첫 번째 문자를 추출
    int firstUnderscore = CFileName.Find(_T("_"));
    int secondUnderscore = CFileName.Find(_T("_"), firstUnderscore + 1);
    
    if (secondUnderscore != -1) {
        // 세 번째 언더스코어가 없는 경우도 처리
        CString targetChar = CFileName.Mid(secondUnderscore + 1, 1);

        if (targetChar == _T("B")) {
            pDC->SetTextColor(RGB(255, 0, 255)); // B일 때 색상
        } else if (targetChar == _T("A")) {
            pDC->SetTextColor(RGB(128, 128, 255)); // A일 때 색상
        } else {
            pDC->SetTextColor(RGB(this->m_color[2].GetR(), this->m_color[2].GetG(), this->m_color[2].GetB())); // 기본 색상
        }
    }
} else if(column_index == 1 && CBBoxUtil::GetEventType(GetItemText(lpDrawItemStruct->itemID, 1)) == CBBoxUtil::BB_EVENT_BUTTON) {
    pDC->SetTextColor(RGB(this->m_color[3].GetR(), this->m_color[3].GetG(), this->m_color[3].GetB()));
} else if(column_index == 1 && CBBoxUtil::GetEventType(GetItemText(lpDrawItemStruct->itemID, 1)) == CBBoxUtil::BB_EVENT_MOTION) {
    pDC->SetTextColor(RGB(this->m_color[4].GetR(), this->m_color[4].GetG(), this->m_color[4].GetB()));
} else {
    pDC->SetTextColor(RGB(this->m_color[0].GetR(), this->m_color[0].GetG(), this->m_color[0].GetB()));
}






		if( this->m_dwFormat & ClDrawText::FORMAT_LEFT )		dwFormat |= DT_LEFT;
		if( this->m_dwFormat & ClDrawText::FORMAT_CENTER )		dwFormat |= DT_CENTER;
		if( this->m_dwFormat & ClDrawText::FORMAT_RIGHT )		dwFormat |= DT_RIGHT;
		if( this->m_dwFormat & ClDrawText::FORMAT_TOP )			dwFormat |= DT_TOP;
		if( this->m_dwFormat & ClDrawText::FORMAT_VCENTER )		dwFormat |= DT_VCENTER;
		if( this->m_dwFormat & ClDrawText::FORMAT_BOTTOM )		dwFormat |= DT_BOTTOM;
		if( this->m_dwFormat & ClDrawText::FORMAT_WORDBREAK )	dwFormat |= DT_WORDBREAK;
		if( this->m_dwFormat & ClDrawText::FORMAT_WORDELLIPSIS )dwFormat |= DT_WORD_ELLIPSIS;

		//if(column_index == 0 ) // no
		//	dwFormat |= DT_CENTER;
			
		pDC->DrawText( buffer, _tcslen(buffer), &lpDrawItemStruct->rcItem, dwFormat );
	}
	pDC->SelectObject( hOldFont );
		
	return;
}









VOID ClListCtrl::TransferBackground( CDC* pDC )
{
	this->m_dcBackground.DeleteDC();

	CBitmap bmp, *pOldBitmap;
	CRect rtWindow;
	GetWindowRect(rtWindow);
	GetParent()->ScreenToClient(rtWindow);

	m_dcBackground.CreateCompatibleDC( pDC );
	bmp.CreateCompatibleBitmap( pDC, rtWindow.Width(), rtWindow.Height() );
	pOldBitmap = m_dcBackground.SelectObject(&bmp);
	m_dcBackground.BitBlt( 0, 0, rtWindow.Width(), rtWindow.Height(), pDC, rtWindow.left, rtWindow.top, SRCCOPY );
	bmp.DeleteObject();
	return;
}

//================================================================================================= E. VIRTUAL
//================================================================================================= S. OPERATION

//================================================================================================= E. OPERATION
//================================================================================================= S. CONTROL

void ClListCtrl::SetHeaderCheck( BOOL bFlag )
{
          HDITEM hditem;

          hditem.mask = HDI_IMAGE | HDI_FORMAT;
          GetHeaderCtrl()->GetItem( 0, &hditem );
          hditem.fmt = HDF_IMAGE | HDF_LEFT;

         if( bFlag )
                     hditem.iImage = 1;
          else
                     hditem.iImage = 0;

         GetHeaderCtrl()->SetItem( 0, &hditem );
}


BOOL ClListCtrl::SetDrawText( const CString& cstrFontFace, INT nFontSize, DWORD dwFormat, const Color& color, const Color& selColor, const Color& evtColor, const Color& manColor , const Color& motColor  )             // jun 170511
{
	if( this->m_hFont )
	{
		DeleteObject( this->m_hFont );
		this->m_hFont = NULL;
	}

	this->m_cstrFont = cstrFontFace;
	this->m_nFontSize = nFontSize;
	this->m_dwFormat = dwFormat;
	this->m_color[0] = color;
	this->m_color[1] = selColor;
	this->m_color[2] = evtColor;
	this->m_color[3] = manColor;             // jun 170511
	this->m_color[4] = motColor;             // jun 170511

	return TRUE;
}


BOOL ClListCtrl::SetBackground( const HashedString& ImagePath, const Color& color, BOOL bUseTpColor, const Color& tpColor,
							   ClDrawImage::DRAW_MODE drawMode, const ClGeometry& geoImage, const ClGeometry& geoSubImage,
							   DWORD dwGeoBase )
{
	if( !this->m_pBackground )
	{
		this->m_pBackground = new ClDrawImage();
		if( !this->m_pBackground )
		{
			CLD_FL( DF_ERROR, "Out of memory" );
			return FALSE;
		}
	}

	if( !this->m_pBackground->loadImage(ImagePath, color, !!bUseTpColor, tpColor, drawMode, geoImage, geoSubImage, dwGeoBase) )
	{
		delete this->m_pBackground;
		this->m_pBackground = NULL;
		return FALSE;
	}

	return TRUE;
}


VOID ClListCtrl::AdaptItemHeight( VOID )
{
	m_imgList.Create(1, this->m_nItemHeight, ILC_COLOR4, 1, 1);
	this->SetImageList( &m_imgList, LVSIL_SMALL );

	return;
}


VOID ClListCtrl::AdaptColumn( VOID )
{
	int nCounter;
	const int nListSize = this->GetColumnInfoListSize();
	for( nCounter = 0; nCounter < nListSize; nCounter++ )
	{
		//INT nWidth = this->GetColumnInfo( nCounter );
		//this->InsertColumn( nCounter, TEXT("ITEM"), LVCFMT_LEFT, nWidth );

		LVCOLUMN COL;
		COL.mask	=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT |LVCF_SUBITEM;
		COL.fmt		=LVCFMT_LEFT;

		COL.cx		=this->GetColumnInfo( nCounter );
		COL.pszText	=TEXT("ITEM");
		COL.iSubItem	=nCounter;
		this->InsertColumn(nCounter, &COL);
	}

	return;
}

VOID ClListCtrl::AddColumnInfo( INT nIndex, INT nWidth )
{
	if( nIndex < 0 || nWidth < 0 )	return;

	ITER_COLUMNINFO iInfo = this->m_ColumnInfoList.find( nIndex );
	if( iInfo == this->m_ColumnInfoList.end() )
		this->m_ColumnInfoList.insert( PAIR_COLUMNINFO(nIndex,nWidth) );
	else
		iInfo->second = nWidth;

	return;
}


BOOL ClListCtrl::RemoveColumnInfo( INT nIndex )
{
	ITER_COLUMNINFO iInfo = this->m_ColumnInfoList.find( nIndex );
	if( iInfo == this->m_ColumnInfoList.end() )
		return FALSE;

	this->m_ColumnInfoList.erase( iInfo );

	return TRUE;
}


VOID ClListCtrl::ClearColumnInfo( VOID )
{
	if( this->m_ColumnInfoList.empty() )
		return;

	this->m_ColumnInfoList.clear();

	return;
}

//================================================================================================= E. CONTROL
//================================================================================================= S. GET/SET

VOID ClListCtrl::SetItemHeight( INT nHeight )
{
	if( nHeight < 0 ) return;
	this->m_nItemHeight = nHeight;

	return;
}


INT ClListCtrl::GetItemHeight( VOID )
{
	return this->m_nItemHeight;
}


BOOL ClListCtrl::IsColumnInfoListEmpty( VOID )
{
	return this->m_ColumnInfoList.empty();
}


INT ClListCtrl::GetColumnInfoListSize( VOID )
{
	return this->m_ColumnInfoList.size();
}


INT ClListCtrl::GetColumnInfo( INT nIndex )
{
	ITER_COLUMNINFO iInfo = this->m_ColumnInfoList.find( nIndex );
	if( iInfo == this->m_ColumnInfoList.end() )
		return NULL;

	return iInfo->second;
}


ClDrawImage* ClListCtrl::GetBackground( VOID )
{
	return this->m_pBackground;
}

//================================================================================================= E. GET/SET
//================================================================================================= S. SORT
BOOL ClListCtrl::Sort(int nSubItem, int nUpDown ) 
{
	if(nUpDown >= SORT_UPDOWN)
		m_nSortUpDown = !m_nSortUpDown;
	else
		m_nSortUpDown = nUpDown;

	m_nSortItem = nSubItem;
	
	if(nSubItem == SORT_STR_DATE)
		return this->SortItems(SortFunc0, (LPARAM) this);
	else if(nSubItem == SORT_FILENAME)
		return this->SortItems(SortString, (LPARAM) this);
	else if(nSubItem == SORT_SIZE)
		return this->SortItems(SortFunc2, (LPARAM) this);
	else if(nSubItem == SORT_PATH)
		return this->SortItems(SortString, (LPARAM) this);
	else if(nSubItem == SORT_FILENAME2)
		return this->SortItems(SortString, (LPARAM) this);
	else
		return FALSE;
}

//date
int CALLBACK ClListCtrl::SortFunc0(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	ClListCtrl *pListCtrl = (ClListCtrl*) lParamSort;
	
	LVFINDINFO info1, info2;
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;
	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;
	int irow1 = pListCtrl->FindItem(&info1,-1);
	int irow2 = pListCtrl->FindItem(&info2,-1);

	
	CString strItem1 = pListCtrl->GetItemText(irow1, SORT_STR_DATE);
	CString strItem2 = pListCtrl->GetItemText(irow2, SORT_STR_DATE);	
//	_tprintf(_T("	%s : %s (%d) \r\n"), strItem1, strItem2, strItem1.Compare(strItem2));

	__int64 val1 = _tstoi64(strItem1);
	__int64 val2 = _tstoi64(strItem2);

//	__int64 val1 = pListCtrl->GetItemData(irow1);
//	__int64 val2 = pListCtrl->GetItemData(irow2);

	if(pListCtrl->m_nSortUpDown)
		return(val2 - val1);
	else
		return(val1 - val2);
}

//path
int CALLBACK ClListCtrl::SortString(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	ClListCtrl *pListCtrl = (ClListCtrl*) lParamSort;
	
	LVFINDINFO info1, info2;
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;
	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;
	int irow1 = pListCtrl->FindItem(&info1,-1);
	int irow2 = pListCtrl->FindItem(&info2,-1);

	CString strItem1 = pListCtrl->GetItemText(irow1, pListCtrl->m_nSortItem);
	CString strItem2 = pListCtrl->GetItemText(irow2, pListCtrl->m_nSortItem);	

//	_tprintf(_T("%d %s : %d %s (%d) \r\n"), irow1, strItem1, irow2, strItem2, strItem1.Compare(strItem2));

	if(pListCtrl->m_nSortUpDown)
		return strItem2.Compare(strItem1);
	else
		return strItem1.Compare(strItem2);		
}


//size
int CALLBACK ClListCtrl::SortFunc2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	ClListCtrl *pListCtrl = (ClListCtrl*) lParamSort;
	
	LVFINDINFO info1, info2;
	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;
	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;
	int irow1 = pListCtrl->FindItem(&info1,-1);
	int irow2 = pListCtrl->FindItem(&info2,-1);

	
	CString strItem1 = pListCtrl->GetItemText(irow1, SORT_SIZE);
	CString strItem2 = pListCtrl->GetItemText(irow2, SORT_SIZE);	
		
	int val1 = _tstof(strItem1) * 1000;
	int val2 = _tstof(strItem2) * 1000;

	if(pListCtrl->m_nSortUpDown)
		return(val1 - val2);
	else
		return(val2 - val1);
}

//================================================================================================= E. SORT
