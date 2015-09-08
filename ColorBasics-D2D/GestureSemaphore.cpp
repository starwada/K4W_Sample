#include "GestureSemaphore.h"

using namespace DirectX;
using namespace SimpleMath;

CGestureSemaphore::CGestureSemaphore():
m_nSignalType(SignalType_Undefined),
m_Prev(SignalType_Undefined)
{
	m_fTol[0] = 0.3;
	m_fTol[1] = 0.5;
	m_fTol[2] = 0.7;
}

CGestureSemaphore::~CGestureSemaphore()
{
}

// トラッキング設定
// ボディインデックスの確認
bool CGestureSemaphore::ItsMe(UINT64 nIndex)
{
	return CGestureBase::ItsMe(nIndex);
}

//
// ジェスチャー判定
// 一定時間あるジェスチャーと認識されるとtrueを返す。
bool CGestureSemaphore::Judge(INT64 time, UINT64 nBodyIndex, const D2D1_POINT_2F* pJoints)
{
	if (pJoints == NULL){ return false; }
	bool bJudge = false;

	// トラッキングインデックスチェック
	if (!ItsMe(nBodyIndex)){ return false; }

	// ジェスチャー 原画判定
	m_nSignalType = JudgeSemaphore(pJoints);
	// 判定OKな条件
	if (m_nSignalType != SignalType_Undefined &&
		m_Prev == m_nSignalType &&
		JudgeTime(time))
	{
		bJudge = true;
	}
	// 時間初期化
	else if (m_Prev == SignalType_Undefined)
	{
		Start(time);
	}
	m_Prev = m_nSignalType;

	return bJudge;
}

// 原画判定
SignalType CGestureSemaphore::JudgeSemaphore(const D2D1_POINT_2F* pJointPoints)
{
	SignalType nType = SignalType_Undefined;

	if (!pJointPoints){ return nType; }

	DirectX::SimpleMath::Vector2	m_geRight;	// 右腕
	DirectX::SimpleMath::Vector2	m_geLeft;	// 左腕

	m_geLeft.x = pJointPoints[JointType_WristLeft].x - pJointPoints[JointType_ShoulderLeft].x;
	m_geLeft.y = pJointPoints[JointType_WristLeft].y - pJointPoints[JointType_ShoulderLeft].y;

	m_geRight.x = pJointPoints[JointType_WristRight].x - pJointPoints[JointType_ShoulderRight].x;
	m_geRight.y = pJointPoints[JointType_WristRight].y - pJointPoints[JointType_ShoulderRight].y;

	int Seg[2] = { 0 };

	Seg[0] = JudgeSegment(m_geRight, 1);		// 右腕
	Seg[1] = JudgeSegment(m_geLeft, 1, false);	// 左腕

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
		//m_nCount = 0;
		//return;
	}

	return nType;
}

// 肩を中心に８分割した領域にあるかで判断している。
// 第６画を判断する際、左手を考慮する。
int CGestureSemaphore::JudgeSegment(DirectX::SimpleMath::Vector2& vec, int nLevel, bool bRight/* =true*/)
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
