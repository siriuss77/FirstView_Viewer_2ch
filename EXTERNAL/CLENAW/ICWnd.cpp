// TWWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ICWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CICWnd

CICWnd::CICWnd()
{
}

CICWnd::~CICWnd()
{
}


BEGIN_MESSAGE_MAP(CICWnd, CStatic)
	//{{AFX_MSG_MAP(CICWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICWnd message handlers

CSize CICWnd::GetWindowSize()
{
	CRect	rcWin;

	GetClientRect(&rcWin);

	return CSize(rcWin.Width(), rcWin.Height());
}
