//
// �W�F�X�`���[������N���X
//
#pragma once
#include "stdafx.h"

// �W�F�X�`���[����p�̊􉽌v�Z�͂��̃N���X�ōs���B
//
#define	BODYINDEX_INIT	1000

class CGestureBase
{
public:
	CGestureBase();
	~CGestureBase();

	bool	ItsMe(UINT64 nIndex);

	void	Start(INT64 time){ m_nStartTime = time; };
	bool	JudgeTime(INT64 time);

	// ���菃�����z�֐��i�h����Ŏ�������j
	virtual bool Judge(INT64 nTime, UINT64 nIndex, const D2D1_POINT_2F* pJoints) = 0;
	
	bool	isOk(){ return m_bOk; };

	void	Clear();

private:
	UINT64	m_nBodyIndex;			// �{�f�B�C���f�b�N�X �قȂ�{�f�B�Ŕ��肵�Ă��d���Ȃ�
	INT64	m_nStartTime;
	bool	m_bOk;


};

