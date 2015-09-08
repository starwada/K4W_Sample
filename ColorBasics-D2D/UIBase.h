#pragma once
// Kinect Header files
#include <Kinect.h>
// DirectXTK(Tool Kit)
#include <SimpleMath.h>
#include "ImageRenderer.h"

class CUIBase
{
public:
	CUIBase(ImageRenderer* pRenderer = NULL);
	~CUIBase();

	void		SetTime(TIMESPAN nTime);
	TIMESPAN	GetTime(){ return m_nTime; }

	virtual bool	IsHover(D2D1_POINT_2F& center) = 0;

	void	CircleButton(D2D1_POINT_2F& center, const wchar_t* pCaption);

private:
	ImageRenderer*	m_pRenderer;
	TIMESPAN		m_nTime;

};

