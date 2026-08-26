#pragma once


#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>

//================================================================================================= S.PREPROCESSOR

#define	OPEN_NOTIFY_DIALOG(NOTIFY_MESSAGE_ID, Lang)														\
do{																									\
	ClCludeApp::PNOTIFYMESSAGE pMessage =															\
		((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getMessage( NOTIFY_MESSAGE_ID );			\
	if( pMessage && pMessage->m_wtsTitle.length() && pMessage->m_wtsComment.length() )				\
	{																								\
		CNotifyDlg notifyDialog(this);																\
		const char* xmlFile =																		\
			((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getXml( TEXT("NOTIFY") );				\
		notifyDialog.SetLanguage(Lang);															\
		if( xmlFile && notifyDialog.OpenCludeBuffer(xmlFile) )										\
		{																							\
			notifyDialog.SetTitle( pMessage->m_wtsTitle.c_str() );									\
			notifyDialog.SetComment( pMessage->m_wtsComment.c_str() );								\
			notifyDialog.DoModal();																	\
		}																							\
	}																								\
} while(0)

#define	OPEN_SLECTION_DIALOG(RETURN, NOTIFY_MESSAGE_ID, Lang)												\
do{																									\
	ClCludeApp::PNOTIFYMESSAGE pMessage =															\
		((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getMessage( NOTIFY_MESSAGE_ID );			\
	if( pMessage && pMessage->m_wtsTitle.length() && pMessage->m_wtsComment.length() )				\
	{																								\
		CNotifyDlg notifyDialog(this);																\
		const char* xmlFile =																		\
		((CBBPlayerApp*)AfxGetApp())->GetGlobalSetting()->getXml( TEXT("NOTIFY") );					\
		notifyDialog.SetLanguage(Lang);														\
		if( xmlFile && notifyDialog.OpenCludeBuffer(xmlFile) )										\
		{																							\
			notifyDialog.SetTitle( pMessage->m_wtsTitle.c_str() );									\
			notifyDialog.SetComment( pMessage->m_wtsComment.c_str() );								\
			notifyDialog.SetNotifyStyle(CNotifyDlg::STYLE_OKCANCEL);								\
			RETURN = notifyDialog.DoModal();														\
		}																							\
	}																								\
} while(0)

//================================================================================================= E.PREPROCESSOR
//================================================================================================= S.CLASS

class CNotifyDlg : public ClDialog
{
	DECLARE_DYNAMIC(CNotifyDlg)

public:
	CNotifyDlg(CWnd* pParent = NULL);
	virtual ~CNotifyDlg();

	enum { IDD = IDD_NOTIFY_DLG };

	enum NOTIFY_STYLE
	{
		STYLE_NORMAL	= 0	,
		STYLE_OKCANCEL		,
		STYLE_INVALID
	};

public:

			BOOL					SetNotifyStyle		( NOTIFY_STYLE style );
			BOOL					SetTitle			( CString cstrTitle );
			BOOL					SetComment			( CString cstrComment );
			void						SetLanguage				( CLanguage * cLang) 	{m_pCLanguage = cLang;}

protected:

	virtual	void					DoDataExchange		( CDataExchange* pDX );
	virtual	VOID					InitResourceID		( VOID );
			VOID					SetResourceState	( VOID );

	DECLARE_MESSAGE_MAP()

public:

	virtual	BOOL					OnInitDialog		();

public:
	afx_msg void					OnClickSelCancel	();
	afx_msg void					OnClickSelOK		();

protected:
			CLanguage * 				m_pCLanguage;
			NOTIFY_STYLE			m_notifyStyle;
			CString					m_cstrTitle;
			CString					m_cstrComment;
};

//================================================================================================= E.CLASS

