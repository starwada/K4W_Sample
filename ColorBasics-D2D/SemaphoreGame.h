#pragma once
#include "GameBase.h"
#include "SemaphorePlayer.h"

// (���)�Q�[���̃X�e�[�^�X�ɂ�
// ��{�ǂ̂悤�ȃQ�[���ɂ����Ă����������������e���z�肳��邪
//
// 0 �J�n�O
// 1 �l���i�v���C���[�j�m��
// 2 �Q�[���X�^�[�g�O
// 3 �Q�[����
// 4 �Q�[���I��
// 5 ����
// �X�e�[�^�X���ς������ێ����Ă��鎞�Ԃ��X�V���Ă�����

// ����Q�[���N���X
// ����͓��R�Q�[���N���X���Ǘ�����
//
class CSemaphoreGame : public CGameBase
{
public:
	CSemaphoreGame(ImageRenderer* pRenderer, ICoordinateMapper* pMapper);
	~CSemaphoreGame();

	void	Clear();

	// �Q�[�����̉�ʕ\��
	void	Display(TIMESPAN nTime);
	//
	void	CloseHand(D2D1_POINT_2F button, D2D1_POINT_2F& hand);

	// �Q�[��
	int		Play(TIMESPAN nTime, int nBodyCount, IBody** ppBodies);

	void	SetPlayer(int nBodyCount, IBody** ppBodies);


private:
	ICoordinateMapper*	m_pCoordinateMapper;

	CSemaphorePlayer*	m_pPlayer[BODY_COUNT];	// �v���C���[
	UINT64				m_nPlayer;				// �l��

	UINT64				m_nStatus;				// �Q�[���X�e�[�^�X�@���ǂ̏�Ԃ�

	D2D1_POINT_2F		BodyToScreen(const CameraSpacePoint& bodyPoint);

};

