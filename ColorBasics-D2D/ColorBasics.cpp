//------------------------------------------------------------------------------
// <copyright file="ColorBasics.cpp" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <strsafe.h>
#include "resource.h"
#include <Wincodec.h>
#include "ColorBasics.h"
#include <math.h>

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// timer
#define IDT_KTIMER_1	1000

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CColorBasics application;
    application.Run(hInstance, nCmdShow);
}

/// <summary>
/// Constructor
/// </summary>
CColorBasics::CColorBasics() :
	m_hWnd(NULL),
	m_nStartTime(0),
	m_nLastCounter(0),
	m_nFramesSinceUpdate(0),
	m_fFreq(0),
	m_nNextStatusTime(0),
	m_bSaveScreenshot(false),
	m_pKinectSensor(NULL),
	m_pCoordinateMapper(NULL),
	m_pBodyFrameReader(NULL),
	m_pColorFrameReader(NULL),
	m_pD2DFactory(NULL),
	m_pDrawColor(NULL),
	m_pColorRGBX(NULL),
	m_pBackgroundRGBX(NULL),
	m_bSemaphore(false),
	m_nSemaphoreStartTime(0),
	m_bStart(false),
	m_bOpening(true),
	m_nButton(0),
	m_nDetectCount(0),
	m_nCommandIndex(0),
	m_pGame(NULL)
{
	LARGE_INTEGER qpf = {0};
	// 高分解能カウンターの周波数
	// 1秒あたりのカウント数
	if (QueryPerformanceFrequency(&qpf))
	{
		m_fFreq = double(qpf.QuadPart);
	}

	// create heap storage for color pixel data in RGBX format
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	// create heap storage for background image pixel data in RGBX format
	m_pBackgroundRGBX = new RGBQUAD[cColorWidth * cColorHeight];

	m_pSemaphore[0] = NULL;
	m_pSemaphore[1] = NULL;
}

