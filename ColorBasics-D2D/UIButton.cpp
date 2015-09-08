#include "UIButton.h"
using namespace DirectX;
using namespace SimpleMath;

wchar_t szButtonCaption[][64] = { L"スタート", L"ストップ" };

CUIButton::CUIButton(ImageRenderer* pRenderer)
	:CUIBase(pRenderer),
	m_fRadius(100.0),
	m_nStatus(ButtonStart)
{
	m_Center = D2D1::Point2F();
}

CUIButton::~CUIButton()
{
}

void CUIButton::SetButton(TIMESPAN nTime, D2D1_POINT_2F& center, ButtonStatus nStatus)
{
	m_Center = center;
	
	CUIBase::SetTime(nTime);

	CircleButton(center, szButtonCaption[nStatus]);

	return;
}

bool CUIButton::IsHover(D2D1_POINT_2F& point)
{
	return (Vector2(m_Center.x, m_Center.y) - Vector2(point.x, point.y)).Length() < m_fRadius ?
		true : false;
}
