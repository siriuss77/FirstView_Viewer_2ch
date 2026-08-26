//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//  Module: wbcUtil.h
//
//  Description: Viewer Utils
//
//  Update History
// 
//      2014.11.06 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. INCLUDE

#include "stdafx.h"
#include "wbcUtil.h"

//================================================================================================= E. INCLUDE

void vwutil_manGeometry( const ClGeometry& geoSource, DWORD dwMan, int width, int height, ClGeometry& rGeoMan )
{
    rGeoMan = geoSource;

    // if( dwMan & GEOMAN_LEFT ) no manipulation
    if( dwMan & GEOMAN_RIGHT )
        rGeoMan.x = width - geoSource.x;

    if( dwMan & GEOMAN_HCENTER )
        rGeoMan.x = width / 2 + geoSource.x;

    // if( dwMan & GEOMAN_TOP ) no manipulation

    if( dwMan & GEOMAN_BOTTOM )
        rGeoMan.y = height - geoSource.y;

    if( dwMan & GEOMAN_VCENTER )
        rGeoMan.y = height / 2 + geoSource.y;

    return;
}