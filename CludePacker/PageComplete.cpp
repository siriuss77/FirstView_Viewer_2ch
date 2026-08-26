// PagePackComplete.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CludePacker.h"
#include "PageComplete.h"


// CPagePackComplete 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPageComplete, CPropertyPageEx)

CPageComplete::CPageComplete()
	: CPropertyPageEx(CPageComplete::IDD)
{

}

CPageComplete::~CPageComplete()
{

}

void CPageComplete::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageComplete, CPropertyPageEx)
END_MESSAGE_MAP()


// CPagePackComplete 메시지 처리기입니다.

BOOL CPageComplete::OnSetActive()
{
	CPropertySheetEx* pSheet = (CPropertySheetEx*)GetParent();
	pSheet->SetWizardButtons( PSWIZB_FINISH );

	return CPropertyPageEx::OnSetActive();
}
