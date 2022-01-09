#pragma once
#include "IRenderMethod.h"


class PathTraceRdrMethod : public IRenderMethod
{
public:
	virtual glm::vec3 Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes) override;
};