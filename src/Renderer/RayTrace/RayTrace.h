#ifndef RAYTRACE_H
#define RAYTRACE_H
#pragma once
#include <glm/vec3.hpp>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <math/Ray.h>

struct SceneData;
bool Ray_PrimitiveIntersect(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max, int primitiveIdx, glm::dvec3& hit, glm::dvec3& normal, glm::vec2& uv);
bool RayTrace(const SceneData& sceneData, const Ray3f& ray, float t_min, float t_max, glm::dvec3& rayHitPosition, glm::dvec3& normal, int& shapeIndex);

#endif
