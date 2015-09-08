#include "Semaphore.h"

#include <math.h>
using namespace DirectX;
using namespace SimpleMath;

Semaphore::Semaphore( UINT64* pIndex ):
	m_Prev(SignalType_Undefined),
	m_nAnimationTimer(0)
{
	if (pIndex == NULL){ return; }
	m_nBodyIndex = *pIndex;

	m_nSignalIndex = 0;
	m_nCount = 0;
	m_nString = 0;
	m_bStringDraw = false;
	m_nDrawCount = 0;
	for (int i = 0; i < ONECHARMAX; i++){ m_nType[i] = SignalType_Undefined; }
	for (int i = 0; i < 64; i++){ m_szString[i] = L'\0'; }
	m_fTol[0] = 0.3;
	m_fTol[1] = 0.5;
	m_fTol[2] = 0.7;

	for (int i = 0; i < 300; i++){ m_nDebug[i] = SignalType_Undefined; }
	m_nDCount = 0;
}

Semaphore::~Semaphore()
{
}

//
bool Semaphore::ItsMe(UINT64* pIndex)
{
	bool bMe = false;

	if (pIndex != NULL)
	{
		if (m_nBodyIndex == *pIndex){ bMe = true; }
	}

	return bMe;
}

//
bool Semaphore::JudgeSemaphore(INT64 time, UINT64 nIndex, const D2D1_POINT_2F* pJoints)
{
	bool bJudge = false;

	bJudge = CGestureSemaphore::Judge(time, nIndex, pJoints);
	// ���攻�肳�ꂽ
	if (bJudge)
	{
		SignalType nType = GetSignalType();
		if (!(m_nSignalIndex > 0 && m_nType[m_nSignalIndex - 1] == nType))
		{
			m_nType[m_nSignalIndex++] = nType;
			// �ꔭ�ڂ����p�͂�蒼���B
			if (m_nType[0] == SignalType_Origin)
			{
				m_nSignalIndex = 0;
				m_nType[m_nSignalIndex] = SignalType_Undefined;
			}
		}
		// m_nType[] index��1�ȏ�Ō��p�̏ꍇ�ɕ�������
		if (m_nSignalIndex > 0 && m_nType[m_nSignalIndex - 1] == SignalType_Origin)
		{
			m_bStringDraw = JudgeString();
			if (m_bStringDraw){ m_nDrawCount = 20; }

			m_nSignalIndex = 0;
			m_Prev = SignalType_Undefined;
		}
		if (m_nSignalIndex == ONECHARMAX){ m_nSignalIndex = 0; m_nCount = 0; m_Prev = SignalType_Undefined; }
	}

	return bJudge;
}

// ���̊֐��͒P���ɂP�t���[���ł̌��o
void Semaphore::SetSignalType(UINT64* pIndex, const D2D1_POINT_2F* pJointPoints, ImageRenderer* pRenderer)
{
	if (!pJointPoints){ return; }
	if (pRenderer == NULL){ return; }
	if (!ItsMe(pIndex)){ return; }

	SignalType nType = JudgeSemaphore(pJointPoints);

//
#ifndef NDEBUG
	m_nDebug[m_nDCount] = nType;
	pRenderer->DrawGraph((INT_PTR*)m_nDebug, m_nDCount);
	if (++m_nDCount == 300){ m_nDCount = 0; }
#endif
//
	if (m_Prev != SignalType_Undefined && m_Prev == nType)
	{
		m_nCount++;
	}
	else
	{
		m_nCount = 0;
	}

	// �F��
	if (m_nCount > 10)
	{
		if (!(m_nSignalIndex > 0 && m_nType[m_nSignalIndex - 1] == nType))
		{
			m_nType[m_nSignalIndex++] = nType;
			// �ꔭ�ڂ����p�͂�蒼���B
			if (m_nType[0] == SignalType_Origin)
			{
				m_nSignalIndex = 0;
				m_nType[m_nSignalIndex] = SignalType_Undefined;
			}
			m_nCount = 0;
		}
	}
// Debug
#ifndef NDEBUG
	pRenderer->DrawSemaphore((INT_PTR*)m_nType, m_nDCount);
#endif
// Debug
	m_Prev = nType;
	// m_nType[] index��1�ȏ�Ō��p�̏ꍇ�ɕ�������
	if (m_nSignalIndex > 0 && m_nType[m_nSignalIndex-1] == SignalType_Origin)
	{
		m_bStringDraw = JudgeString();
		if (m_bStringDraw){ m_nDrawCount = 20; }

		m_nSignalIndex = 0;
		m_Prev = SignalType_Undefined;
	}
	// ���肵�������������ԕ`�悷��
	if (m_bStringDraw && m_nDrawCount-- > 0)
	{
		// �\���ʒu���w�肷��
		m_oSpineMid = pJointPoints[JointType_SpineMid];
		pRenderer->DrawChar(m_szString, m_nString, pJointPoints[JointType_SpineMid]);
	}
// �ΐ탂�[�h���l�����ăR�����g �������ǂ��F������Ă��邩�͕�����Ȃ��悤��
//	pRenderer->DrawMessage(m_szString);

	if (m_nSignalIndex == ONECHARMAX){ m_nSignalIndex = 0; m_nCount = 0; m_Prev = SignalType_Undefined; }

	return;
}

