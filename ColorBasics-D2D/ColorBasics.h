//------------------------------------------------------------------------------
// <copyright file="ColorBasics.h" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "resource.h"
#include "ImageRenderer.h"
#include "Semaphore.h"							// 手旗信号判定クラス
#include "SemaphoreGame.h"						// 手旗ゲーム

class CColorBasics
{
    static const int        cColorWidth  = 1920;
    static const int        cColorHeight = 1080;

	//static const int        cDepthWidth = 640;
	//static const int        cDepthHeight = 360;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    CColorBasics();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CColorBasics();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="nCmdShow"></param>
    int                     Run(HINSTANCE hInstance, int nCmdShow);

private:
    HWND                    m_hWnd;
    INT64                   m_nStartTime;
    INT64                   m_nLastCounter;
    double                  m_fFreq;
    DWORD                   m_nNextStatusTime;
    DWORD                   m_nFramesSinceUpdate;
    bool                    m_bSaveScreenshot;

	bool					m_bOpening;		// 手旗オープニングタイトルフラグ true 実行中/false 待機中
	bool					m_bStart;		// 手旗処理実行フラグ true 実行中/false 待機中

	INT64					m_nDetectCount;	//
	int						m_nCommandIndex;

    // Current Kinect
    IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

    // Color reader
    IColorFrameReader*      m_pColorFrameReader;
	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;

    // Direct2D
    ImageRenderer*          m_pDrawColor;
    ID2D1Factory*           m_pD2DFactory;
    RGBQUAD*                m_pColorRGBX;
	RGBQUAD*                m_pBackgroundRGBX;

	//
	Semaphore*				m_pSemaphore[2] ;	// 二人対戦を想定
	bool					m_bSemaphore;		// 手旗処理実行フラグ true 実行中/false 待機中
	int						m_nButton;
	INT64                   m_nSemaphoreStartTime;

	// 手旗ゲーム処理
	CSemaphoreGame*			m_pGame;

    /// <summary>
    /// Main processing function
    /// </summary>
    void                    Update();
    void                    UpdateBody();

	// オープニング
	void					Opening();
	// お題表示
	bool					Question(INT64 nTime);
	void					Detect(IBody* pBody);
    /// <summary>
    /// Initializes the default Kinect sensor
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 InitializeDefaultSensor();

    /// <summary>
    /// Handle new color data
    /// <param name="nTime">timestamp of frame</param>
    /// <param name="pBuffer">pointer to frame data</param>
    /// <param name="nWidth">width (in pixels) of input image data</param>
    /// <param name="nHeight">height (in pixels) of input image data</param>
    /// </summary>
    void                    ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight);
	/// <summary>
	/// Handle new body data
	/// <param name="nTime">timestamp of frame</param>
	/// <param name="nBodyCount">body data count</param>
	/// <param name="ppBodies">body data in frame</param>
	/// </summary>
	void                    ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);
	/// <summary>
	/// Converts a body point to screen space
	/// </summary>
	/// <param name="bodyPoint">body point to tranform</param>
	/// <param name="width">width (in pixels) of output buffer</param>
	/// <param name="height">height (in pixels) of output buffer</param>
	/// <returns>point in screen-space</returns>
	//D2D1_POINT_2F           BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);
	D2D1_POINT_2F           BodyToScreen(const CameraSpacePoint& bodyPoint);

    /// <summary>
    /// Set the status bar message
    /// </summary>
    /// <param name="szMessage">message to display</param>
    /// <param name="nShowTimeMsec">time in milliseconds for which to ignore future status messages</param>
    /// <param name="bForce">force status update</param>
    bool                    SetStatusMessage(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);

    /// <summary>
    /// Get the name of the file where screenshot will be stored.
    /// </summary>
    /// <param name="lpszFilePath">string buffer that will receive screenshot file name.</param>
    /// <param name="nFilePathSize">number of characters in lpszFilePath string buffer.</param>
    /// <returns>
    /// S_OK on success, otherwise failure code.
    /// </returns>
    HRESULT                 GetScreenshotFileName(LPWSTR lpszFilePath, UINT nFilePathSize);

    /// <summary>
    /// Save passed in image data to disk as a bitmap
    /// </summary>
    /// <param name="pBitmapBits">image data to save</param>
    /// <param name="lWidth">width (in pixels) of input image data</param>
    /// <param name="lHeight">height (in pixels) of input image data</param>
    /// <param name="wBitsPerPixel">bits per pixel of image data</param>
    /// <param name="lpszFilePath">full file path to output bitmap to</param>
    /// <returns>indicates success or failure</returns>
    HRESULT                 SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCWSTR lpszFilePath);
	/// <summary>
	/// Load an image from a resource into a buffer
	/// </summary>
	/// <param name="resourceName">name of image resource to load</param>
	/// <param name="resourceType">type of resource to load</param>
	/// <param name="nOutputWidth">width (in pixels) of scaled output bitmap</param>
	/// <param name="nOutputHeight">height (in pixels) of scaled output bitmap</param>
	/// <param name="pOutputBuffer">buffer that will hold the loaded image</param>
	/// <returns>S_OK on success, otherwise failure code</returns>
	HRESULT                 LoadResourceImage(PCWSTR resourceName, PCWSTR resourceType, UINT nOutputWidth, UINT nOutputHeight, RGBQUAD* pOutputBuffer);

	/// <summary>
	/// 手旗処理
	/// </summary>

};

