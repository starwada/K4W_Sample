#pragma once

// Direct2D Header Files
#include <d2d1.h>
// Kinect Header files
#include <Kinect.h>
// DirectXTK(Tool Kit)
#include <SimpleMath.h>
#include "ImageRenderer.h"

#include "UIButton.h"

// ゲーム管理クラス
// 今のところ、時間（スタート、ストップ等）

class CGameBase
{
public:
	CGameBase(ImageRenderer* pRenderer);
	~CGameBase();

	void		SetRelativeTime(TIMESPAN nTime);
	TIMESPAN	GetRelativeTime(){ return m_nTime; };

	void		Title(const wchar_t* pTitle);
	void		Player(UINT64 nPlayer);
	void		Button(TIMESPAN nTime, ButtonStatus nStatus);

	void		SetCursor(D2D1_POINT_2F pos);
	void		GetButtonPos(D2D1_POINT_2F& pos);

	bool		IsButtonHover(D2D1_POINT_2F& pos);

	void		Clear();

private:
	ImageRenderer*		m_pRenderer;			// 描画クラス
	D2D1_POINT_2F		m_fCursor;				// UI用
	TIMESPAN			m_nTime;

	CUIButton*			m_pButton;

};