/// <summary>
/// Destructor
/// </summary>
CColorBasics::~CColorBasics()
{
	// clean up Direct2D renderer
	if (m_pDrawColor)
	{
		delete m_pDrawColor;
		m_pDrawColor = NULL;
	}

	if (m_pColorRGBX)
	{
		delete [] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}
	if (m_pBackgroundRGBX)
	{
		delete[] m_pBackgroundRGBX;
		m_pBackgroundRGBX = NULL;
	}

	// clean up Direct2D
	SafeRelease(m_pD2DFactory);

	// done with color frame reader
	SafeRelease(m_pColorFrameReader);
	// done with body frame reader
	SafeRelease(m_pBodyFrameReader);
	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);

	if (m_pSemaphore[0]){ delete m_pSemaphore[0]; }
	if (m_pSemaphore[1]){ delete m_pSemaphore[1]; }

	if (m_pGame){ delete m_pGame; }
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CColorBasics::Run(HINSTANCE hInstance, int nCmdShow)
{
	if (m_pBackgroundRGBX)
	{
		//if (FAILED(LoadResourceImage(L"Background", L"Image", cDepthWidth, cDepthHeight, m_pBackgroundRGBX)))
		//if (FAILED(LoadResourceImage(MAKEINTRESOURCE(IDB_BITMAP1), RT_BITMAP, cDepthWidth, cDepthHeight, m_pBackgroundRGBX)))
		if (FAILED(LoadResourceImage(MAKEINTRESOURCE(IDB_PNG1), L"Png", cColorWidth, cColorHeight, m_pBackgroundRGBX)))
		{
			delete[] m_pBackgroundRGBX;
			m_pBackgroundRGBX = NULL;
		}
	}

	MSG       msg = { 0 };
	WNDCLASS  wc;

	// Dialog custom window class
	ZeroMemory(&wc, sizeof(wc));
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.cbWndExtra    = DLGWINDOWEXTRA;
	wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
	wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
	wc.lpfnWndProc   = DefDlgProcW;
	wc.lpszClassName = L"ColorBasicsAppDlgWndClass";

	if (!RegisterClassW(&wc))
	{
		return 0;
	}

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        NULL,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CColorBasics::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

	// Show window
	ShowWindow(hWndApp, nCmdShow);

	// Timer
	SetTimer(hWndApp, IDT_KTIMER_1, 50, (TIMERPROC)NULL);

	// Main message loop
	while (WM_QUIT != msg.message)
	{
		// 2014/02/20 Wada カラーとボディを別々に描画させたら
		// やはり、ちらちらしてだめだった。FPSも落ちた。
		//UpdateBody();
		if (m_bOpening){ Opening(); }
		//else{ Update(); }

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If a dialog message will be taken care of by the dialog proc
			if (hWndApp && IsDialogMessageW(hWndApp, &msg))
			{
				continue;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

/// <summary>
/// Main processing function
/// </summary>
// この関数がループ処理される
void CColorBasics::Update()
{
	HRESULT hr = NULL;
	// (1) カラーフレーム（背景描画のみに使用）
	if (m_pColorFrameReader)
	{

		IColorFrame* pColorFrame = NULL;

		hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);

		if (SUCCEEDED(hr))
		{
			INT64 nTime = 0;
			IFrameDescription* pFrameDescription = NULL;
			int nWidth = 0;
			int nHeight = 0;
			ColorImageFormat imageFormat = ColorImageFormat_None;
			UINT nBufferSize = 0;
			RGBQUAD *pBuffer = NULL;

			hr = pColorFrame->get_RelativeTime(&nTime);

			if (SUCCEEDED(hr))
			{
				hr = pColorFrame->get_FrameDescription(&pFrameDescription);
			}

			if (SUCCEEDED(hr))
			{
				hr = pFrameDescription->get_Width(&nWidth);
			}

			if (SUCCEEDED(hr))
			{
				hr = pFrameDescription->get_Height(&nHeight);
			}

			if (SUCCEEDED(hr))
			{
				hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
			}

			if (SUCCEEDED(hr))
			{
				if (imageFormat == ColorImageFormat_Bgra)
				{
					hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
				}
				else if (m_pColorRGBX)
				{
					pBuffer = m_pColorRGBX;
					nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
					hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);
				}
				else
				{
					hr = E_FAIL;
				}
			}

			if (SUCCEEDED(hr))
			{
				ProcessColor(nTime, pBuffer, nWidth, nHeight);
			}
			SafeRelease(pFrameDescription);
		}
		SafeRelease(pColorFrame);
	}


	// これ以降にBody処理を実装
	TIMESPAN nBodyTime = 0;
	if (m_pBodyFrameReader)
	{
		IBodyFrame* pBodyFrame = NULL;

		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->get_RelativeTime(&nBodyTime);

			// ここに、UI描画処理 ボタン等
			// ゲームのステータスによって描画を設定
			if (!m_pGame){ m_pGame = new CSemaphoreGame(m_pDrawColor, m_pCoordinateMapper); }
			if (m_pGame){ m_pGame->Display(nBodyTime); }

			IBody* ppBodies[BODY_COUNT] = { 0 };

			if (SUCCEEDED(hr))
			{
				hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
			}

			if (SUCCEEDED(hr))
			{
				// ここに、UI処理->ProcessBody()内でOK
				// ボディデータにてUI処理を行う
				// ステータスの変更
				if (m_pGame){ m_pGame->Play(nBodyTime, BODY_COUNT, ppBodies); }

				ProcessBody(nBodyTime, BODY_COUNT, ppBodies);
			}

			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				SafeRelease(ppBodies[i]);
			}
		}
		SafeRelease(pBodyFrame);
	}

	return;
}

// テスト用 ボディのみの関数
void CColorBasics::UpdateBody()
{

	// これ以降にBody処理を実装
	if (!m_pBodyFrameReader){ return; }

	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}

	SafeRelease(pBodyFrame);

}

void CColorBasics::Opening()
{
	// Draw the data with Direct2D
	m_pDrawColor->Draw(reinterpret_cast<BYTE*>(m_pBackgroundRGBX), cColorWidth * cColorHeight * sizeof(RGBQUAD));
	if (m_bStart)
	{
		if (m_pDrawColor->DrawOpeningTitle()){ m_bOpening = false; }
	}
	m_pDrawColor->EndDraw();

	return;
}

