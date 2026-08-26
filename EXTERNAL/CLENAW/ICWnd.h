#if !defined(AFX_ICWND_H__29F44D89_EA5D_4060_92C9_886D6401054D__INCLUDED_)
#define AFX_ICWND_H__29F44D89_EA5D_4060_92C9_886D6401054D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ICWnd.h : header file
//

#include	"ICMessage.h"

/////////////////////////////////////////////////////////////////////////////
// CICWnd window

class AFX_EXT_CLASS CICWnd : public CStatic
{
// Construction
public:
	CICWnd();

// Attributes
public:

// Operations
public:
	virtual CSize	GetWindowSize();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CICWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CICWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICWND_H__29F44D89_EA5D_4060_92C9_886D6401054D__INCLUDED_)
