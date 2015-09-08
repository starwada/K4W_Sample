//------------------------------------------------------------------------------
// <copyright file="ImageRenderer.h" company="Microsoft">
//     The Kinect for Windows APIs used here are preliminary and subject to change
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the drawing of image data
// 2014/1/28 Wada
// このサンプルにBody描画を追加する。

#pragma once

#include <d2d1.h>
//
#include <dwrite.h>

#define WM_SEMAPHORE	(WM_USER+1)

class ImageRenderer
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    ImageRenderer();

    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~ImageRenderer();

    /// <summary>
    /// Set the window to draw to as well as the video format
    /// Implied bits per pixel is 32
    /// </summary>
    /// <param name="hWnd">window to draw to</param>
    /// <param name="pD2DFactory">already created D2D factory object</param>
    /// <param name="sourceWidth">width (in pixels) of image data to be drawn</param>
    /// <param name="sourceHeight">height (in pixels) of image data to be drawn</param>
    /// <param name="sourceStride">length (in bytes) of a single scanline</param>
    /// <returns>indicates success or failure</returns>
    HRESULT Initialize(HWND hwnd, ID2D1Factory* pD2DFactory, int sourceWidth, int sourceHeight, int sourceStride);

    /// <summary>
    /// Draws a 32 bit per pixel image of previously specified width, height, and stride to the associated hwnd
    /// </summary>
    /// <param name="pImage">image data in RGBX format</param>
    /// <param name="cbImage">size of image data in bytes</param>
    /// <returns>indicates success or failure</returns>
    HRESULT Draw(BYTE* pImage, unsigned long cbImage);
	/// <summary>
	/// Draws a body 
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	void                    DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);
	/// <summary>
	/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
	/// </summary>
	/// <param name="handState">state of the hand</param>
	/// <param name="handPosition">position of the hand</param>
	void                    DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
	void                    DrawHand(const Joint* pJoint, const D2D1_POINT_2F& handPosition);

	/// <summary>
	/// Draws one bone of a body (joint to joint)
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="joint0">one joint of the bone to draw</param>
	/// <param name="joint1">other joint of the bone to draw</param>
	void                    DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);

	void					DrawHead(const D2D1_POINT_2F& headPosition, int bodycount, DetectionResult nResult, PointF lean);

	void					DrawHit();

	void					HitButton();

	void					DrawButton(const D2D1_POINT_2F& center, int nMode);
	void					DrawButton(const D2D1_POINT_2F& center, const wchar_t* pCaption);

	void					DrawSemaphore(INT_PTR* pType, INT64 nCount);
	void					DrawGraph(INT_PTR* pType, int nCount);

	void					DrawMessage(const wchar_t* pChar);
	void					DrawMessage(const wchar_t* pChar, const D2D1_RECT_F& rect);
	void					DrawChar(const wchar_t* pChar, int nIndex, const D2D1_POINT_2F& pos);

	int						DrawOpeningTitle();
	void					DrawQuestion(const wchar_t* pChar);

	void					DrawCommand(const D2D1_POINT_2F& pos, int nIndex);

	HRESULT					BeginDraw();
	HRESULT					EndDraw();
private:
    HWND                     m_hWnd;

    // Format information
    UINT                     m_sourceHeight;
    UINT                     m_sourceWidth;
    LONG                     m_sourceStride;

     // Direct2D 
    ID2D1Factory*            m_pD2DFactory;
    ID2D1HwndRenderTarget*   m_pRenderTarget;
    ID2D1Bitmap*             m_pBitmap;

	// ボディ描画用のブラシ
	ID2D1SolidColorBrush*   m_pBrushJointTracked;
	ID2D1SolidColorBrush*   m_pBrushJointInferred;
	ID2D1SolidColorBrush*   m_pBrushBoneTracked;
	ID2D1SolidColorBrush*   m_pBrushBoneInferred;
	ID2D1SolidColorBrush*   m_pBrushHandClosed;
	ID2D1SolidColorBrush*   m_pBrushHandOpen;
	ID2D1SolidColorBrush*   m_pBrushHandLasso;

	ID2D1SolidColorBrush*   m_pBrushRed;
	ID2D1SolidColorBrush*   m_pBrushWhite;

	// テキスト用
	IDWriteFactory*			m_pDWriteFactory;
	IDWriteTextFormat*		m_pTextFormat;
	IDWriteTextFormat*		m_pAnnotate;
	IDWriteTextFormat*		m_pSemaphore;
	IDWriteTextFormat*		m_pQuestion;

	int						m_nCount;
    HWND                    m_hParentWnd;

	INT64					m_nOpen;
	bool					m_bOpen;

    /// <summary>
    /// Ensure necessary Direct2d resources are created
    /// </summary>
    /// <returns>indicates success or failure</returns>
    HRESULT EnsureResources();

    /// <summary>
    /// Dispose of Direct2d resources 
    /// </summary>
    void DiscardResources();

};