//
// 手旗用判定クラス
//
#pragma once
// Direct2D Header Files
#include <d2d1.h>
// Kinect Header files
#include <Kinect.h>

// DirectXTK(Tool Kit)
#include <SimpleMath.h>
#include "GestureBase.h"

// 手旗原画タイプ
// 第２原画が2種類あるのでどうしようか。

#ifndef _SignalType_
#define _SignalType_
typedef enum _SignalType SignalType;

enum _SignalType
{
	SignalType_Undefined = -1,		// 不明
	SignalType_Origin = 0,			// 原姿
	SignalType_0 = 1,			// 零原画 本当は動きがある（円を描く）
	SignalType_1 = 2,			// 第1原画
	SignalType_2 = 3,			// 第2原画
	SignalType_2W = 4,			// 第2原画白旗が上
	SignalType_3 = 5,			// 第3原画
	SignalType_4 = 6,
	SignalType_5 = 7,
	SignalType_6 = 8,
	SignalType_7 = 9,
	SignalType_8 = 10,
	SignalType_9 = 11,
	SignalType_10 = 12,
	SignalType_11 = 13,			// 第11原画 これも動きがある
	SignalType_12 = 14,
	SignalType_13 = 15,
	SignalType_14 = 16
};
#endif // _SignalType_

class CGestureSemaphore :
	public CGestureBase
{
public:
	CGestureSemaphore();
	~CGestureSemaphore();

	bool	ItsMe(UINT64 nIndex);

	bool Judge(INT64 time, UINT64 nIndex, const D2D1_POINT_2F* pJoints);

	SignalType	GetSignalType(){ return m_nSignalType; };

private:
	SignalType	m_nSignalType;
	SignalType	m_Prev;

	float		m_fTol[3];

	// 原画判定
	SignalType	JudgeSemaphore(const D2D1_POINT_2F* pJointPoints);
	int		JudgeSegment(DirectX::SimpleMath::Vector2& vec, int nLevel, bool bRight = true);

};

