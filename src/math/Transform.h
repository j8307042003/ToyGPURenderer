#ifndef Transform_H
#define Transform_H


#include "Vec3.h"
#include "Quaternion.h"
#include "glm/gtc/matrix_transform.hpp"

class Transform {
public:
	Vec3 position;
	Quaternion rotation;
	Transform * parent;

	// Matrix4 modelMatrix;
	glm::mat4 modelMatrix;

	Transform(){ UpdateMatrix(); }
	Transform(Vec3 position, Quaternion rotation ) : position(position), rotation(rotation) { UpdateMatrix(); }
	Vec3 GetWorldPosition();
	Vec3 TransformPoint(Vec3 point);
	Vec3 TransformDir(Vec3 dir);

	void UpdateMatrix();

private:

};


#endif