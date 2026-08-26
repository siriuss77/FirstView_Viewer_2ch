//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: ClWebBrowser.CPP
//
//	Description: CLENA-W WebBrowser control
//
//	Update History
// 
//		2011.11.23 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include <stdafx.h>
#include <CLENAW/DigitNum.h>
#include "ClWebBrowser.h"

//================================================================================================= E. INCLUDE

IMPLEMENT_DYNCREATE(ClWebBrowser, CWnd)

//================================================================================================= S. MESSAGE

//================================================================================================= E. MESSAGE
//================================================================================================= S. OPERATION

BEGIN_MESSAGE_MAP(ClWebBrowser, CWnd)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void ClWebBrowser::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

HRESULT ClWebBrowser::ExcuteScript(const CString & FuncName, const CStringArray & strParamArr, const std::vector<VARENUM>&  vtArr , VARIANT * vaResult)
{
	HRESULT		hr;

	IDispatch		*spDisp = NULL;
	IHTMLDocument2	*spDocument = NULL;

	spDisp = get_Document();
	if (spDisp == NULL)
	{
		return S_FALSE;
	}

	hr = spDisp->QueryInterface(IID_IHTMLDocument2, (void**)&spDocument);
	spDisp->Release();
	spDisp = NULL;


	if (FAILED(hr) || spDocument == NULL)
	{
		return hr;
	}

	BSTR	szMember = FuncName.AllocSysString();
	DISPID	dispid = NULL;

	IDispatch	*spScript;
	hr = spDocument->get_Script(&spScript);
	spDocument->Release();
	spDocument = NULL;

	if (FAILED(hr))
	{
		return hr;
	}

	hr = spScript->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
	spScript->Release();
	SysFreeString(szMember);

	if (SUCCEEDED(hr))
	{
		EXCEPINFO excepInfo;
		memset(&excepInfo, 0, sizeof(EXCEPINFO));
		UINT	nArgErr = (UINT)-1;

		// putting parameters
		DISPPARAMS	dispParams;
		memset(&dispParams, 0, sizeof(DISPPARAMS));
		dispParams.cArgs		= strParamArr.GetSize();
		dispParams.rgvarg		= new VARIANT[strParamArr.GetSize()];

		for (int i=0; i<strParamArr.GetSize(); i++)
		{
			if (vtArr[i] == VT_R8)
			{
				dispParams.rgvarg[i].dblVal = _tcstod(strParamArr[i], NULL);
			}
			else if (vtArr[i] == VT_BSTR)
			{
				BSTR bstr = strParamArr[i].AllocSysString();
				dispParams.rgvarg[i].bstrVal = bstr;
				SysFreeString(bstr);
			}
			else if(vtArr[i] == VT_I4)
			{
				dispParams.rgvarg[i].intVal = _ttoi(strParamArr[i]);
			}
			dispParams.rgvarg[i].vt = (VARTYPE)vtArr[i];

		}
		dispParams.cNamedArgs = 0;

		hr = spScript->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, vaResult, &excepInfo, &nArgErr);

		delete [] dispParams.rgvarg;

		if (FAILED(hr))
		{
			return hr;
		}
	}

	return hr;
}

