#include "Triangle.h"
#include "math/Vec3.h"
#include <iostream>

Triangle::Triangle(Vec3 v1, Vec3 v2, Vec3 v3 ) : 
	Vertices{v1, v2, v3}, _normal( make_normal(v1, v2, v3) ), normal(_normal)
{
	

}


bool Triangle::RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction) {
	/*
	if (Vec3::Dot(ray->dir, normal) == 0 ) return false;

	float d = normal.x * Vertices[0].x + normal.y * Vertices[1].y + normal.z * Vertices[2].z;
	float t = -(Vec3::Dot(normal, ray->origin) - d) / (Vec3::Dot(normal, ray->dir));

	if (t < 0) return false;

	Vec3 pos = ray->origin + t * ray->dir;

	Vec3 edge1 = Vertices[1] - Vertices[0];
	Vec3 pedge1 = pos - Vertices[0];
	if (Vec3::Dot(Vec3::Cross(edge1, pedge1), normal) < 0) return false;

	Vec3 edge2 = Vertices[2] - Vertices[1];
	Vec3 pedge2 = pos - Vertices[1];
	if (Vec3::Dot(Vec3::Cross(edge2, pedge2), normal) < 0) return false;

	Vec3 edge3 = Vertices[0] - Vertices[2];
	Vec3 pedge3 = pos - Vertices[2];
	if (Vec3::Dot(Vec3::Cross(edge3, pedge3), normal) < 0) return false;

	hitPos = pos;
	direction = (Vec3::Reflect(ray->dir, normal)).normalized();
	if (Vec3::Dot(direction, ray->dir) > 0) direction = -direction;
	return true;
	*/

  
  // compute triangle edges
  auto edge1 = Vertices[1] - Vertices[0];
  auto edge2 = Vertices[2] - Vertices[0];

  // compute determinant to solve a linear system
  auto pvec = Vec3::Cross(ray->dir, edge2);
  auto det  = Vec3::Dot(edge1, pvec);

  // check determinant and exit if triangle and ray are parallel
  // (could use EPSILONS if desired)
  if (det == 0) return false;
  auto inv_det = 1.0f / det;

  // compute and check first bricentric coordinated
  auto tvec = ray->origin - Vertices[0];
  auto u    = Vec3::Dot(tvec, pvec) * inv_det;
  if (u < 0 || u > 1) return false;

  // compute and check second bricentric coordinated
  auto qvec = Vec3::Cross(tvec, edge1);
  auto v    = Vec3::Dot(ray->dir, qvec) * inv_det;
  if (v < 0 || u + v > 1) return false;

  // compute and check ray parameter
  auto t = Vec3::Dot(edge2, qvec) * inv_det;
  if (t < 0.1) return false;

  // intersection occurred: set params and exit
  hitPos = ray->origin + t * ray->dir;
  direction = Vec3::Reflect(ray->dir, normal);
  return true;

}



Vec3 Triangle::make_normal(const Vec3 & v1, const Vec3 & v2, const Vec3 & v3) {
	Vec3 d1 = v2 - v1;
	Vec3 d2 = v3 - v1;

	std::cout << "Make Triangle " << std::endl;
	std::cout << "V1 " << v1.tostring() << std::endl;
	std::cout << "V2 " << v2.tostring() << std::endl;
	std::cout << "V3 " << v3.tostring() << std::endl;
	std::cout << "d1 " << d1.tostring() << std::endl;
	std::cout << "d2 " << d2.tostring() << std::endl;
	std::cout << "normal " << ((Vec3::Cross(d1, d2)).normalized()).tostring() << std::endl << std::endl;


	return (Vec3::Cross(d1, d2)).normalized();
}
