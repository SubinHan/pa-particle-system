#pragma once

class IWindow
{
public:
	virtual void show() = 0;
	virtual bool isAlive() = 0;
};