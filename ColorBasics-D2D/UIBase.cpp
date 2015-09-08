#include "UIBase.h"

CUIBase::CUIBase(ImageRenderer* pRenderer) :
	m_nTime(0)
{
	m_pRenderer = pRenderer;
}

CUIBase::~CUIBase()
{
}

void CUIBase::SetTime(TIMESPAN nTime)
{
	if (!m_nTime){ m_nTime = nTime; }
}

void CUIBase::CircleButton(D2D1_POINT_2F& center, const wchar_t* pCaption)
{
	if (!m_pRenderer){ return; }

	m_pRenderer->DrawButton(center, pCaption);

	return;
}
