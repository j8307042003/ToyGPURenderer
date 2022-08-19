#pragma once

#include "Application.h"
#include "InteractiveApp.h"
#include "DirectRenderApp.h"

class AppBuilder
{
public:
	static Application * MakeApp(int argc, char *argv[])
	{
		if (argc == 0)
		{
			//Application* app = new InteractiveApp(argv);

			Application* app = new DirectRenderApp(argv);

			return app;
		}


		return new InteractiveApp(argv);
		//return new DirectRenderApp(argv);
	}
};