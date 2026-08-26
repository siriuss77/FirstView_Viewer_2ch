#pragma once


#include "PageParse.h"
#include "PageCollect.h"
#include "PageMerge.h"

#include <CLUDE/ClCludeApp.H>


class CCludeWizard : public CPropertySheetEx
{
	DECLARE_DYNAMIC(CCludeWizard)

public:
	CCludeWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CCludeWizard();

protected:
	DECLARE_MESSAGE_MAP()

private:
	
	CPageParse			m_PageParse;
	CPageCollect		m_PageCollect;
	CPageMerge			m_PageMerge;

	ClCludeApp			m_CludeApp;
	CImageCollector		m_imageCollector;
};
