#include "Util/GameTimer.h"

GameTimer::GameTimer() : _secondsPerCount(0.0), _deltaTime(-1.0), _baseTime(0),
_pausedTime(0), _stopTime(0), _prevTime(0), _currentTime(0), _isStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	_secondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::totalTime() const
{
	if (_isStopped)
	{
		return (float)(((_stopTime - _pausedTime) - _baseTime) * _secondsPerCount);
	}

	return (float)(((_currentTime - _pausedTime) - _baseTime) * _secondsPerCount);
}

float GameTimer::deltaTime() const
{
	return (float)_deltaTime;
}

void GameTimer::reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	_baseTime = currentTime;
	_prevTime = currentTime;
	_stopTime = 0;
	_isStopped = false;
}

void GameTimer::start()
{
	if (!_isStopped)
		return;

	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	_pausedTime += startTime - _stopTime;
	_prevTime = startTime;
	_stopTime = 0;
	_isStopped = false;
}

void GameTimer::stop()
{
	if (_isStopped)
		return;

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	_stopTime = currentTime;
	_isStopped = true;
}

void GameTimer::tick()
{
	if (_isStopped)
	{
		_deltaTime = 0.0;
		return;
	}

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	this->_currentTime = currentTime;

	_deltaTime = (currentTime - _prevTime) * _secondsPerCount;
	_prevTime = currentTime;

	if (_deltaTime < 0.0)
	{
		_deltaTime = 0.0;
	}
}
