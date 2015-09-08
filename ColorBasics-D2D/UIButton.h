#pragma once
#include "UIBase.h"

// ボタンクラス
// 表示文字
// 形状情報　円のみ想定するので、中心座標と半径
// 自分がどんなボタンなのかは認識する。スタート、ストップ等。
//
enum ButtonStatus
{
	ButtonStart = 0,
	ButtonStop = 1
};

class CUIButton :
	public CUIBase
{
public:
	CUIButton(ImageRenderer* pRenderer = NULL);
	~CUIButton();

	bool	IsHover(D2D1_POINT_2F& point);

	void	SetButton(TIMESPAN nTime, D2D1_POINT_2F& center, ButtonStatus nStatus);

	void	GetPosition(D2D1_POINT_2F& pos){ pos = m_Center; };

private:
	D2D1_POINT_2F	m_Center;
	float			m_fRadius;
	ButtonStatus	m_nStatus;
};