// お題表示
bool CColorBasics::Question(INT64 nTime)
{
	bool bQuit = false;
	if (!m_bSemaphore){ return bQuit; }
	// 一定時間でどうにかしようと思ったけど現状は未使用
	if (!m_nSemaphoreStartTime){ m_nSemaphoreStartTime = nTime; }

	// このお題の文字もランダムおよびパターンを考慮したものにしたい。
	// 文字を直接ではなくパターンにて指定に
	wchar_t szQuestion[] = L"アイウエオ";
	//wchar_t szQuestion[] = L"ココ";
	int nSize = 5;
	m_pDrawColor->DrawQuestion(szQuestion);

	if (m_pSemaphore[0] && m_pSemaphore[1])
	{
		// ここで、判定？
		if (m_pSemaphore[0]->Judge(szQuestion, nSize))
		{
			m_pSemaphore[0]->Win(m_pDrawColor);
			bQuit = true;
		}
		else if (m_pSemaphore[1]->Judge(szQuestion, nSize))
		{
			m_pSemaphore[1]->Win(m_pDrawColor);
			bQuit = true;
		}
	}
	else if (m_pSemaphore[0])
	{
		// ここで、判定？
		if (m_pSemaphore[0]->Judge(szQuestion, nSize))
		{
			m_pSemaphore[0]->Win(m_pDrawColor);
			bQuit = true;
		}
	}

	return bQuit;
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CColorBasics::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CColorBasics* pThis = NULL;
    
	if (WM_INITDIALOG == uMsg)
	{
		pThis = reinterpret_cast<CColorBasics*>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CColorBasics*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis)
	{
		return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CColorBasics::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
		case WM_INITDIALOG:
		{
			// Bind application window handle
			m_hWnd = hWnd;

			// Init Direct2D
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

			// Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
			// We'll use this to draw the data we receive from the Kinect to the screen
			m_pDrawColor = new ImageRenderer();
			HRESULT hr = m_pDrawColor->Initialize(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), m_pD2DFactory, cColorWidth, cColorHeight, cColorWidth * sizeof(RGBQUAD)); 
			if (FAILED(hr))
			{
				SetStatusMessage(L"Failed to initialize the Direct2D draw device.", 10000, true);
			}

			// Get and initialize the default Kinect sensor
			InitializeDefaultSensor();
		}
		break;

		// If the titlebar X is clicked, destroy app
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			KillTimer(m_hWnd, IDT_KTIMER_1);
			// Quit the main message pump
			PostQuitMessage(0);
			break;

		// Handle button press
		case WM_COMMAND:
			// If it was for the screenshot control and a button clicked event, save a screenshot next frame 
			if (IDC_BUTTON_SCREENSHOT == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
			{
				m_bSaveScreenshot = true;
			}
			break;
		case WM_SEMAPHORE:
			// ここに、手旗処理開始キック処理を追加する。
			// PostMessageを受け取る
			m_bSemaphore = !m_bSemaphore;
			break;
		case WM_RBUTTONDOWN:
			// スタート
			m_bStart = true;
			break;
		case WM_LBUTTONDOWN:
			// オープニングの一回だけ
			m_bOpening = false;
			break;
		case WM_TIMER:
			switch (wParam)
			{
			case IDT_KTIMER_1:
				if (!m_bOpening){ Update(); }
				break;
			}
			break;
	}

	return FALSE;
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CColorBasics::InitializeDefaultSensor()
{
    HRESULT hr;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        return hr;
    }

    if (m_pKinectSensor)
    {
        // Initialize the Kinect and get the color reader
        IColorFrameSource* pColorFrameSource = NULL;
		IBodyFrameSource* pBodyFrameSource = NULL;

        hr = m_pKinectSensor->Open();

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
        }
        if (SUCCEEDED(hr))
        {
            hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
        }

		// Body
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		SafeRelease(pBodyFrameSource);
		SafeRelease(pColorFrameSource);
    }

    if (!m_pKinectSensor || FAILED(hr))
    {
        SetStatusMessage(L"No ready Kinect found!", 10000, true);
        return E_FAIL;
    }

    return hr;
}

