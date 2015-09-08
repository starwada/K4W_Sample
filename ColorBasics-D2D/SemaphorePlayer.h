// ����M����ǃN���X

#pragma once
// Direct2D Header Files
#include <d2d1.h>
// Kinect Header files
#include <Kinect.h>

// DirectXTK(Tool Kit)
#include <SimpleMath.h>
#include "ImageRenderer.h"

#include "GestureSemaphore.h"

#define	ONECHARMAX	5			// 1������\������ɕK�v�ȃT�C�Y
// �u�X�v�͂R�̃p�^�[���ő��_�A�����_���l������ƁA���p�����ĂT�K�v�B
// ���𒆐S�Ƃ����܂ł𔼌a�Ƃ����~��z�肷��B// ���X+�������O�x�Ƃ�45�x�̉~�ʋ�Ԃ��l����B
// X+������ɁA���v���Ɂ}22.5������ԂO�i�E�����j�B
// +22.5��->+67.5�� ��ԂP�i�E���j
// +67.5��->+112.5����ԂQ�i���j
// +112.5��->+157.5����ԂR�i�����j
// +157.5��->-157.5����ԂS�i���j
// -157.5��->-112.5����ԂT�i����j
// -112.5��->-67.5����ԂU�i��j
// -67.5��->-22.5����ԂV�i�E��j

class CSemaphorePlayer :
	public CGestureSemaphore
{
public:
	CSemaphorePlayer();
	~CSemaphorePlayer();

	bool		SetPlayer(UINT64 nIndex);

	int			Play(D2D1_POINT_2F jointPoints[JointType_Count]);

	D2D1_POINT_2F	GetJoints(JointType nJoint){ return m_jointPoints[nJoint]; }

private:
	D2D1_POINT_2F m_jointPoints[JointType_Count];

	wchar_t*	m_pString;
	INT64		m_nStringSize;

	int			m_nSignalIndex;		// �ꕶ������̈ʒu�i�C���f�b�N�X�j
	int			m_nCount;		// �����

	SignalType	m_Prev;
	SignalType	m_nType[ONECHARMAX];	// �ꕶ�� ���p���܂ߍő�5�|�[�Y�Ȃ̂�

	wchar_t		m_szString[64];		// ���������
	int			m_nString;
	bool		m_bStringDraw;
	int			m_nDrawCount;

	D2D1_POINT_2F	m_oSpineMid;

	float		m_fTol[3];

	SignalType	m_nDebug[300];
	int			m_nDCount;

	INT64		m_nAnimationTimer;	// �A�j���[�V�����p�^�C�}�[

	bool		JudgeString();
	bool		JudgeType();

	// ���攻��
	SignalType	JudgeSemaphore(const D2D1_POINT_2F* pJointPoints);
	int		JudgeSegment(DirectX::SimpleMath::Vector2& vec, int nLevel, bool bRight = true);
};

