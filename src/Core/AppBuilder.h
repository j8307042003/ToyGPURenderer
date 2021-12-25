#pragma once

#include "Application.h"
#include "InteractiveApp.h"

class AppBuilder
{
public:
	static Application * MakeApp(const char * args)
	{
		Application* app = new InteractiveApp(args);

		return app;
	}
};