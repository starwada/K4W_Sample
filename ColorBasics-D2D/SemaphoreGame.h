#pragma once
#include "GameBase.h"
#include "SemaphorePlayer.h"

// (手旗)ゲームのステータスには
// 基本どのようなゲームにおいてもだいたい同じ内容が想定されるが
//
// 0 開始前
// 1 人数（プレイヤー）確定
// 2 ゲームスタート前
// 3 ゲーム中
// 4 ゲーム終了
// 5 判定
// ステータスが変わったら保持している時間も更新していいか

// 手旗ゲームクラス
// お題は当然ゲームクラスが管理する
//
class CSemaphoreGame : public CGameBase
{
public:
	CSemaphoreGame(ImageRenderer* pRenderer, ICoordinateMapper* pMapper);
	~CSemaphoreGame();

	void	Clear();

	// ゲーム中の画面表示
	void	Display(TIMESPAN nTime);
	//
	void	CloseHand(D2D1_POINT_2F button, D2D1_POINT_2F& hand);

	// ゲーム
	int		Play(TIMESPAN nTime, int nBodyCount, IBody** ppBodies);

	void	SetPlayer(int nBodyCount, IBody** ppBodies);


private:
	ICoordinateMapper*	m_pCoordinateMapper;

	CSemaphorePlayer*	m_pPlayer[BODY_COUNT];	// プレイヤー
	UINT64				m_nPlayer;				// 人数

	UINT64				m_nStatus;				// ゲームステータス　今どの状態か

	D2D1_POINT_2F		BodyToScreen(const CameraSpacePoint& bodyPoint);

};

