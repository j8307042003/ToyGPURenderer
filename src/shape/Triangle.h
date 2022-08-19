#pragma once

#include "math/Vec3.h"
#include "Shape.h"

class Triangle : public Shape {
public:
	// Vec3 position;
	Vec3 Vertices[3];
	Vec3 uv[3];
	Triangle();
	Triangle(Vec3 v1, Vec3 v2, Vec3 v3 );

	Vec3 normal[3];

	bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction) override;
	virtual ShapeType Type() override {return ShapeType::Triangle;}


private:
	Vec3 _normal[3];

	Vec3 make_normal(const Vec3 & v1, const Vec3 & v2, const Vec3 & v3);
};



inline bool IntersectTriangle(const glm::dvec3 & v0, const glm::dvec3 & v1, const glm::dvec3 & v2, const glm::dvec3 & normal, const Ray3f& ray, glm::dvec3& hit, glm::dvec2 & uv)
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
  uv = {u, v};
  return true;
}

/*
void IntersectSIMD(const glm::dvec3& v1, const glm::dvec3& v2, const glm::dvec3& v3)
{
	const float EPSILON = 1e-9f;
	__m128 dx4 = _mm_set_ps1(0);
	__m128 dy4 = _mm_set_ps1(0);
	__m128 dz4 = _mm_set_ps1(0);
	__m128 Py4 = _mm_set_ps1(0);
	__m128 ox4 = _mm_set_ps1(0);
	__m128 oy4 = _mm_set_ps1(0);
	__m128 oz4 = _mm_set_ps1(0);
	__m128 t4 = _mm_set_ps1(0);
	__m128 EPS4 = _mm_set_ps1(EPSILON);
	__m128 MINUSEPS4 = _mm_set_ps1(-EPSILON);
	__m128 ONE4 = _mm_set_ps1(1.0f);
	__m128 e1x4 = _mm_set_ps1(v2.x - v1.x);
	__m128 e1y4 = _mm_set_ps1(v2.y - v1.y);
	__m128 e1z4 = _mm_set_ps1(v2.z - v1.z);
	__m128 e2x4 = _mm_set_ps1(v3.x - v1.x);
	__m128 e2y4 = _mm_set_ps1(v3.y - v1.y);
	__m128 e2z4 = _mm_set_ps1(v3.z - v1.z);
	__m128 hx4 = _mm_sub_ps(_mm_mul_ps(dy4, e2z4), _mm_mul_ps(dz4, e2y4));
	__m128 hy4 = _mm_sub_ps(_mm_mul_ps(dz4, e2x4), _mm_mul_ps(dx4, e2z4));
	__m128 hz4 = _mm_sub_ps(_mm_mul_ps(dx4, e2y4), _mm_mul_ps(dy4, e2x4));
	__m128 det4 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(e1x4, hx4), _mm_mul_ps(e1y4, Py4)), _mm_mul_ps(e1z4, hz4));
	__m128 mask1 = _mm_or_ps(_mm_cmple_ps(det4, MINUSEPS4), _mm_cmpge_ps(det4, EPS4));
	__m128 inv_det4 = _mm_rcp_ps(det4);
	__m128 sx4 = _mm_sub_ps(ox4, _mm_set_ps1(v1.x));
	__m128 sy4 = _mm_sub_ps(oy4, _mm_set_ps1(v1.y));
	__m128 sz4 = _mm_sub_ps(oz4, _mm_set_ps1(v1.z));
	__m128 u4 = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(sx4, hx4), _mm_mul_ps(sy4, hy4)), _mm_mul_ps(sz4, hz4)), inv_det4);
	__m128 mask2 = _mm_and_ps(_mm_cmpge_ps(u4, _mm_setzero_ps()), _mm_cmple_ps(u4, ONE4));
	__m128 qx4 = _mm_sub_ps(_mm_mul_ps(sy4, e1z4), _mm_mul_ps(sz4, e1y4));
	__m128 qy4 = _mm_sub_ps(_mm_mul_ps(sz4, e1x4), _mm_mul_ps(sx4, e1z4));
	__m128 qz4 = _mm_sub_ps(_mm_mul_ps(sx4, e1y4), _mm_mul_ps(sy4, e1x4));
	__m128 v4 = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(dx4, qx4), _mm_mul_ps(dy4, qy4)), _mm_mul_ps(dz4, qz4)), inv_det4);
	__m128 mask3 = _mm_and_ps(_mm_cmpge_ps(v4, _mm_setzero_ps()), _mm_cmple_ps(_mm_add_ps(u4, v4), ONE4));
	__m128 newt4 = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(e2x4, qx4), _mm_mul_ps(e2y4, qy4)), _mm_mul_ps(e2z4, qz4)), inv_det4);
	__m128 mask4 = _mm_cmpgt_ps(newt4, _mm_setzero_ps());
	__m128 mask5 = _mm_cmplt_ps(newt4, t4);
	__m128 combined = _mm_and_ps(_mm_and_ps(_mm_and_ps(_mm_and_ps(mask1, mask2), mask3), mask4), mask5);
	t4 = _mm_blendv_ps(t4, newt4, combined);
}
*/