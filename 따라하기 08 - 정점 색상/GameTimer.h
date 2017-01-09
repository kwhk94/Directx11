#pragma once

const ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균한다.

class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer();

	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTimeElapsed();

private:
	bool m_bHardwareHasPerformanceCounter;				//컴퓨터가 Perforamce Conter를 가지고 있는가?
	float m_fTimeScale;								//Scale Counter의 양
	float m_fTimeElapsed;							///마지막 프레임이 지나간 시간
	__int64 m_nCurrentTime;							//현재시간
	__int64 m_nLastTime;							//마지막 프레임 시간
	__int64 m_PerformanceFrequency;					//컴퓨터의.Performance Frequency

	float m_fFrameTime[MAX_SAMPLE_COUNT];			//프레임 시간을 누적하기 위한 배열
	ULONG m_nSampleCount;							//누적된 프레임 횟수

	unsigned long m_nCurrentFrameRate;		//프레임 레이트
	unsigned long m_FramePerSecond;			//초당 프레임 수
	float m_fFPSTimeElapsed;				//프레임 레이트 계산 소요시간
};

