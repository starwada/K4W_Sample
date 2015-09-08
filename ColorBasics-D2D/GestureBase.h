//
// ジェスチャー判定基底クラス
//
#pragma once
#include "stdafx.h"

// ジェスチャー判定用の幾何計算はこのクラスで行う。
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

	// 判定純粋仮想関数（派生先で実装する）
	virtual bool Judge(INT64 nTime, UINT64 nIndex, const D2D1_POINT_2F* pJoints) = 0;
	
	bool	isOk(){ return m_bOk; };

	void	Clear();

private:
	UINT64	m_nBodyIndex;			// ボディインデックス 異なるボディで判定しても仕方ない
	INT64	m_nStartTime;
	bool	m_bOk;


};

