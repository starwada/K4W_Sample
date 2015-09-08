#include <strsafe.h>
#include "GameBase.h"

static D2D1_POINT_2F g_center;

CGameBase::CGameBase(ImageRenderer* pRenderer)
	: m_pButton(NULL)
{
	m_pRenderer = pRenderer;
	m_nTime = 0;

	g_center.x = 1920.0 / 2.0;
	g_center.y = 1080.0 / 3.0;
}

CGameBase::~CGameBase()
{
	Clear();
}

void CGameBase::Clear()
{
	if (m_pButton){ delete m_pButton; m_pButton = NULL; }
	return;
}

void CGameBase::SetRelativeTime(TIMESPAN nTime)
{
	if (!m_nTime){ m_nTime = nTime; }

	return;
}

// タイトル表示
void CGameBase::Title(const wchar_t* pTitle)
{
	if (!m_pRenderer || !pTitle){ return; }

	D2D1_RECT_F rect = D2D1::RectF(200.0, 100.0, 1700.0, 300.0);
	m_pRenderer->DrawMessage(pTitle, rect);

	return;
}

void CGameBase::Player(UINT64 nPlayer)
{
	if (!m_pRenderer){ return; }
	if (!nPlayer){ return; }

	wchar_t szStatusMessage[128] = { 0 };
	StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L"%d人", nPlayer);

	D2D1_RECT_F rect = D2D1::RectF(1000.0, 100.0, 1700.0, 300.0);
	m_pRenderer->DrawMessage(szStatusMessage, rect);

	return;
}

void CGameBase::Button(TIMESPAN nTime, ButtonStatus nStatus)
{
	if (!m_pButton)
	{
		m_pButton = new CUIButton(m_pRenderer);
		if (!m_pButton){ return; }
	}

	m_pButton->SetButton(nTime, g_center, nStatus);

	return;
}

void CGameBase::SetCursor(D2D1_POINT_2F pos)
{
	if (!m_pRenderer){ return; }

	m_pRenderer->DrawHand(HandState_Open, pos);

	return;
}

void CGameBase::GetButtonPos(D2D1_POINT_2F& pos)
{
	if (m_pButton)
	{
		m_pButton->GetPosition(pos);
	}

	return;
}
bool CGameBase::IsButtonHover(D2D1_POINT_2F& pos)
{
	bool bHover = false;

	if (m_pButton)
	{
		bHover = m_pButton->IsHover(pos);
	}

	return bHover;
}