/// <summary>
/// Handle new color data
/// <param name="nTime">timestamp of frame</param>
/// <param name="pBuffer">pointer to frame data</param>
/// <param name="nWidth">width (in pixels) of input image data</param>
/// <param name="nHeight">height (in pixels) of input image data</param>
/// </summary>
void CColorBasics::ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight) 
{
    if (m_hWnd)
    {
        if (!m_nStartTime)
        {
            m_nStartTime = nTime;
        }

        double fps = 0.0;

		// LARGE_INTEGER union 中身はLONGLONG
        LARGE_INTEGER qpcNow = {0};
		// m_fFreq には1秒あたりのカウント数（周波数）を保存
        if (m_fFreq)
        {
			// とにかく、パソコンの高分解能カウンターらしい
            if (QueryPerformanceCounter(&qpcNow))
            {
                if (m_nLastCounter)
                {
                    m_nFramesSinceUpdate++;
                    fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
                }
            }
        }

        WCHAR szStatusMessage[64];
        StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));

        if (SetStatusMessage(szStatusMessage, 1000, false))
        {
            m_nLastCounter = qpcNow.QuadPart;
            m_nFramesSinceUpdate = 0;
        }
    }

    // Make sure we've received valid data
    if (pBuffer && (nWidth == cColorWidth) && (nHeight == cColorHeight))
    {
        // Draw the data with Direct2D
        m_pDrawColor->Draw(reinterpret_cast<BYTE*>(pBuffer), cColorWidth * cColorHeight * sizeof(RGBQUAD));
		//m_pDrawColor->EndDraw();

		// スクリーンキャプチャ処理
        if (m_bSaveScreenshot)
        {
            WCHAR szScreenshotPath[MAX_PATH];

            // Retrieve the path to My Photos
            GetScreenshotFileName(szScreenshotPath, _countof(szScreenshotPath));

            // Write out the bitmap to disk
            HRESULT hr = SaveBitmapToFile(reinterpret_cast<BYTE*>(pBuffer), nWidth, nHeight, sizeof(RGBQUAD) * 8, szScreenshotPath);

            WCHAR szStatusMessage[64 + MAX_PATH];
            if (SUCCEEDED(hr))
            {
                // Set the status bar to show where the screenshot was saved
                StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"Screenshot saved to %s", szScreenshotPath);
            }
            else
            {
                StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"Failed to write screenshot to %s", szScreenshotPath);
            }

            SetStatusMessage(szStatusMessage, 5000, true);

            // toggle off so we don't save a screenshot again next frame
            m_bSaveScreenshot = false;
        }
    }
}

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void CColorBasics::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
	if (m_hWnd)
	{
		HRESULT hr = S_OK;

		D2D1_POINT_2F start;
		start.x = 1500.0;
		start.y = 800.0;

		D2D1_POINT_2F quit;
		quit.x = 300.0;
		quit.y = 800.0;

		//int width = 0;
		//int height = 0;
		if (SUCCEEDED(hr) && m_pCoordinateMapper)
		{
			// 先に実行しているProcessColor()にて行っているのでコメント
			//hr = m_pDrawColor->BeginDraw();

			DetectionResult nEngaged[6] = { DetectionResult_Unknown };
			PointF lean;

			//RECT rct;
			//GetClientRect(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), &rct);
			//width = rct.right;
			//height = rct.bottom;

			UINT64 nTrackBody = 10;

			for (int i = 0; i < nBodyCount; ++i)
			{
				IBody* pBody = ppBodies[i];
				if (pBody)
				{
					// 手旗二人での対戦モードを想定してインデックスを取得する。
					// 本来はゲーム前に対戦の二人ｗフィックスしておくべきだろうが。
					// 
					// トラッキングされているボディかはちゃんと確かめること。
					BOOLEAN bTracked = false;
					hr = pBody->get_IsTracked(&bTracked);

					// Engaged()は使えるみたい。これは、視野に入ってきた人を認識するものだろう。
					hr = pBody->get_Engaged(&nEngaged[i]);
					pBody->get_Lean(&lean);
					// 以下はまだ使えないようだ
					//hr = pBody->GetAppearanceDetectionResults((UINT)i, &nEngaged[i]);

					if (SUCCEEDED(hr) && bTracked)
					{
						// トラッキングが無効な場合のインデックスは０が返るので使い方に注意！！
						UINT64 nBodyIndex = 0;
						hr = pBody->get_TrackingId(&nBodyIndex);

						Joint joints[JointType_Count];
						D2D1_POINT_2F jointPoints[JointType_Count];
						HandState leftHandState = HandState_Unknown;
						HandState rightHandState = HandState_Unknown;

						pBody->get_HandLeftState(&leftHandState);
						pBody->get_HandRightState(&rightHandState);

						hr = pBody->GetJoints(_countof(joints), joints);
						if (SUCCEEDED(hr))
						{
							// スクリーン座標に変換
							for (int j = 0; j < _countof(joints); ++j)
							{
								jointPoints[j] = BodyToScreen(joints[j].Position);
							}
							// ここに頭部に丸を描いて、ボディ番号を表示
							m_pDrawColor->DrawHead(jointPoints[JointType_Head], i, nEngaged[i], lean);

							// 手先がある領域にきたら実行
							// ボタンのような
							// 現状、複数人が認識されても実行するので、本来は最初に認識された一人のみにする必要がある。
							float xy[2] = { 0.0 };

							if (!m_bSemaphore)
							{
								if (m_pSemaphore[0])
								{
									delete m_pSemaphore[0];
									m_pSemaphore[0] = NULL;
								}
								if (m_pSemaphore[1])
								{
									delete m_pSemaphore[1];
									m_pSemaphore[1] = NULL;
								}
								m_nButton = 1;
								xy[0] = jointPoints[JointType_HandTipRight].x - start.x;
								xy[1] = jointPoints[JointType_HandTipRight].y - start.y;
								if (sqrt( xy[0]*xy[0] + xy[1]*xy[1] ) < 100.0 )
								{
									if (nTrackBody == 10 || nTrackBody == nBodyIndex)
									{
										m_nButton = 0;
										nTrackBody = nBodyIndex;
									}
								}
							}
							else
							{
								// 手旗スタート
								// 手旗判定
								if (m_pSemaphore[0] == NULL)
								{
									m_pSemaphore[0] = new Semaphore( &nBodyIndex );
								}
								else
								{
									if (m_pSemaphore[1] == NULL && !m_pSemaphore[0]->ItsMe(&nBodyIndex))
									{
										m_pSemaphore[1] = new Semaphore(&nBodyIndex);
									}
								}

								// カウント
								// 基本ポーズでのデータ取得
								// 手旗本番処理
								// 手旗の判定に画像と同等のフレームは必要はないのでは。
								// タイマーでBodyフレームを取得し、それで手旗判定を行う。
								if (m_pSemaphore[0])
								{
									m_pSemaphore[0]->SetSignalType(&nBodyIndex, jointPoints, m_pDrawColor);
								}
								if (m_pSemaphore[1])
								{
									m_pSemaphore[1]->SetSignalType(&nBodyIndex, jointPoints, m_pDrawColor);
								}
								//m_pSemaphore[0]->Practice(nTime, jointPoints, m_pDrawColor);

								// quitボタン処理
								m_nButton = 2;
								// 基本ポーズ用の表示
								xy[0] = jointPoints[JointType_HandTipLeft].x - quit.x;
								xy[1] = jointPoints[JointType_HandTipLeft].y - quit.y;
								if (sqrt( xy[0]*xy[0] + xy[1]*xy[1] ) < 100.0 )
								{
									if (nTrackBody == 10 || nTrackBody == nBodyIndex)
									{
										m_nButton = 0;
										nTrackBody = nBodyIndex;
									}
								}
							}
							m_pDrawColor->DrawBody(joints, jointPoints);
							//m_pDrawColor->DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
							//m_pDrawColor->DrawHand(rightHandState, jointPoints[JointType_HandRight]);

							Detect(pBody);
							//break;
						}
					}
				}
			}
			if (!m_bSemaphore)
			{
				// このボタン処理でウィンドウにメッセージを送っている
				m_pDrawColor->DrawButton(start, m_nButton);
			}
			else
			{
				m_pDrawColor->DrawButton(quit, m_nButton);
			}
			// 二人対戦モードのお題表示
			if (Question(nTime))
			{
				m_pDrawColor->DrawButton(quit, 0);
			}

			m_pDrawColor->EndDraw();
		}
	}
}

