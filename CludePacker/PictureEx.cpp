// PictureEx.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CludePacker.h"
#include "PictureEx.h"
#include "memdc.h"


// CPictureEx

IMPLEMENT_DYNAMIC(CPictureEx, CStatic)

CPictureEx::CPictureEx()
{

}

CPictureEx::~CPictureEx()
{
}


BEGIN_MESSAGE_MAP(CPictureEx, CStatic)
END_MESSAGE_MAP()



// CPictureEx 메시지 처리기입니다.

void CPictureEx::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect rect;
	CMemDC dc( pDC, &rect, FALSE );

	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	return;
}
