#include "GestureBase.h"


CGestureBase::CGestureBase():
	m_nBodyIndex(BODYINDEX_INIT),
	m_nStartTime(0),
	m_bOk(false)
{
}

CGestureBase::~CGestureBase()
{
}

void CGestureBase::Clear()
{
	m_nStartTime = 0;
	m_bOk = false;
}

// トラッキング設定
// ボディインデックスの確認
bool CGestureBase::ItsMe(UINT64 nIndex)
{
	if (m_nBodyIndex != BODYINDEX_INIT && m_nBodyIndex != nIndex){ return false; }

	m_nBodyIndex = nIndex;
	return true;
}

bool CGestureBase::JudgeTime(INT64 time)
{
	if (m_nStartTime - time > 1000000)
	{
		m_bOk = true;
	}

	return m_bOk;
}

//
// End Of File