void CColorBasics::Detect(IBody* pBody)
{
	if (pBody == NULL){ return; }

	Joint joints[JointType_Count];
	D2D1_POINT_2F pos;
	HandState leftHandState = HandState_Unknown;
	HandState rightHandState = HandState_Unknown;

	pBody->get_HandLeftState(&leftHandState);
	pBody->get_HandRightState(&rightHandState);

	HRESULT hr = pBody->GetJoints(_countof(joints), joints);
	if (SUCCEEDED(hr))
	{
		Joint hand = joints[JointType_HandLeft];
		Joint Shoulder = joints[JointType_ShoulderLeft];
		if (leftHandState == HandState_Open &&
			(Shoulder.Position.Z-hand.Position.Z) > 0.4)
		{
			pos = BodyToScreen(hand.Position);
			m_pDrawColor->DrawHand(joints, pos);

			if (++m_nDetectCount > 20)
			{
				// 
				Joint thumb = joints[JointType_ThumbLeft];
				if (fabs(thumb.Position.X - hand.Position.X) < 0.01)
				{
					m_nCommandIndex++;
					if (m_nCommandIndex>3){ m_nCommandIndex = 0; }
				}
				m_pDrawColor->DrawCommand(pos, m_nCommandIndex);
			}
		}
		else
		{
			m_nDetectCount = 0;
		}
	}

	return;
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
/// <param name="showTimeMsec">time in milliseconds to ignore future status messages</param>
/// <param name="bForce">force status update</param>
bool CColorBasics::SetStatusMessage(WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce)
{
    DWORD now = GetTickCount();

    if (m_hWnd && (bForce || (m_nNextStatusTime <= now)))
    {
        SetDlgItemText(m_hWnd, IDC_STATUS, szMessage);
        m_nNextStatusTime = now + nShowTimeMsec;

        return true;
    }

    return false;
}

/// <summary>
/// Converts a body point to screen space
/// </summary>
/// <param name="bodyPoint">body point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F CColorBasics::BodyToScreen(const CameraSpacePoint& bodyPoint)
{
	// Calculate the body's position on the screen
	//DepthSpacePoint depthPoint = { 0 };
	//m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
	// カラーフレームにマッピングする
	ColorSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToColorSpace(bodyPoint, &depthPoint);

	// マッピングしたらそのままの座標で
	//float screenPointX = static_cast<float>(depthPoint.X * width) / cColorWidth;
	//float screenPointY = static_cast<float>(depthPoint.Y * height) / cColorHeight;
	float screenPointX = static_cast<float>(depthPoint.X);
	float screenPointY = static_cast<float>(depthPoint.Y);

	return D2D1::Point2F(screenPointX, screenPointY);
}

/// <summary>
/// Get the name of the file where screenshot will be stored.
/// </summary>
/// <param name="lpszFilePath">string buffer that will receive screenshot file name.</param>
/// <param name="nFilePathSize">number of characters in lpszFilePath string buffer.</param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CColorBasics::GetScreenshotFileName(LPWSTR lpszFilePath, UINT nFilePathSize)
{
    WCHAR* pszKnownPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &pszKnownPath);

    if (SUCCEEDED(hr))
    {
        // Get the time
        WCHAR szTimeString[MAX_PATH];
        GetTimeFormatEx(NULL, 0, NULL, L"hh'-'mm'-'ss", szTimeString, _countof(szTimeString));

        // File name will be KinectScreenshotColor-HH-MM-SS.bmp
        StringCchPrintfW(lpszFilePath, nFilePathSize, L"%s\\KinectScreenshot-Color-%s.bmp", pszKnownPath, szTimeString);
    }

    if (pszKnownPath)
    {
        CoTaskMemFree(pszKnownPath);
    }

    return hr;
}