// �ΐ�p����֐�
bool Semaphore::Judge(const wchar_t* pQuestion, int size)
{
	bool bOk = false;

	if (pQuestion == NULL){ return bOk; }
	if (m_nString < size){ return bOk; }

	bOk = true;
	for (int i = 0; i < size; i++)
	{
		if (m_szString[i] != pQuestion[i]){ bOk = false; break; }
	}

	return bOk;
}

//
void Semaphore::Win(ImageRenderer* pRenderer)
{
	if (!pRenderer){ return; }
	pRenderer->DrawChar(L"Win!!", -1, m_oSpineMid);

	return;
}

// �������
// �ߋ��t���[����z�肵������
// �J�A�T�A�^�A�n�s�ɂ͑��_
// �n�s�ɂ͔����_���z�肳���B
// �A�C�E�G�I���ł͌딻�������B
bool Semaphore::JudgeString()
{
	bool flag = true;
	if (m_nString > 63){ return false; }

	switch (m_nSignalIndex)
	{
	// 1��
	case 2:
		{
			  if (m_nType[0] == SignalType_11){ m_szString[m_nString++] = L'�N'; }
			  else if (m_nType[0] == SignalType_6){ m_szString[m_nString++] = L'�j'; }
			  else if (m_nType[0] == SignalType_3){ m_szString[m_nString++] = L'�m'; }
			  else if (m_nType[0] == SignalType_10){ m_szString[m_nString++] = L'�n'; }
			  else if (m_nType[0] == SignalType_9){ m_szString[m_nString++] = L'�t'; }
			  else if (m_nType[0] == SignalType_4){ m_szString[m_nString++] = L'�w'; }
			  else if (m_nType[0] == SignalType_12){ m_szString[m_nString++] = L'��'; }
			  else if (m_nType[0] == SignalType_7){ m_szString[m_nString++] = L'��'; }
			  else{ flag = false; }
		}
		break;
	// 2��
	case 3:
		{
			if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�A';
			}
			else if (m_nType[0] == SignalType_3 && m_nType[1] == SignalType_2)
			{
				m_szString[m_nString++] = L'�C';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'�E';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�J';
			}
			else if (m_nType[0] == SignalType_11 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'�O';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_2)
			{
				m_szString[m_nString++] = L'�L';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�P';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'�R';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_12)
			{
				m_szString[m_nString++] = L'�T';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'�V';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'�Z';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�\';
			}
			else if (m_nType[0] == SignalType_11 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'�^';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_2W)
			{
				m_szString[m_nString++] = L'�`';
			}
			else if (m_nType[0] == SignalType_12 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�c';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�e';
			}
			else if (m_nType[0] == SignalType_2 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'�g';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'�i';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_4)
			{
				m_szString[m_nString++] = L'�k';
			}
			else if (m_nType[0] == SignalType_10 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'�o';
			}
			else if (m_nType[0] == SignalType_10 && m_nType[1] == SignalType_14)
			{
				m_szString[m_nString++] = L'�p';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'�q';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'�u';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_14)
			{
				m_szString[m_nString++] = L'�v';
			}
			else if (m_nType[0] == SignalType_4 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'�x';
			}
			else if (m_nType[0] == SignalType_4 && m_nType[1] == SignalType_14)
			{
				m_szString[m_nString++] = L'�y';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'�}';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'�~';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_3 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_4)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_6)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_3 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_8)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_2 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'��';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'��';
			}
			else{ flag = false; }
		}
		break;
	// 3��
	case 4:
		{
			if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2W && m_nType[2] == SignalType_1)
			{
				m_szString[m_nString++] = L'�G';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_3)
			{
				m_szString[m_nString++] = L'�I';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�K';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�M';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�Q';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_1 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�S';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_12 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�U';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�W';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_5)
			{
				m_szString[m_nString++] = L'�X';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�[';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�]';
			}
			else if (m_nType[0] == SignalType_11 && m_nType[1] == SignalType_5 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�_';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_2W && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�a';
			}
			else if (m_nType[0] == SignalType_12 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�d';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�f';
			}
			else if (m_nType[0] == SignalType_2 && m_nType[1] == SignalType_5 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�h';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_1)
			{
				m_szString[m_nString++] = L'�l';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'�r';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_14)
			{
				m_szString[m_nString++] = L'�s';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_10)
			{
				m_szString[m_nString++] = L'�z';
			}
			else{ flag = false; }
		}
		break;
	// 4��
	case 5:
		{
			if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_5 && m_nType[3] == SignalType_13)
			{
				m_szString[m_nString++] = L'�Y';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_10 && m_nType[3] == SignalType_13)
			{
				m_szString[m_nString++] = L'�{';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_10 && m_nType[3] == SignalType_14)
			{
				m_szString[m_nString++] = L'�|';
			}
			else{ flag = false; }
		}
		break;
	default:
		flag = false;
		break;
	}

	return flag;
}

