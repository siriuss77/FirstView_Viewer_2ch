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

#include "stdafx.h"
#include "wbcMapView.h"

//================================================================================================= E. INCLUDE

IMPLEMENT_DYNCREATE(CWBCMapView, CWnd)

//================================================================================================= S. MESSAGE

//================================================================================================= E. MESSAGE
//================================================================================================= S. OPERATION

HRESULT CWBCMapView::ExcuteScript(const CString & FuncName, const CStringArray & strParamArr, const std::vector<VARENUM>&  vtArr , VARIANT * vaResult)
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

CString CWBCMapView::ExecuteScript2(const CString& strFunction)
{
	if(strFunction.GetLength() == 0) 
		return CString(); 

	int     nFind[2]; 
	TCHAR   szSeps[] = _T(" ,'\"\t();"); 
	TCHAR*  pszToken = NULL; 
	TCHAR*  pszNextToken = NULL;    // _tcstok_s ¡æ 3parameter _Context 
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
	spDisp = CWBCMapView::get_Document(); 

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

		hr = spDisp->GetIDsOfNames(IID_NULL, &bstrFunction, 1, LOCALE_SYSTEM_DEFAULT, &dispid); 
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


void CWBCMapView::Synchronize( unsigned long dwCTS )
{
	if( this->m_pTextData->m_GPSList.size() > 0 )
	{
		CWBCTextData::ITER_GPS iGPS	= this->m_pTextData->m_GPSList.begin();
		CWBCTextData::ITER_GPS iGPSEnd	= this->m_pTextData->m_GPSList.end();

		for( ; iGPS != iGPSEnd; iGPS++ )
		{
			if( dwCTS < iGPS->m_cts )
				break;
		}
		if( iGPS == iGPSEnd ) return;
		if( iGPS->m_bValid )
		{
			CString strFunction;
			strFunction.Format( TEXT("javascript:DrawCurrent(\'%lf\', \'%lf\')"), iGPS->GetLng(), iGPS->GetLat());
			ExecuteScript2( strFunction );
		}		
	}

	return;
}

//================================================================================================= E. OPERATION
//================================================================================================= S. GET/SET

void CWBCMapView::setTextData( CWBCTextData* pSensorData )
{
	this->m_pTextData = pSensorData;

	return;
}

//================================================================================================= E. GET/SET
