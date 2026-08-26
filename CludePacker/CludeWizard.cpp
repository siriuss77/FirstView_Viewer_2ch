// CludeWizard.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CludePacker.h"
#include "CludeWizard.h"


// CCludeWizard

IMPLEMENT_DYNAMIC(CCludeWizard, CPropertySheetEx)

CCludeWizard::CCludeWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheetEx(pszCaption, pParentWnd, iSelectPage)
{
	this->AddPage( &this->m_PageParse );
	this->AddPage( &this->m_PageCollect );
	this->AddPage( &this->m_PageMerge );

	this->m_psh.dwFlags |= PSH_WIZARD;
	this->m_psh.dwFlags &= ~PSH_HASHELP;

	this->m_PageParse.m_psp.dwFlags		&= ~ PSP_HASHELP;
	this->m_PageCollect.m_psp.dwFlags	&= ~ PSP_HASHELP;
	this->m_PageMerge.m_psp.dwFlags		&= ~ PSP_HASHELP;

	this->m_PageParse.SetCludeApp( &this->m_CludeApp );
	this->m_PageCollect.SetCludeApp( &this->m_CludeApp );
	this->m_PageCollect.SetImageCollector( &this->m_imageCollector );
	this->m_PageMerge.SetCludeApp( &this->m_CludeApp );
	this->m_PageMerge.SetImageCollector( &this->m_imageCollector );
}


CCludeWizard::~CCludeWizard()
{
}


BEGIN_MESSAGE_MAP(CCludeWizard, CPropertySheetEx)
END_MESSAGE_MAP()


// CCludeWizard 메시지 처리기입니다.