// ���攻��
SignalType Semaphore::JudgeSemaphore(const D2D1_POINT_2F* pJointPoints)
{
	SignalType nType = SignalType_Undefined;

	if (!pJointPoints){ return nType; }

	DirectX::SimpleMath::Vector2	m_geRight;	// �E�r
	DirectX::SimpleMath::Vector2	m_geLeft;	// ���r

	m_geLeft.x = pJointPoints[JointType_WristLeft].x - pJointPoints[JointType_ShoulderLeft].x;
	m_geLeft.y = pJointPoints[JointType_WristLeft].y - pJointPoints[JointType_ShoulderLeft].y;

	m_geRight.x = pJointPoints[JointType_WristRight].x - pJointPoints[JointType_ShoulderRight].x;
	m_geRight.y = pJointPoints[JointType_WristRight].y - pJointPoints[JointType_ShoulderRight].y;
	//m_geLeft.x = pJointPoints[JointType_WristLeft].x - pJointPoints[JointType_ElbowLeft].x;
	//m_geLeft.y = pJointPoints[JointType_WristLeft].y - pJointPoints[JointType_ElbowLeft].y;

	//m_geRight.x = pJointPoints[JointType_WristRight].x - pJointPoints[JointType_ElbowRight].x;
	//m_geRight.y = pJointPoints[JointType_WristRight].y - pJointPoints[JointType_ElbowRight].y;

	int Seg[2] = { 0 };

	Seg[0] = JudgeSegment(m_geRight, 1);	// �E�r
	Seg[1] = JudgeSegment(m_geLeft, 1, false);		// ���r

	// �x�N�g���̒��������f��ɒǉ�����B
	if (Seg[0] == 2 && Seg[1] == 2)
	{
		nType = SignalType_Origin;
	}
	// ��1����
	else if (Seg[0] == 0 && Seg[1] == 4)
	{
		nType = SignalType_1;
	}
	else if (Seg[0] == 6 && Seg[1] == 2)
	{
		nType = SignalType_2;
	}
	else if (Seg[0] == 2 && Seg[1] == 6)
	{
		nType = SignalType_2W;
	}
	else if (Seg[0] == 1 && Seg[1] == 5)
	{
		nType = SignalType_3;
	}
	else if (Seg[0] == 7 && Seg[1] == 3)
	{
		nType = SignalType_4;
	}
	else if ((Seg[0] == 5 && Seg[1] == 7) ||
		(pJointPoints[JointType_WristRight].x < pJointPoints[JointType_WristLeft].x
		&& m_geRight.y < 0 && m_geLeft.y < 0))
	{
		nType = SignalType_5;
	}
	else if (Seg[0] == 0 && Seg[1] == 0)
	{
		nType = SignalType_6;
	}
	else if (Seg[0] == 6 && Seg[1] == 4)
	{
		nType = SignalType_7;
	}
	else if (Seg[0] == 0 && Seg[1] == 2)
	{
		nType = SignalType_8;
	}
	else if (Seg[0] == 0 && Seg[1] == 1)
	{
		nType = SignalType_9;
	}
	else if (Seg[0] == 7 && Seg[1] == 5)
	{
		nType = SignalType_10;
	}
	else if (Seg[0] == 5 && Seg[1] == 5)
	{
		nType = SignalType_11;
	}
	else if (Seg[0] == 6 && Seg[1] == 6)
	{
		nType = SignalType_12;
	}
	else if (Seg[0] == 2 && Seg[1] == 5)
	{
		nType = SignalType_13;
	}
	else if (Seg[0] == 7 && Seg[1] == 2)
	{
		nType = SignalType_14;
	}
	else if (nType == SignalType_Undefined)
	{
		m_nCount = 0;
		//return;
	}

	return nType;
}

