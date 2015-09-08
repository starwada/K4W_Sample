#pragma once

// Direct2D Header Files
#include <d2d1.h>
// Kinect Header files
#include <Kinect.h>
// DirectXTK(Tool Kit)
#include <SimpleMath.h>
#include "ImageRenderer.h"

#include "UIButton.h"

// �Q�[���Ǘ��N���X
// ���̂Ƃ���A���ԁi�X�^�[�g�A�X�g�b�v���j

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
	ImageRenderer*		m_pRenderer;			// �`��N���X
	D2D1_POINT_2F		m_fCursor;				// UI�p
	TIMESPAN			m_nTime;

	CUIButton*			m_pButton;

};

