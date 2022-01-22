#pragma once

#include "math/Vec3.h"
#include "Shape.h"

class Triangle : public Shape {
public:
	// Vec3 position;
	Vec3 Vertices[3];

	Triangle(Vec3 v1, Vec3 v2, Vec3 v3 );

	const Vec3 & normal;

	bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction) override;
	virtual ShapeType Type() override {return ShapeType::Triangle;}


private:
	Vec3 _normal;

	Vec3 make_normal(const Vec3 & v1, const Vec3 & v2, const Vec3 & v3);
};



inline bool IntersectTriangle(const glm::dvec3 & v0, const glm::dvec3 & v1, const glm::dvec3 & v2, const glm::dvec3 & normal, const Ray3f& ray, glm::dvec3& hit)
{
  
  // compute triangle edges
  auto edge1 = v1 - v0;
  auto edge2 = v2 - v0;

  // compute determinant to solve a linear system
  auto pvec = glm::cross(ray.direction, edge2);
  auto det  = glm::dot(edge1, pvec);

  // check determinant and exit if triangle and ray are parallel
  // (could use EPSILONS if desired)
  if (det == 0) return false;
  auto inv_det = 1.0f / det;
  // compute and check first bricentric coordinated
  auto tvec = ray.origin - v0;
  auto u    = glm::dot(tvec, pvec) * inv_det;
  if ((u < 0) | (u > 1)) return false;

  // compute and check second bricentric coordinated
  auto qvec = glm::cross(tvec, edge1);
  auto v    = glm::dot(ray.direction, qvec) * inv_det;
  if ((v < 0) | (u + v > 1)) return false;

  // compute and check ray parameter
  auto t = glm::dot(edge2, qvec) * inv_det;
  if (t < 0.1) return false;

  // intersection occurred: set params and exit
  hit = ray.origin + t * ray.direction;
  return true;




}