/// <summary>
/// Save passed in image data to disk as a bitmap
/// </summary>
/// <param name="pBitmapBits">image data to save</param>
/// <param name="lWidth">width (in pixels) of input image data</param>
/// <param name="lHeight">height (in pixels) of input image data</param>
/// <param name="wBitsPerPixel">bits per pixel of image data</param>
/// <param name="lpszFilePath">full file path to output bitmap to</param>
/// <returns>indicates success or failure</returns>
HRESULT CColorBasics::SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCWSTR lpszFilePath)
{
    DWORD dwByteCount = lWidth * lHeight * (wBitsPerPixel / 8);

    BITMAPINFOHEADER bmpInfoHeader = {0};

    bmpInfoHeader.biSize        = sizeof(BITMAPINFOHEADER);  // Size of the header
    bmpInfoHeader.biBitCount    = wBitsPerPixel;             // Bit count
    bmpInfoHeader.biCompression = BI_RGB;                    // Standard RGB, no compression
    bmpInfoHeader.biWidth       = lWidth;                    // Width in pixels
    bmpInfoHeader.biHeight      = -lHeight;                  // Height in pixels, negative indicates it's stored right-side-up
    bmpInfoHeader.biPlanes      = 1;                         // Default
    bmpInfoHeader.biSizeImage   = dwByteCount;               // Image size in bytes

    BITMAPFILEHEADER bfh = {0};

    bfh.bfType    = 0x4D42;                                           // 'M''B', indicates bitmap
    bfh.bfOffBits = bmpInfoHeader.biSize + sizeof(BITMAPFILEHEADER);  // Offset to the start of pixel data
    bfh.bfSize    = bfh.bfOffBits + bmpInfoHeader.biSizeImage;        // Size of image + headers

    // Create the file on disk to write to
    HANDLE hFile = CreateFileW(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Return if error opening file
    if (NULL == hFile) 
    {
        return E_ACCESSDENIED;
    }

    DWORD dwBytesWritten = 0;
    
    // Write the bitmap file header
    if (!WriteFile(hFile, &bfh, sizeof(bfh), &dwBytesWritten, NULL))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }
    
    // Write the bitmap info header
    if (!WriteFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwBytesWritten, NULL))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }
    
    // Write the RGB Data
    if (!WriteFile(hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwBytesWritten, NULL))
    {
        CloseHandle(hFile);
        return E_FAIL;
    }    

    // Close the file
    CloseHandle(hFile);
    return S_OK;
}

