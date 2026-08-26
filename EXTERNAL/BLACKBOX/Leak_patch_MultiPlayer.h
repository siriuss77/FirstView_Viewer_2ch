//=================================================================================================
// Copyright (c) Core Logic INC. All Rights Reserved.
//
// Use of this source code is subject to the terms of the Core Logic license agreement under
// which you licensed this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
//	Module: MultiPlayer.h
//
//	Description: Black box multiplayer
//
//	Update History
// 
//		2011.10.10 [Allen Han] Initial Release v1.0.00
//=================================================================================================
//================================================================================================= S. FILEID

#pragma once

//================================================================================================= E. FILEID
//================================================================================================= S. INCLUDE

#include <stdafx.h>

//================================================================================================= E. INCLUDE
//================================================================================================= S. PREPROCESSOR


//================================================================================================= E. PREPROCESSOR
//================================================================================================= S. TYPEDEF

//================================================================================================= E. TYPEDEF
//================================================================================================= S. CLASS

enum PlaybackState_
{
	PLAY_STATE_RUNNING = 0x00,
	PLAY_STATE_PAUSED,
	PLAY_STATE_STOPPED,
	PLAY_STATE_CLOSED,
};

class CDShowInterface;

class CMultiPlayer
{
public:
	// 출력 메인 윈도우 핸들
	int		m_nNumberOfWindows;
	HWND	m_hWindow1;
	HWND	m_hWindow2;

	// 화면 인터페이스 객체
	CDShowInterface* m_pInterface;

	// 현재 화면 분할 모드 저장 변수
	int		m_iMode;

	// 자동으로 크기 조절 여부 체크 변수
	BOOL	m_bAutoSetPrimary,m_bAutoSetSecondary;

	// 주 화면과 보조 화면을 교환해야하는지 체크 변수
	BOOL	m_bSwapDisplay;

	// 주화면 크기와 보조 화면 크기 저장 변수
	RECT	m_PrimarySize, m_SecondarySize;

	// 윈도우 화면 크기 저장 변수
	RECT	m_rtWindowSize1;
	RECT	m_rtWindowSize2;

	BOOL	m_bMainSwap;
	int		m_iViewMode;

	// Brightness Filter
	bool	m_bBrightnessFilter;
	bool	m_bAACPlaySkip;

public:
	CMultiPlayer(void);
	~CMultiPlayer(void);
	
	bool CheckAudioDecoderFilter(void);
	bool CheckBrightnessFilter(void);
	int CheckFilters(WCHAR* strPath);
	bool CheckSourceFilter(void);
	bool CheckVideoDecoderFilter(void);
	BOOL CloseFile();	//	파일 닫기
	void EnableBritness(bool bEnable);
	HRESULT	GetBritnessValue(int *nEffectNum, int *iEffectValue);
	BOOL GetCaptureToDib(DWORD nIndex, CBitmap& bitmapImage);
	LONGLONG GetCurrentPosition();	//	현재 재생 위치 얻어오기
	LONGLONG GetCurrentPositionLongLong();	//	현재 재생 위치 얻어오기
	LONGLONG GetDuration();		//	총 재생 크기 얻어오기
	LONGLONG GetDurationLongLong();		//	총 재생 크기 얻어오기
	int GetState(void);
	int GetStreamCount(void);	
	HRESULT GetTextData(unsigned char * pStr, int * size, unsigned long * cts);
	HRESULT GoStep(DWORD dwFrames);
	HRESULT GetVideoFrameCount(int nTrack, int * nCount);
	void Initialize(int nNumberOfWindows, HWND hWnd1, HWND hWnd2=NULL);	// 플레이어 초기화
	bool IsEnableBritness();	
	BOOL OnDraw(HDC hDC = NULL);	// 화면 갱신시 Repaint 호출
	BOOL OnDraw( CDC *pDC );		// 화면 갱신시 Repaint 호출
	BOOL OnSize(RECT& rt);			// 윈도우 크기 변경시 해당값으로 변경
	BOOL OnSize();					// 윈도우 크기 변경시 해당값으로 변경
	BOOL OpenFile( const WCHAR* szFilename);	//	파일 열기
	bool RegistAudioDecodeFilter(CString strPath);
	bool RegistBrightnessFilter(CString strPath);
	bool RegistSourceFilter(CString strPath);
	bool RegistVideoDecoderFilter(CString strPath);
	void Release(void);		// 플레이어 릴리즈
	void SetAACPlaySkip(bool bAACPlaySkip);
	HRESULT SetBrightness(int nStream, int iValue);
	HRESULT	SetBrightnessValue(int nEffectNum, int iEffectValue);
	HRESULT SetFlip(int nStream, bool bType);
	HRESULT SetMirror(int nStream, bool bType);
	BOOL SetPlaySpeed( float fSpeed );		// 재생 속도 조절
	BOOL SetPosition( long lPosition );		//	재생 위치 변경하기
	bool SetPositionLongLong( LONGLONG lPosition );
	BOOL SetVolume( int iVolume );			//	볼륨 크기 변경하기
	BOOL StreamBackward( long double iTime = 5);	//	Tick(초) 만큼 뒤로 이동
	BOOL StreamForward( long double iTime = 5 );	//	Tick(초) 만큼 앞으로 이동
	BOOL StreamPause(void);					//	일시 정지
	BOOL StreamPlay(void);					//	재생
	BOOL StreamStop(void);					// 정지
	BOOL UpdatePosition(BOOL bAutoCheck = TRUE, BOOL bSetWindowSize = FALSE);	// 변경된 위치와 크기에 대한값으로 갱신
	bool UpdateWindow(void);
};

//================================================================================================= S. CLASS