// ���𒆐S�ɂW���������̈�ɂ��邩�Ŕ��f���Ă���B
// ��U��𔻒f����ہA������l������B
int Semaphore::JudgeSegment(DirectX::SimpleMath::Vector2& vec, int nLevel, bool bRight/* =true*/)
{
	int nSegmentType = -1;
	if (nLevel < 0 || 2 < nLevel){ return nSegmentType; }

	DirectX::SimpleMath::Vector2	vecX(1.0, 0.0);
	float fAngle = 0.0;

	fAngle = acos(vecX.Dot(vec) / vec.Length());

	if (fAngle < m_fTol[nLevel]){ nSegmentType = 0; }
	else if (fabs(fAngle - XM_PI / 4.0) < m_fTol[nLevel])
	{
		nSegmentType = 1;
		if (vec.y < 0.0){ nSegmentType = 7; }
	}
	else if (fabs(fAngle - XM_PI / 2.0) < m_fTol[nLevel])
	{
		nSegmentType = 2;
		if (vec.y < 0.0){ nSegmentType = 6; }
	}
	else if (fabs(fAngle - XM_PI*3.0 / 4.0) < m_fTol[nLevel])
	{
		nSegmentType = 3;
		if (vec.y < 0.0){ nSegmentType = 5; }
	}
	else if (fabs(fAngle - XM_PI) < m_fTol[nLevel]){ nSegmentType = 4; }

	// ��6��̍����z�肵������
	if (!bRight && fabs(fAngle - XM_PI / 5.0) < m_fTol[nLevel] && vec.y < 0.0)
	{
		nSegmentType = 0;
	}

	return nSegmentType;
}

