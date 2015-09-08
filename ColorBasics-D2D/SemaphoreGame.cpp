#include "SemaphoreGame.h"
using namespace DirectX;
using namespace SimpleMath;

wchar_t g_szTitle[] = { L"�ΐ탂�[�h" };

CSemaphoreGame::CSemaphoreGame(ImageRenderer* pRenderer, ICoordinateMapper* pMapper)
	: CGameBase(pRenderer),
	m_pCoordinateMapper(pMapper)
{
	for (int i = 0; i < BODY_COUNT; ++i)
	{
		m_pPlayer[i] = NULL;
	}
	m_nPlayer = 0;

	m_nStatus = 0;
}

CSemaphoreGame::~CSemaphoreGame()
{
	Clear();
}

void CSemaphoreGame::Clear()
{
	for (int i = 0; i < BODY_COUNT; i++)
	{
		if (m_pPlayer[i]){ delete m_pPlayer[i]; }
	}

	return;
}

// �Q�[�����̉�ʕ\��
// �X�e�[�^�X�ɉ����ă��b�Z�[�W��{�^���̕`����s��
void CSemaphoreGame::Display(TIMESPAN nTime)
{
	// �v���C���[�F��
	Player(m_nPlayer);

	// ��ʏ㕔�Ɂu�ΐ탂�[�h�v�̕\��
	// ������v���C���[���m�肵�Ă���
	if (m_nPlayer){ Title(g_szTitle); }

	// �X�^�[�g�{�^��
	if (m_nStatus > 0)
	{
		Button(nTime, ButtonStart);

		// �X�^�[�g�{�^���̈ʒu�ɋ߂��A�v���C���[�̎�ɃJ�[�\����\��������B
		D2D1_POINT_2F button;
		D2D1_POINT_2F hand;
		GetButtonPos(button);
		CloseHand(button, hand);

		SetCursor(hand);

		if (IsButtonHover(hand))
		{
			
		}
	}

	return;
}

// �w��|�C���g�Ɉ�ԋ߂��v���C���[�̎�̈ʒu��Ԃ�
void CSemaphoreGame::CloseHand(D2D1_POINT_2F button, D2D1_POINT_2F& hand)
{
	if (!m_nPlayer){ return; }

	Vector2 orig;
	orig.x = button.x;
	orig.y = button.y;

	D2D1_POINT_2F joint[2];
	Vector2 vec[2];
	float	fDistance[2] = { 0.0 };
	float	fMin = FLT_MAX;

	CSemaphorePlayer* pPlayer = NULL;
	for (int i = 0; i < BODY_COUNT; i++)
	{
		pPlayer = m_pPlayer[i];
		if (pPlayer)
		{
			joint[0] = pPlayer->GetJoints(JointType_HandLeft);
			joint[1] = pPlayer->GetJoints(JointType_HandRight);

			vec[0].x = joint[0].x;
			vec[0].y = joint[0].y;

			vec[1].x = joint[1].x;
			vec[1].y = joint[1].y;

			fDistance[0] = Vector2::Distance(orig, vec[0]);
			fDistance[1] = Vector2::Distance(orig, vec[1]);

			if (fDistance[0] < fMin){ fMin = fDistance[0]; hand = joint[0]; }
			if (fDistance[1] < fMin){ fMin = fDistance[1]; hand = joint[1]; }
		}
	}

	return;
}

// �v���C���[�F�����͂��ꂾ���̏����Ƃ���B
// �Q�[�����ɑ��̐l�Ԃ��g���b�N���Ă���������B
void CSemaphoreGame::SetPlayer(int nBodyCount, IBody** ppBodies)
{
	if (m_nStatus > 0){ return; }
	if (m_nPlayer >= BODY_COUNT){ return; }

	HRESULT hr = S_OK;
	// �X�e�[�^�X�ɂ���ď����𕪊�
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			// �����l�ł̑ΐ탂�[�h��z�肵�ăC���f�b�N�X���擾����B
			// �{���̓Q�[���O�ɑΐ�̓�l���t�B�b�N�X���Ă����ׂ����낤���B
			// 
			// �g���b�L���O����Ă���{�f�B���͂����Ɗm���߂邱�ƁB
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);
			if (SUCCEEDED(hr) && bTracked)
			{
				// �g���b�L���O�������ȏꍇ�̃C���f�b�N�X�͂O���Ԃ�̂Ŏg�����ɒ��ӁI�I
				UINT64 nBodyIndex = 0;
				hr = pBody->get_TrackingId(&nBodyIndex);

				CSemaphorePlayer* pPlayer = new CSemaphorePlayer();
				if (pPlayer)
				{
					pPlayer->SetPlayer(nBodyIndex);
					m_pPlayer[m_nPlayer++] = pPlayer;
				}
			}
		}
	}
	if (m_nPlayer > 0){ m_nStatus = 1; }

	return;
}