CString ClWebBrowser::ExecuteScript2(const CString& strFunction)
{
	if(strFunction.GetLength() == 0) 
		return CString(); 

	int     nFind[2]; 
	TCHAR   szSeps[] = _T(" ,'\"\t();"); 
	TCHAR*  pszToken = NULL; 
	TCHAR*  pszNextToken = NULL;    // _tcstok_s → 3parameter _Context 
	CString strName, strValue(strFunction), strTemp, strReturn; 
	CStringArray astrArgument; 

	nFind[0] = strFunction.Find(_T("javascript:")); 
	if(nFind[0] > -1) 
	{ 
		strValue = (LPCTSTR)strFunction.Mid(nFind[0] + 11); 
	} 

	nFind[0] = strValue.Find(_T("(")); 
	strName  = (LPCTSTR)strValue.Left(nFind[0]); 
	strName.TrimLeft (); 
	strName.TrimRight(); 


	strTemp  = (LPCTSTR)strValue; 
	strValue = (LPCTSTR)strTemp.Mid(nFind[0] + 1); 
	//  pszToken = _tcstok((TCHAR *)(LPCTSTR)strValue, szSeps); 
	pszToken = _tcstok_s((TCHAR *)(LPCTSTR)strValue, szSeps, &pszNextToken);    // warning C4996: 'wcstok' 

	while(pszToken != NULL) 
	{ 
		astrArgument.Add((LPCTSTR)pszToken); 
		//      pszToken = _tcstok(NULL, szSeps); 
		pszToken = _tcstok_s(NULL, szSeps, &pszNextToken);  // warning C4996: 'wcstok' 
	} 

	HRESULT     hr; 
	CComVariant vtResult(0); 
	CComPtr<IDispatch>      spDisp = NULL; 
	CComPtr<IHTMLDocument2> spDocument = NULL; 

	//spDisp = GetHtmlDocument();
	spDisp = ClWebBrowser::get_Document(); 

	if(spDisp == NULL) 
		return CString(); 

	hr = spDisp->QueryInterface(IID_IHTMLDocument2, (void **)&spDocument); 
	spDisp = NULL; 
	if(FAILED(hr) || spDocument == NULL) 
		return CString(); 

	hr = spDocument->get_Script(&spDisp); 
	spDocument = NULL; 
	if(SUCCEEDED(hr) && spDisp != NULL) 
	{ 
		BSTR   bstrFunction = strName.AllocSysString(); 
		DISPID dispid; 

		hr = spDisp->GetIDsOfNames(IID_NULL, &bstrFunction, 1, 
			LOCALE_SYSTEM_DEFAULT, &dispid); 
		::SysFreeString(bstrFunction); 

		if(SUCCEEDED(hr)) 
		{ 
			long         lCount = astrArgument.GetSize(); 
			CComVariant *pvarArguments = NULL; 
			VARIANT      varParam = {0}; 
			DISPPARAMS   dispParams = {0}; 

			if(lCount > 0) 
			{ 
				pvarArguments = new CComVariant[lCount]; 
				for(int i = 0 ; i < lCount ; i++) 
				{ 
					pvarArguments[lCount - 1 - i] = (LPCTSTR)astrArgument[i]; 
				} 

				dispParams.rgvarg = pvarArguments; 
			}
			else
			{ 
				dispParams.rgvarg = &varParam; 
			} 
			dispParams.cArgs = lCount; 

			hr = spDisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT,  
				DISPATCH_METHOD, &dispParams, &vtResult, NULL, NULL); 


			if(vtResult.vt != VT_EMPTY) 
				strReturn = vtResult.bstrVal;

			if(lCount > 0) 
				delete[] pvarArguments; 
		} 

		spDisp = NULL; 
	} 

	return strReturn;    

}


