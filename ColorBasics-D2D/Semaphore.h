// 手旗信号解読クラス

#pragma once
// Direct2D Header Files
#include <d2d1.h>
// Kinect Header files
#include <Kinect.h>

// DirectXTK(Tool Kit)
#include <SimpleMath.h>
#include "ImageRenderer.h"

#include "GestureSemaphore.h"

#define	ONECHARMAX	5			// 1文字を表現するに必要なサイズ
						// 「ス」は３つのパターンで濁点、半濁点を考慮すると、原姿を入れて５つ必要。
// 肩を中心とし手首までを半径とした円を想定する。// 画面X+方向を０度とし45度の円弧区間を考える。
// X+軸を基準に、時計回りに±22.5°を区間０（右方向）。
// +22.5°->+67.5° 区間１（右下）
// +67.5°->+112.5°区間２（下）
// +112.5°->+157.5°区間３（左下）
// +157.5°->-157.5°区間４（左）
// -157.5°->-112.5°区間５（左上）
// -112.5°->-67.5°区間６（上）
// -67.5°->-22.5°区間７（右上）

class Semaphore : public CGestureSemaphore
{
public:
	Semaphore( UINT64* pIndex ) ;
	~Semaphore();

	bool		JudgeSemaphore(INT64 time, UINT64 nIndex, const D2D1_POINT_2F* pJoints);
	// 自分したポーズを認識させる
	void		SetSignalType(UINT64* pIndex, const D2D1_POINT_2F* pJointPoints, ImageRenderer* pRenderer);

	// 練習用にポーズを表示させる。自分のボディーを使って。
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

	UINT64		m_nBodyIndex;		// ボディインデックス（複数人での対戦を想定して）
	int		m_nSignalIndex;		// 一文字判定の位置（インデックス）
	int		m_nCount;		// 判定回数

	SignalType	m_Prev;
	SignalType	m_nType[ONECHARMAX];	// 一文字 原姿を含め最大5ポーズなので

	wchar_t		m_szString[64];		// 手旗文字列
	int		m_nString;
	bool		m_bStringDraw;
	int		m_nDrawCount;

	D2D1_POINT_2F	m_oSpineMid;

	float		m_fTol[3];

	SignalType	m_nDebug[300];
	int		m_nDCount;

	INT64		m_nAnimationTimer;	// アニメーション用タイマー

	bool		JudgeString();
	bool		JudgeType();

	// 原画判定
	SignalType	JudgeSemaphore(const D2D1_POINT_2F* pJointPoints);
	int		JudgeSegment(DirectX::SimpleMath::Vector2& vec, int nLevel, bool bRight=true);

};

