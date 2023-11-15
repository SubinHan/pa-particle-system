#pragma once
#include <Windows.h>

class GameTimer
{
public:
	GameTimer();

	float totalTime() const;
	double deltaTime() const;

	void reset();
	void start();
	void stop();
	void tick();

private:
	double	_secondsPerCount;
	double	_deltaTime;

	__int64 _baseTime;
	__int64 _pausedTime;
	__int64 _stopTime;
	__int64 _prevTime;
	__int64 _currentTime;

	bool	_isStopped;
};