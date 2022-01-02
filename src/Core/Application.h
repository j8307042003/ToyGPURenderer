#pragma once
#include "ImguiUI.h"


class Application
{
public:
	virtual ~Application() {};
	virtual void Run() = 0;
	virtual void AddUI(ImguiUI * ui) = 0;
	virtual void RemoveUI(ImguiUI * ui) = 0;
};