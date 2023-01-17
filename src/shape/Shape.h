#pragma once

#include "math/Vec3.h"
#include "math/Ray.h"
#include "math/Transform.h"
#include <glm/matrix.hpp>
#include <vector>
#include <glm/vec3.hpp>

enum class ShapeType
{
	Sphere,
	Triangle,
	Plane, // TODO
};

class Shape {
public:
	glm::mat4 m_transform;
	Shape(){}

	virtual bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction){return false;}
	virtual ShapeType Type() = 0;
};




struct ShapeData
{
	ShapeType type;
	int primitiveId;
	int matIdx;
};


struct ShapesData
{
	std::vector<glm::ivec3> triangles; // Map To Vertex Data
	std::vector<glm::ivec2> spheres; // x for position index, y for radius


	// Vertex Data
	std::vector<glm::dvec3> positions;
	std::vector<glm::dvec3> normals;
	std::vector<glm::dvec3> tangents;
	std::vector<glm::dvec2> texcoords;

	// Radius data. for Sphere
	std::vector<float> radius;
};

inline int AddShapesDataTriangle(ShapesData & shapesData, 
	const glm::dvec3 & v0, const glm::dvec3 & v1, const glm::dvec3 & v2, 
	const glm::dvec3 & normal0, const glm::dvec3 & normal1, const glm::dvec3 & normal2,
	const glm::dvec3 & tangent0, const glm::dvec3 & tangent1, const glm::dvec3 & tangent2,
	const glm::dvec2 & uv0 = {}, const glm::dvec2 & uv1 = {}, const glm::dvec2 & uv2 = {}
)
{
	shapesData.positions.push_back(v0);
	shapesData.positions.push_back(v1);
	shapesData.positions.push_back(v2);

	shapesData.normals.push_back(normal0); // TODO
	shapesData.normals.push_back(normal1); // TODO
	shapesData.normals.push_back(normal2); // TODO

	shapesData.tangents.push_back(tangent0);
	shapesData.tangents.push_back(tangent1);
	shapesData.tangents.push_back(tangent2);

	shapesData.texcoords.push_back(uv0); // TODO
	shapesData.texcoords.push_back(uv1); // TODO
	shapesData.texcoords.push_back(uv2); // TODO


	int vertexOffset = (int)shapesData.positions.size() - 1;
	shapesData.triangles.push_back(glm::ivec3(vertexOffset - 2, vertexOffset - 1, vertexOffset));
	return (int)shapesData.triangles.size() - 1;
}

inline int AddShapesDataSphere(ShapesData& shapesData, const glm::dvec3 position, float radius)
{
	shapesData.positions.push_back(position);
	shapesData.normals.push_back(glm::dvec3()); //
	shapesData.tangents.push_back(glm::dvec3());
	shapesData.texcoords.push_back(glm::dvec3()); //

	shapesData.radius.push_back(radius);

	int vertexOffset = (int)shapesData.positions.size() - 1;
	int radiusOffset = (int)shapesData.radius.size() - 1;

	shapesData.spheres.push_back({ vertexOffset, radiusOffset });

	return (int)shapesData.spheres.size() - 1;
}
