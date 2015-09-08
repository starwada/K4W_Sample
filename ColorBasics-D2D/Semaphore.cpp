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
	// 原画判定された
	if (bJudge)
	{
		SignalType nType = GetSignalType();
		if (!(m_nSignalIndex > 0 && m_nType[m_nSignalIndex - 1] == nType))
		{
			m_nType[m_nSignalIndex++] = nType;
			// 一発目が原姿はやり直し。
			if (m_nType[0] == SignalType_Origin)
			{
				m_nSignalIndex = 0;
				m_nType[m_nSignalIndex] = SignalType_Undefined;
			}
		}
		// m_nType[] indexが1以上で原姿の場合に文字判定
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

// この関数は単純に１フレームでの検出
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

	// 認定
	if (m_nCount > 10)
	{
		if (!(m_nSignalIndex > 0 && m_nType[m_nSignalIndex - 1] == nType))
		{
			m_nType[m_nSignalIndex++] = nType;
			// 一発目が原姿はやり直し。
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
	// m_nType[] indexが1以上で原姿の場合に文字判定
	if (m_nSignalIndex > 0 && m_nType[m_nSignalIndex-1] == SignalType_Origin)
	{
		m_bStringDraw = JudgeString();
		if (m_bStringDraw){ m_nDrawCount = 20; }

		m_nSignalIndex = 0;
		m_Prev = SignalType_Undefined;
	}
	// 判定した文字を一定期間描画する
	if (m_bStringDraw && m_nDrawCount-- > 0)
	{
		// 表示位置を指定する
		m_oSpineMid = pJointPoints[JointType_SpineMid];
		pRenderer->DrawChar(m_szString, m_nString, pJointPoints[JointType_SpineMid]);
	}
// 対戦モードを考慮してコメント 自分がどう認識されているかは分からないように
//	pRenderer->DrawMessage(m_szString);

	if (m_nSignalIndex == ONECHARMAX){ m_nSignalIndex = 0; m_nCount = 0; m_Prev = SignalType_Undefined; }

	return;
}

// 対戦用判定関数
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

// 手旗判定
// 過去フレームを想定した判定
// カ、サ、タ、ハ行には濁点
// ハ行には半濁点が想定される。
// アイウエオ順では誤判定をする。
bool Semaphore::JudgeString()
{
	bool flag = true;
	if (m_nString > 63){ return false; }

	switch (m_nSignalIndex)
	{
	// 1画
	case 2:
		{
			  if (m_nType[0] == SignalType_11){ m_szString[m_nString++] = L'ク'; }
			  else if (m_nType[0] == SignalType_6){ m_szString[m_nString++] = L'ニ'; }
			  else if (m_nType[0] == SignalType_3){ m_szString[m_nString++] = L'ノ'; }
			  else if (m_nType[0] == SignalType_10){ m_szString[m_nString++] = L'ハ'; }
			  else if (m_nType[0] == SignalType_9){ m_szString[m_nString++] = L'フ'; }
			  else if (m_nType[0] == SignalType_4){ m_szString[m_nString++] = L'ヘ'; }
			  else if (m_nType[0] == SignalType_12){ m_szString[m_nString++] = L'リ'; }
			  else if (m_nType[0] == SignalType_7){ m_szString[m_nString++] = L'レ'; }
			  else{ flag = false; }
		}
		break;
	// 2画
	case 3:
		{
			if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'ア';
			}
			else if (m_nType[0] == SignalType_3 && m_nType[1] == SignalType_2)
			{
				m_szString[m_nString++] = L'イ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'ウ';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'カ';
			}
			else if (m_nType[0] == SignalType_11 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'グ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_2)
			{
				m_szString[m_nString++] = L'キ';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'ケ';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'コ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_12)
			{
				m_szString[m_nString++] = L'サ';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'シ';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'セ';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'ソ';
			}
			else if (m_nType[0] == SignalType_11 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'タ';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_2W)
			{
				m_szString[m_nString++] = L'チ';
			}
			else if (m_nType[0] == SignalType_12 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'ツ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'テ';
			}
			else if (m_nType[0] == SignalType_2 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'ト';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_3)
			{
				m_szString[m_nString++] = L'ナ';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_4)
			{
				m_szString[m_nString++] = L'ヌ';
			}
			else if (m_nType[0] == SignalType_10 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'バ';
			}
			else if (m_nType[0] == SignalType_10 && m_nType[1] == SignalType_14)
			{
				m_szString[m_nString++] = L'パ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'ヒ';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'ブ';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_14)
			{
				m_szString[m_nString++] = L'プ';
			}
			else if (m_nType[0] == SignalType_4 && m_nType[1] == SignalType_13)
			{
				m_szString[m_nString++] = L'ベ';
			}
			else if (m_nType[0] == SignalType_4 && m_nType[1] == SignalType_14)
			{
				m_szString[m_nString++] = L'ペ';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'マ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'ミ';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'ム';
			}
			else if (m_nType[0] == SignalType_3 && m_nType[1] == SignalType_5)
			{
				m_szString[m_nString++] = L'メ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'モ';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_4)
			{
				m_szString[m_nString++] = L'ヤ';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'ユ';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_6)
			{
				m_szString[m_nString++] = L'ヨ';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'ラ';
			}
			else if (m_nType[0] == SignalType_3 && m_nType[1] == SignalType_7)
			{
				m_szString[m_nString++] = L'ル';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_8)
			{
				m_szString[m_nString++] = L'ロ';
			}
			else if (m_nType[0] == SignalType_2 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'ワ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_9)
			{
				m_szString[m_nString++] = L'ヲ';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_1)
			{
				m_szString[m_nString++] = L'ン';
			}
			else{ flag = false; }
		}
		break;
	// 3画
	case 4:
		{
			if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2W && m_nType[2] == SignalType_1)
			{
				m_szString[m_nString++] = L'エ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_3)
			{
				m_szString[m_nString++] = L'オ';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ガ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ギ';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ゲ';
			}
			else if (m_nType[0] == SignalType_8 && m_nType[1] == SignalType_1 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ゴ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_12 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ザ';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ジ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_5)
			{
				m_szString[m_nString++] = L'ス';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ゼ';
			}
			else if (m_nType[0] == SignalType_5 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ゾ';
			}
			else if (m_nType[0] == SignalType_11 && m_nType[1] == SignalType_5 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ダ';
			}
			else if (m_nType[0] == SignalType_7 && m_nType[1] == SignalType_2W && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ヂ';
			}
			else if (m_nType[0] == SignalType_12 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ヅ';
			}
			else if (m_nType[0] == SignalType_6 && m_nType[1] == SignalType_3 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'デ';
			}
			else if (m_nType[0] == SignalType_2 && m_nType[1] == SignalType_5 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ド';
			}
			else if (m_nType[0] == SignalType_9 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_1)
			{
				m_szString[m_nString++] = L'ネ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_13)
			{
				m_szString[m_nString++] = L'ビ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_7 && m_nType[2] == SignalType_14)
			{
				m_szString[m_nString++] = L'ピ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_10)
			{
				m_szString[m_nString++] = L'ホ';
			}
			else{ flag = false; }
		}
		break;
	// 4画
	case 5:
		{
			if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_5 && m_nType[3] == SignalType_13)
			{
				m_szString[m_nString++] = L'ズ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_10 && m_nType[3] == SignalType_13)
			{
				m_szString[m_nString++] = L'ボ';
			}
			else if (m_nType[0] == SignalType_1 && m_nType[1] == SignalType_2 && m_nType[2] == SignalType_10 && m_nType[3] == SignalType_14)
			{
				m_szString[m_nString++] = L'ポ';
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

// 原画判定
SignalType Semaphore::JudgeSemaphore(const D2D1_POINT_2F* pJointPoints)
{
	SignalType nType = SignalType_Undefined;

	if (!pJointPoints){ return nType; }

	DirectX::SimpleMath::Vector2	m_geRight;	// 右腕
	DirectX::SimpleMath::Vector2	m_geLeft;	// 左腕

	m_geLeft.x = pJointPoints[JointType_WristLeft].x - pJointPoints[JointType_ShoulderLeft].x;
	m_geLeft.y = pJointPoints[JointType_WristLeft].y - pJointPoints[JointType_ShoulderLeft].y;

	m_geRight.x = pJointPoints[JointType_WristRight].x - pJointPoints[JointType_ShoulderRight].x;
	m_geRight.y = pJointPoints[JointType_WristRight].y - pJointPoints[JointType_ShoulderRight].y;
	//m_geLeft.x = pJointPoints[JointType_WristLeft].x - pJointPoints[JointType_ElbowLeft].x;
	//m_geLeft.y = pJointPoints[JointType_WristLeft].y - pJointPoints[JointType_ElbowLeft].y;

	//m_geRight.x = pJointPoints[JointType_WristRight].x - pJointPoints[JointType_ElbowRight].x;
	//m_geRight.y = pJointPoints[JointType_WristRight].y - pJointPoints[JointType_ElbowRight].y;

	int Seg[2] = { 0 };

	Seg[0] = JudgeSegment(m_geRight, 1);	// 右腕
	Seg[1] = JudgeSegment(m_geLeft, 1, false);		// 左腕

	// ベクトルの長さも判断基準に追加する。
	if (Seg[0] == 2 && Seg[1] == 2)
	{
		nType = SignalType_Origin;
	}
	// 第1原画
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

// 肩を中心に８分割した領域にあるかで判断している。
// 第６画を判断する際、左手を考慮する。
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

	// 第6画の左手を想定した判定
	if (!bRight && fabs(fAngle - XM_PI / 5.0) < m_fTol[nLevel] && vec.y < 0.0)
	{
		nSegmentType = 0;
	}

	return nSegmentType;
}

// 本来は音声認識にて指定した文字の手旗を表示させたいが、文字はここでのハードコーディング
void Semaphore::Practice(INT64 nTime, D2D1_POINT_2F* pJointPoints, ImageRenderer* pRenderer)
{
	if (m_nAnimationTimer == 0){ m_nAnimationTimer = nTime; }
	// アニメーションタイマーの差分にて処理を分岐させる。つまり、アニメーション。

	// 文字によって決まったポーズを一定時間表示させる。
	// 以下でだいたい1秒
	if (nTime - m_nAnimationTimer > 10000000 && nTime - m_nAnimationTimer < 50000000)
	{
		m_szString[0] = L'ア';
		m_szString[1] = L'\0';
		m_nString = 1;
		pRenderer->DrawChar(m_szString, m_nString, pJointPoints[JointType_SpineMid]);
	}
	// アのパターン
	m_nType[0] = SignalType_9;
	m_nType[1] = SignalType_3;
	m_nType[2] = SignalType_Origin;
	m_nType[3] = SignalType_Undefined;

	// 2次元だけどZ軸の回転をクォータニオンにて
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

	// ポーズの変更はここで行い、描画は呼び出し元
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

// 指定文字のポーズを判定する
bool Semaphore::JudgeType()
{
	bool flag = true;
	if (m_nString > 63){ return false; }
	// 初期化
	for (int i = 0; i < ONECHARMAX; i++){ m_nType[i] = SignalType_Undefined; }

	switch (m_nSignalIndex)
	{
		// 1画
	case 2:
	{
			  if (m_szString[m_nString] == L'ク'){ m_nType[0] = SignalType_11; }
			  else if (m_szString[m_nString] == L'ニ'){ m_nType[0] = SignalType_6; }
			  else if (m_szString[m_nString] == L'ノ'){ m_nType[0] = SignalType_3; }
			  else if (m_szString[m_nString] == L'ハ'){ m_nType[0] = SignalType_10; }
			  else if (m_szString[m_nString] == L'フ'){ m_nType[0] = SignalType_9; }
			  else if (m_szString[m_nString] == L'ヘ'){ m_nType[0] = SignalType_4; }
			  else if (m_szString[m_nString] == L'リ'){ m_nType[0] = SignalType_12; }
			  else if (m_szString[m_nString] == L'レ'){ m_nType[0] = SignalType_7; }
			  else{ flag = false; }
	}
		break;
		// 2画
	case 3:
	{
			  if (m_szString[m_nString] == L'ア')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'イ')
			  {
				  m_nType[0] = SignalType_3;
				  m_nType[1] = SignalType_2;
			  }
			  else if (m_szString[m_nString] == L'ウ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'カ')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'グ')
			  {
				  m_nType[0] = SignalType_11;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'キ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_2;
			  }
			  else if (m_szString[m_nString] == L'ケ')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'コ')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'サ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_12;
			  }
			  else if (m_szString[m_nString] == L'シ')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'セ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'ソ')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'タ')
			  {
				  m_nType[0] = SignalType_11;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'チ')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_2W;
			  }
			  else if (m_szString[m_nString] == L'ツ')
			  {
				  m_nType[0] = SignalType_12;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'テ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'ト')
			  {
				  m_nType[0] = SignalType_2;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'ナ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'ヌ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_4;
			  }
			  else if (m_szString[m_nString] == L'バ')
			  {
				  m_nType[0] = SignalType_10;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'パ')
			  {
				  m_nType[0] = SignalType_10;
				  m_nType[1] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'ヒ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'ブ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'プ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'ベ')
			  {
				  m_nType[0] = SignalType_4;
				  m_nType[1] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ペ')
			  {
				  m_nType[0] = SignalType_4;
				  m_nType[1] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'マ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'ミ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'ム')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'メ')
			  {
				  m_nType[0] = SignalType_3;
				  m_nType[1] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'モ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'ヤ')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_4;
			  }
			  else if (m_szString[m_nString] == L'ユ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'ヨ')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_6;
			  }
			  else if (m_szString[m_nString] == L'ラ')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'ル')
			  {
				  m_nType[0] = SignalType_3;
				  m_nType[1] = SignalType_7;
			  }
			  else if (m_szString[m_nString] == L'ロ')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_8;
			  }
			  else if (m_szString[m_nString] == L'ワ')
			  {
				  m_nType[0] = SignalType_2;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'ヲ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_9;
			  }
			  else if (m_szString[m_nString] == L'ン')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_1;
			  }
			  else{ flag = false; }
	}
		break;
		// 3画
	case 4:
	{
			  if (m_szString[m_nString] == L'エ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2W;
				  m_nType[2] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'オ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_3;
			  }
			  else if (m_szString[m_nString] == L'ガ')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ギ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ゲ')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ゴ')
			  {
				  m_nType[0] = SignalType_8;
				  m_nType[1] = SignalType_1;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ザ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_12;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ジ')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ス')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_5;
			  }
			  else if (m_szString[m_nString] == L'ゼ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ゾ')
			  {
				  m_nType[0] = SignalType_5;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ダ')
			  {
				  m_nType[0] = SignalType_11;
				  m_nType[1] = SignalType_5;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ヂ')
			  {
				  m_nType[0] = SignalType_7;
				  m_nType[1] = SignalType_2W;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ヅ')
			  {
				  m_nType[0] = SignalType_12;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'デ')
			  {
				  m_nType[0] = SignalType_6;
				  m_nType[1] = SignalType_3;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ド')
			  {
				  m_nType[0] = SignalType_2;
				  m_nType[1] = SignalType_5;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ネ')
			  {
				  m_nType[0] = SignalType_9;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_1;
			  }
			  else if (m_szString[m_nString] == L'ビ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ピ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_7;
				  m_nType[2] = SignalType_14;
			  }
			  else if (m_szString[m_nString] == L'ホ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_10;
			  }
			  else{ flag = false; }
	}
		break;
		// 4画
	case 5:
	{
			  if (m_szString[m_nString] == L'ズ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_5;
				  m_nType[3] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ボ')
			  {
				  m_nType[0] = SignalType_1;
				  m_nType[1] = SignalType_2;
				  m_nType[2] = SignalType_10;
				  m_nType[3] = SignalType_13;
			  }
			  else if (m_szString[m_nString] == L'ポ')
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