void ClWebBrowser::Synchronize(BOOL internet_use,  unsigned long dwCTS,  BOOL bRedraw /*= FALSE*/ )
{
	BOOL draw_speed = 0;
	if( this->m_pSensorData->m_GPSList.size() > 0 )
	{
		CTextData::ITER_GPS iGPS	= this->m_pSensorData->m_GPSList.begin();
		CTextData::ITER_GPS iGPSEnd	= this->m_pSensorData->m_GPSList.end();

		if( iGPS->m_bNotDegree) //RDF DATA
		{
			CTextData::ITER_GPS _GPS = iGPSEnd;
			_GPS--;

			for( ; _GPS != iGPS; _GPS-- )
			{
				if( dwCTS > _GPS->m_cts )
					break;
			}
			iGPS = _GPS;
		}
		else
		{
			if(dwCTS < 3600)
				dwCTS /= 2;
			else
				dwCTS -= 900;

			for( ; iGPS != iGPSEnd; iGPS++ )
			{
				if( dwCTS < iGPS->m_cts )
					break;
			}
		}

		if( iGPS == iGPSEnd){
			goto exit;
		}

		if(m_dwCTS == iGPS->m_cts && bRedraw == FALSE) 
			return;
		
		if( iGPS->m_bValid)
		{
			draw_speed = 1;
			
			if(internet_use){
				m_bIsMapShow = TRUE;
				MoveWindow( m_rectClient );
			}
			
			if(m_fLat !=  iGPS->GetLat() || m_fLng != iGPS->GetLng() || bRedraw)
			{
				CString strFunction;
				CString cstrValue;
				m_GpsData = *iGPS;
				m_fLat =  iGPS->GetLat();
				m_fLng = iGPS->GetLng();
				m_fSpeed =  iGPS->m_fSpeed;

				DrawText( m_fLat, m_fLng,  m_fSpeed);

				if(internet_use){
					strFunction.Format( TEXT("javascript:DrawCurrent(\'%lf\', \'%lf\', \'%d\')"), iGPS->GetLng(), iGPS->GetLat(), bRedraw);
 					ExecuteScript2( strFunction );
				}
			}
			m_dwCTS = iGPS->m_cts;
		}
		else if(m_bIsMapShow || bRedraw)
		{
			m_bIsMapShow = FALSE;
			DrawText();
			MoveWindow( 0, 0, 0, 0 );
			
		}
	}
	else if(m_bIsMapShow || bRedraw)
	{
		m_bIsMapShow = FALSE;
		DrawText();
		MoveWindow( 0, 0, 0, 0 );
	}

exit:

	if(!draw_speed){
		if(m_fSpeed != 0.0) {
			m_fSpeed = 0.0;

			if(m_pTextSpeed){
#ifdef SPEED_DIGIT_PONT_USE

			if(!this->m_pSensorData->m_bPulseSpeedUse){                  // 220824 jun GPS 0값일대 펄스 0으로 표기되는 부분 수정
				CDigitNum* pDigit = (CDigitNum * )m_pTextSpeed;
				
				pDigit->DrawDigits((int)(m_fSpeed));	
			}
#else
#if (BUILD_LANGUAGE == LANGUAGE_KOREAN)
			 	m_pTextSpeed->SetWindowText(TEXT("속도 : 0"));
#else
				m_pTextSpeed->SetWindowText(TEXT("Speed : 0"));
#endif					
				m_pTextSpeed->Invalidate();
#endif
			}
		}
	}
	return;
}

void ClWebBrowser::drawPoly( void )
{
	CString strFunction;
		
	strFunction.Format( TEXT("javascript:ResetData()"));
	ExecuteScript2( strFunction );

	if( this->m_pSensorData->m_GPSList.size() > 0 )
	{
		CTextData::ITER_GPS iGPS	= this->m_pSensorData->m_GPSList.begin();
		CTextData::ITER_GPS iGPSEnd	= this->m_pSensorData->m_GPSList.end();
		
		for( ; iGPS != iGPSEnd; iGPS++ )
		{
			if( iGPS->m_bValid && ( this->m_fLat !=  iGPS->GetLat() || this->m_fLng != iGPS->GetLng()))
			{
				this->m_fLat =  iGPS->GetLat();
				this->m_fLng = iGPS->GetLng();
				
				strFunction.Format( TEXT("javascript:cli(\'%lf\', \'%lf\')"), iGPS->GetLng(), iGPS->GetLat() );
				ExecuteScript2( strFunction );
			}	
		}

		strFunction.Format( TEXT("javascript:drawPoly()"));
		ExecuteScript2( strFunction );

	}

	return;
}
//================================================================================================= E. OPERATION
//================================================================================================= S. GET/SET

void ClWebBrowser::SetSensorData( CTextData* pSensorData)
{
	this->m_pSensorData = pSensorData;
	//this->drawPoly();
	return;
}


VOID ClWebBrowser::SetUiText(CStatic* pTextLatitude, CStatic* pTextLongitude, CStatic* pTextSpeed /* = NULL */, CStatic* pTextAltitude /* = NULL */) 
{
	m_pTextLatitude=pTextLatitude;
	m_pTextLongitude = pTextLongitude;
	m_pTextSpeed = pTextSpeed;
	m_pTextAltitude = pTextAltitude; 

	DrawText();
}

