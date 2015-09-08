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

class Semaphore : public CGestureSemaphore
{
public:
	Semaphore( UINT64* pIndex ) ;
	~Semaphore();

	bool		JudgeSemaphore(INT64 time, UINT64 nIndex, const D2D1_POINT_2F* pJoints);
	// ���������|�[�Y��F��������
	void		SetSignalType(UINT64* pIndex, const D2D1_POINT_2F* pJointPoints, ImageRenderer* pRenderer);

	// ���K�p�Ƀ|�[�Y��\��������B�����̃{�f�B�[���g���āB
	void		Practice(INT64 nTime, D2D1_POINT_2F* pJointPoints, ImageRenderer* pRenderer);

	//
	bool		ItsMe(UINT64* pIndex);
	//
	bool		Judge(const wchar_t* pQuestion, int size);
	//
	void		Win(ImageRenderer* pRenderer);

private:
	WCHAR*			m_pString;
	INT64			m_nStringSize;

	UINT64		m_nBodyIndex;		// �{�f�B�C���f�b�N�X�i�����l�ł̑ΐ��z�肵�āj
	int		m_nSignalIndex;		// �ꕶ������̈ʒu�i�C���f�b�N�X�j
	int		m_nCount;		// �����

	SignalType	m_Prev;
	SignalType	m_nType[ONECHARMAX];	// �ꕶ�� ���p���܂ߍő�5�|�[�Y�Ȃ̂�

	wchar_t		m_szString[64];		// ���������
	int		m_nString;
	bool		m_bStringDraw;
	int		m_nDrawCount;

	D2D1_POINT_2F	m_oSpineMid;

	float		m_fTol[3];

	SignalType	m_nDebug[300];
	int		m_nDCount;

	INT64		m_nAnimationTimer;	// �A�j���[�V�����p�^�C�}�[

	bool		JudgeString();
	bool		JudgeType();

	// ���攻��
	SignalType	JudgeSemaphore(const D2D1_POINT_2F* pJointPoints);
	int		JudgeSegment(DirectX::SimpleMath::Vector2& vec, int nLevel, bool bRight=true);

};

