#pragma once


#include <CLENAW/ClDialog.h>
#include <BLACKBOX/Language.h>
#include <BBConfigure.h>
#include <ConfigTextFile.h>

class CSettingDlg : public ClDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);
	virtual ~CSettingDlg();

	enum { IDD = IDD_SETTING_DLG };


public:

			void					SetSelDrive			( CString cstrSelDrive );

			void					SetLanguage				( CLanguage * cLang);
			void					SetPassword			( CString cstrPassword) { m_cstrPassword = cstrPassword; }
			CString 				GetPassword				(void) { return m_cstrPassword; }

protected:

	virtual void					DoDataExchange		( CDataExchange* pDX );
	virtual VOID					InitResourceID		( VOID );

	DECLARE_MESSAGE_MAP()

public:

	virtual	BOOL					OnInitDialog		( VOID );

	afx_msg HBRUSH 				OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void 					OnTimer(UINT nIDEvent);
	
	afx_msg void 					OnBnClickedSettingClose();
	

	afx_msg void					OnClickSettingInit	( VOID );
	afx_msg void					OnClickSettingSave	( VOID );
	afx_msg void					OnClickSettingCancel( VOID );

	afx_msg void					OnClickPassword		(VOID);
	afx_msg void					OnClickFormatDisk	( VOID );
	afx_msg void					OnClickSetTime		( VOID );
	afx_msg void					OnClickPulsereset	( VOID );
	afx_msg void 					OnBnClickedSettingQuality();
	afx_msg void 					OnBnClickedSettingEventCapacity();
	afx_msg	LRESULT				OnSliderScroll		( WPARAM wParam, LPARAM lParam );
	
	void 						OnLanguageChange(void);

protected:

			void  					SetItemText(int nIDDlgItem, UINT resource, CString index_str = NULL );
			VOID					SetResourceState	( VOID );
			BOOL 					InitConfigValue(LPST_CFG_DAVIEW spConfig = NULL);
			VOID 					updateGsensorValue(int nPos, int nRange);
			VOID 					updateCylindersValue(int nPos, int nRange);  // jun 20201202
			VOID 					updateVolumeValue(int nPos);
			BOOL 					SelectMemory(LPWSTR pszPathname, LPWSTR pszVolumeName = NULL, LPWSTR pszTitle = NULL);
			void 					SaveFormatTextFile(CString file_dir, LPWSTR file_name = NULL);
private:

			BOOL				m_bConfigOpen;
			BOOL				m_bTimeSetSave;
			CBBConfigure			m_Configure;
			CLanguage * 			m_pCLanguage;

			CComboBox 			m_cbTimeZone;
			CString				m_csCurrenttime;
			CString				m_csCarNumber;         // jun 200805
			CString				m_csDriverCode;         // jun 2021017
			CComboBox 			m_cbTimeOffset;

			bool					m_bChangedEventMode;
			int					m_ipreEventMode;

			bool					m_bChangedVideoQualit;
			int 					m_ipreVideoQuality;
			int					m_ipreGsensorSensi;
			ST_CFG_DAVIEW		m_stCfgDaview;
			ST_CFG_TIME			m_stTime;
			CString					m_cstrSelDrive;
			CString				m_cstrPassword;

			HBRUSH 				m_hBgBrush;
public:
	afx_msg void OnStnClickedSettingStaticDeviceGsensorSensitive();
	afx_msg void OnStnClickedSettingStaticDisplayTimezone();
	afx_msg void OnStnClickedSettingStaticSpeakerVolumeValue();
	afx_msg void OnEnChangeSettingEditDrivercode();
};