VOID ClWebBrowser::DrawText(double dLatitude, double dLongitude, double dSpeed) 
{
	CString cstrValue;
		
	if(m_pTextLatitude)
	{
		CString cstrText(TEXT(""));
#if BUILD_MODEL == MODEL_RALLY_EYES
		if(m_GpsData.m_bValid && dLatitude != 0)
		{
			TCHAR ew[] = TEXT("N   ");
			double fLat = m_GpsData.m_fLat;
			int degree = 0;
			 if( fLat < 0)
			 {
			  	ew [0]= TEXT('S');
				fLat = -fLat;
			 }

			degree = ((int)fLat / 100);
			if(degree > 100)
				ew[1] = 0;
			else if(degree > 10)
				ew[2] =  0;

			if(m_GpsData.m_bNotDegree)
				cstrText.Format(TEXT("%s%.6f"), ew, fLat);
			else
				cstrText.Format(TEXT("%s%d`%02d\'%02d.%02d\""), ew, degree, (int)(fLat)%100,(int)(fLat*100) % 100, (int)(fLat*10000) % 100);
		}
#else
		if(dLatitude != 0)
		{
#if (BUILD_LANGUAGE == LANGUAGE_KOREAN)
			cstrText = CString(TEXT("위도 : "));
#else
			cstrText = CString(TEXT("Lat : "));
#endif

			cstrValue.Format(TEXT("%0.6f"), dLatitude);	

			cstrText += cstrValue;
		}
#endif //end of (BUILD_MODEL != MODEL_RALLY_EYES)
		
		m_pTextLatitude->SetWindowText(cstrText);
		m_pTextLatitude->Invalidate();
	}

	if(m_pTextLongitude )
	{
		CString cstrText(TEXT(""));
#if BUILD_MODEL == MODEL_RALLY_EYES	
		if(m_GpsData.m_bValid && dLongitude != 0)
		{
			TCHAR ew[] = TEXT("E   ");
			double fLng = m_GpsData.m_fLng;
			int degree = 0;
			 if( fLng < 0)
			 {
			  	ew [0]= TEXT('W');
				fLng = -fLng;
			 }

			degree = ((int)fLng / 100);
			if(degree > 100)
				ew[1] = 0;
			else if(degree > 10)
				ew[2] =  0;

			if(m_GpsData.m_bNotDegree)
				cstrText.Format(TEXT("%s%.6f"), ew, fLng);
			else
				cstrText.Format(TEXT("%s%d`%02d\'%02d.%02d\""), ew, degree, (int)(fLng)%100,(int)(fLng*100) % 100, (int)(fLng*10000) % 100);
		}
#else

		if(dLongitude != 0)
		{
#if (BUILD_LANGUAGE == LANGUAGE_KOREAN)
			cstrText = CString(TEXT("경도 : "));
#else
			cstrText = CString(TEXT("Lon : "));
#endif
			cstrValue.Format(TEXT("%0.6f"), dLongitude);	

			cstrText += cstrValue;
		}
#endif //end of (BUILD_MODEL != MODEL_RALLY_EYES)
		
		m_pTextLongitude->SetWindowText(cstrText);
		m_pTextLongitude->Invalidate();
	}

	if(m_pTextSpeed)
	{
		CString cstrText(TEXT(""));

		if( dLatitude != 0 && dLongitude != 0 && (!this->m_pSensorData->m_bPulseSpeedUse || this->m_pSensorData->m_PulseList.size() <= 0))
		{
#ifdef SPEED_DIGIT_PONT_USE
			CDigitNum* pDigit = (CDigitNum * )m_pTextSpeed;

			pDigit->DrawDigits((int)(dSpeed * 1.853184));
	
#else	
 #if (BUILD_LANGUAGE == LANGUAGE_KOREAN)
		 	cstrText = CString(TEXT("속도 : "));
 #else
			cstrText = CString(TEXT("Speed : "));
 #endif
	
			switch(m_eUnitOfSpeed)
			{
			case UNIT_MILE_P_H: 	cstrValue.Format(TEXT("%0d mile/h"), (int)(dSpeed * 1.150779));	break;
			case UNIT_KNOT: 		cstrValue.Format(TEXT("%0d knot"), (int)(dSpeed));	break;
			
			case UNIT_KM_P_H: 	
			default:				cstrValue.Format(TEXT("%0d km/h"),  (int)(dSpeed * 1.853184));	break;
			}
			
			m_pTextSpeed->SetWindowText(cstrText);
			m_pTextSpeed->Invalidate();
#endif //end of (BUILD_MODEL != MODEL_RALLY_EYES)		
			cstrText += cstrValue ;	
		}
	}
}

//================================================================================================= E. GET/SET
