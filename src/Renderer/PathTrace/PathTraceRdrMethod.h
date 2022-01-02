#pragma once
#include "IRenderMethod.h"


class PathTraceRdrMethod : public IRenderMethod
{
	virtual void Sample(RenderData * rdrData, int x, int y) override;
}