// �{���͉����F���ɂĎw�肵�������̎����\�������������A�����͂����ł̃n�[�h�R�[�f�B���O
void Semaphore::Practice(INT64 nTime, D2D1_POINT_2F* pJointPoints, ImageRenderer* pRenderer)
{
	if (m_nAnimationTimer == 0){ m_nAnimationTimer = nTime; }
	// �A�j���[�V�����^�C�}�[�̍����ɂď����𕪊򂳂���B�܂�A�A�j���[�V�����B

	// �����ɂ���Č��܂����|�[�Y����莞�ԕ\��������B
	// �ȉ��ł�������1�b
	if (nTime - m_nAnimationTimer > 10000000 && nTime - m_nAnimationTimer < 50000000)
	{
		m_szString[0] = L'�A';
		m_szString[1] = L'\0';
		m_nString = 1;
		pRenderer->DrawChar(m_szString, m_nString, pJointPoints[JointType_SpineMid]);
	}
	// �A�̃p�^�[��
	m_nType[0] = SignalType_9;
	m_nType[1] = SignalType_3;
	m_nType[2] = SignalType_Origin;
	m_nType[3] = SignalType_Undefined;

	// 2����������Z���̉�]���N�H�[�^�j�I���ɂ�
	Vector3 axis(0, 0, 1);
	Quaternion quat[2];

	Vector2 wristright(pJointPoints[JointType_WristRight].x, pJointPoints[JointType_WristRight].y);
	Vector2 elbowright(pJointPoints[JointType_ElbowRight].x, pJointPoints[JointType_ElbowRight].y);
	Vector2 shoulderright(pJointPoints[JointType_ShoulderRight].x, pJointPoints[JointType_ShoulderRight].y);

	Vector2 right[2];
	right[0] = wristright - elbowright;
	right[1] = elbowright - shoulderright;
	right[0].x = right[0].Length();
	right[1].x = right[1].Length();
	right[0].y = right[1].y = 0.0;

	Vector2 wristleft(pJointPoints[JointType_WristLeft].x, pJointPoints[JointType_WristLeft].y);
	Vector2 elbowleft(pJointPoints[JointType_ElbowLeft].x, pJointPoints[JointType_ElbowLeft].y);
	Vector2 shoulderleft(pJointPoints[JointType_ShoulderLeft].x, pJointPoints[JointType_ShoulderLeft].y);

	Vector2 left[2];
	left[0] = wristleft - elbowleft;
	left[1] = elbowleft - shoulderleft;
	left[0].x = left[0].Length();
	left[1].x = left[1].Length();
	left[0].y = left[1].y = 0.0;

	// �|�[�Y�̕ύX�͂����ōs���A�`��͌Ăяo����
	if (nTime - m_nAnimationTimer > 40000000 && nTime - m_nAnimationTimer < 60000000)
	{
		quat[0] = Quaternion::CreateFromAxisAngle(axis, 0);
		quat[1] = Quaternion::CreateFromAxisAngle(axis, 3.14159 / 4);
	}
	if (nTime - m_nAnimationTimer > 60000000 && nTime - m_nAnimationTimer < 80000000)
	{
		quat[0] = Quaternion::CreateFromAxisAngle(axis, 3.14159/4);
		quat[1] = Quaternion::CreateFromAxisAngle(axis, 3.14159*5 / 4);
	}
	if (nTime - m_nAnimationTimer > 80000000 && nTime - m_nAnimationTimer < 100000000)
	{
		quat[0] = Quaternion::CreateFromAxisAngle(axis, 3.14159/2);
		quat[1] = Quaternion::CreateFromAxisAngle(axis, 3.14159 / 2);
	}
	right[0] = Vector2::Transform(right[0], quat[0]);
	right[1] = Vector2::Transform(right[1], quat[0]);

	elbowright = shoulderright + right[1];
	wristright = elbowright + right[0];
	left[0] = Vector2::Transform(left[0], quat[1]);
	left[1] = Vector2::Transform(left[1], quat[1]);

	elbowleft = shoulderleft + left[1];
	wristleft = elbowleft + left[0];

	pJointPoints[JointType_WristRight].x = wristright.x;
	pJointPoints[JointType_WristRight].y = wristright.y;
	pJointPoints[JointType_ElbowRight].x = elbowright.x;
	pJointPoints[JointType_ElbowRight].y = elbowright.y;

	pJointPoints[JointType_WristLeft].x = wristleft.x;
	pJointPoints[JointType_WristLeft].y = wristleft.y;
	pJointPoints[JointType_ElbowLeft].x = elbowleft.x;
	pJointPoints[JointType_ElbowLeft].y = elbowleft.y;

	return;
}

