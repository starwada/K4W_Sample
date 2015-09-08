#pragma once
#include "UIBase.h"

// �{�^���N���X
// �\������
// �`����@�~�̂ݑz�肷��̂ŁA���S���W�Ɣ��a
// �������ǂ�ȃ{�^���Ȃ̂��͔F������B�X�^�[�g�A�X�g�b�v���B
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

