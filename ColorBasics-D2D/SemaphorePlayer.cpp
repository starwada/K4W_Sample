#include "SemaphorePlayer.h"


CSemaphorePlayer::CSemaphorePlayer()
{
}

CSemaphorePlayer::~CSemaphorePlayer()
{
}

bool CSemaphorePlayer::SetPlayer(UINT64 nIndex)
{
	return ItsMe(nIndex);
}

// 手旗ゲーム
// 
int CSemaphorePlayer::Play(D2D1_POINT_2F jointPoints[JointType_Count])
{
	// スクリーン座標に変換
	for (int i = 0; i < _countof(m_jointPoints); ++i)
	{
		m_jointPoints[i] = jointPoints[i];
	}


	return 0;
}