// �w�蕶���̃|�[�Y�𔻒肷��
bool Semaphore::JudgeType()
{
	bool flag = true;
	if (m_nString > 63){ return false; }
	// ������
	for (int i = 0; i < ONECHARMAX; i++){ m_nType[i] = SignalType_Undefined; }

	switch (m_nSignalIndex)
	{
		// 1��
	case 2:
	{
			  if (m_szString[m_nString] == L'�N'){ m_nType[0] = SignalType_11; }
			  else if (m_szString[m_nString] == L'�j'){ m_nType[0] = SignalType_6; }
			  else if (m_szString[m_nString] == L'�m'){ m_nType[0] = SignalType_3; }
			  else if (m_szString[m_nString] == L'�n'){ m_nType[0] = SignalType_10; }
			  else if (m_szString[m_nString] == L'�t'){ m_nType[0] = SignalType_9; }
			  else if (m_szString[m_nString] == L'�w'){ m_nType[0] = SignalType_4; }
			  else if (m_szString[m_nString] == L'��'){ m_nType[0] = SignalType_12; }
			  else if (m_szString[m_nString] == L'��'){ m_nType[0] = SignalType_7; }
			  else{ flag = false; }
	}
		break;
		// 2��
	case 3:
	{
			  if (m_szString[m_nString] == L'�A')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�C')
			  {
				  m_nType[0] = SignalType_3;
				  m_nType[1] = SignalType_2;
			  }
			  else if (m_szString[m_nString] == L'�E')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'�J')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�O')
			  {
				  m_nType[0] = SignalType_11;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�L')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_2;
			  }
			  else if (m_szString[m_nString] == L'�P')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�R')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'�T')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_12;
			  }
			  else if (m_szString[m_nString] == L'�V')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'�Z')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'�\')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�^')
			  {
				  m_nType[0] = SignalType_11;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'�`')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_2W;
			  }
			  else if (m_szString[m_nString] == L'�c')
			  {
				  m_nType[0] = SignalType_12;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�e')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�g')
			  {
				  m_nType[0] = SignalType_2;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'�i')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�k')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_4;
			  }
			  else if (m_szString[m_nString] == L'�o')
			  {
				  m_nType[0] = SignalType_10;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�p')
			  {
				  m_nType[0] = SignalType_10;
				  m_nType[1] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'�q')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'�u')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�v')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'�x')
			  {
				  m_nType[0] = SignalType_4;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�y')
			  {
				  m_nType[0] = SignalType_4;
				  m_nType[1] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'�}')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'�~')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_3;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_4;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_6;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_3;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_8;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_2;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'��')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_1;
			  }
			  else{ flag = false; }
	}
		break;
		// 3��
	case 4:
	{
			  if (m_szString[m_nString] == L'�G')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2W;
				  m_nType[2] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'�I')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'�K')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�M')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�Q')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�S')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_1;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�U')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_12;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�W')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�X')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'�[')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�]')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�_')
			  {
				  m_nType[0] = SignalType_11;
				  m_nType[1] = SignalType_5;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�a')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_2W;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�d')
			  {
				  m_nType[0] = SignalType_12;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�f')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�h')
			  {
				  m_nType[0] = SignalType_2;
				  m_nType[1] = SignalType_5;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�l')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'�r')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�s')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'�z')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_10;
			  }
			  else{ flag = false; }
	}
		break;
		// 4��
	case 5:
	{
			  if (m_szString[m_nString] == L'�Y')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_5;
				  m_nType[3] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�{')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_10;
				  m_nType[3] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'�|')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_10;
				  m_nType[3] = SignalType_14;
			  }
			  else{ flag = false; }
	}
		break;
	default:
		flag = false;
		break;
	}

	return flag;
}