/// <summary>
/// Load an image from a resource into a buffer
/// </summary>
/// <param name="resourceName">name of image resource to load</param>
/// <param name="resourceType">type of resource to load</param>
/// <param name="nOutputWidth">width (in pixels) of scaled output bitmap</param>
/// <param name="nOutputHeight">height (in pixels) of scaled output bitmap</param>
/// <param name="pOutputBuffer">buffer that will hold the loaded image</param>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT CColorBasics::LoadResourceImage(PCWSTR resourceName, PCWSTR resourceType, UINT nOutputWidth, UINT nOutputHeight, RGBQUAD* pOutputBuffer)
{
	IWICImagingFactory* pIWICFactory = NULL;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;

	HRESULT hrCoInit = CoInitialize(NULL);
	HRESULT hr = hrCoInit;

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWICFactory);
	}

	if (SUCCEEDED(hr))
	{
		// Locate the resource
		imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);
		hr = imageResHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Load the resource
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);
		hr = pImageFile ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateBitmapScaler(&pScaler);
	}

	if (SUCCEEDED(hr))
	{
		hr = pScaler->Initialize(
			pSource,
			nOutputWidth,
			nOutputHeight,
			WICBitmapInterpolationModeCubic
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}

	UINT width = 0;
	UINT height = 0;
	if (SUCCEEDED(hr))
	{
		hr = pConverter->GetSize(&width, &height);
	}

	// make sure the image scaled correctly so the output buffer is big enough
	if (SUCCEEDED(hr))
	{
		if ((width != nOutputWidth) || (height != nOutputHeight))
		{
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->CopyPixels(NULL, width * sizeof(RGBQUAD), nOutputWidth * nOutputHeight * sizeof(RGBQUAD), reinterpret_cast<BYTE*>(pOutputBuffer));
	}

	SafeRelease(pScaler);
	SafeRelease(pConverter);
	SafeRelease(pSource);
	SafeRelease(pDecoder);
	SafeRelease(pStream);
	SafeRelease(pIWICFactory);

	if (SUCCEEDED(hrCoInit))
	{
		CoUninitialize();
	}

	return hr;
}