// �X�^�[�g�{�^������
// �Ƃɂ����{�^���������ǂ����邩
// �I����

// �Q�[��
int CSemaphoreGame::Play(TIMESPAN nTime, int nBodyCount, IBody** ppBodies)
{
	SetRelativeTime(nTime);

	// �X�e�[�^�X����
	// �X�e�[�^�X�͎��ԂƁE�E�E�Ŕ��f����
	TIMESPAN nSpan = nTime - GetRelativeTime();
	//if (m_nStatus == 0 && nSpan > 2000)
	//{
	//	bPlayer = true;
	//	m_nStatus++;
	//}

	// �v���C���[�F��������B
	// �����^�C�~���O�Ńg���b�L���O����Ă���Ƃ͌���Ȃ��B
	SetPlayer(nBodyCount, ppBodies);

	HRESULT hr = S_OK;
	// �X�e�[�^�X�ɂ���ď����𕪊�
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			// �����l�ł̑ΐ탂�[�h��z�肵�ăC���f�b�N�X���擾����B
			// �{���̓Q�[���O�ɑΐ�̓�l���t�B�b�N�X���Ă����ׂ����낤���B
			// 
			// �g���b�L���O����Ă���{�f�B���͂����Ɗm���߂邱�ƁB
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);
			if (SUCCEEDED(hr) && bTracked)
			{
				// �g���b�L���O�������ȏꍇ�̃C���f�b�N�X�͂O���Ԃ�̂Ŏg�����ɒ��ӁI�I
				UINT64 nBodyIndex = 0;
				hr = pBody->get_TrackingId(&nBodyIndex);
				if (!SUCCEEDED(hr)){ continue; }

				CSemaphorePlayer* pPlayer = NULL;
				for (int j = 0; j < BODY_COUNT; j++)
				{
					pPlayer = m_pPlayer[j];
					if (pPlayer && ((CGestureSemaphore*)pPlayer)->ItsMe(nBodyIndex))
					{
						Joint joints[JointType_Count];
						D2D1_POINT_2F jointPoints[JointType_Count];
						hr = pBody->GetJoints(_countof(joints), joints);
						if (SUCCEEDED(hr))
						{
							// �X�N���[�����W�ɕϊ�
							for (int j = 0; j < _countof(joints); ++j)
							{
								jointPoints[j] = BodyToScreen(joints[j].Position);
							}
							pPlayer->Play(jointPoints);
						}
						break;
					}
				}
			}
		}
	}

	return 0;
}
/// <summary>
/// Converts a body point to screen space
/// </summary>
/// <param name="bodyPoint">body point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F CSemaphoreGame::BodyToScreen(const CameraSpacePoint& bodyPoint)
{
	// Calculate the body's position on the screen
	//DepthSpacePoint depthPoint = { 0 };
	//m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
	// �J���[�t���[���Ƀ}�b�s���O����
	ColorSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToColorSpace(bodyPoint, &depthPoint);

	// �}�b�s���O�����炻�̂܂܂̍��W��
	//float screenPointX = static_cast<float>(depthPoint.X * width) / cColorWidth;
	//float screenPointY = static_cast<float>(depthPoint.Y * height) / cColorHeight;
	float screenPointX = static_cast<float>(depthPoint.X);
	float screenPointY = static_cast<float>(depthPoint.Y);

	return D2D1::Point2F(screenPointX, screenPointY);
}
