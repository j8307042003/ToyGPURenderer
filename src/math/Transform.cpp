#include "Transform.h"
#include "Matrix4.h"
#include <iostream>

Vec3 Transform::TransformPoint(Vec3 point) {
	Vec4 t = modelMatrix * Vec4(point, 1);
	t = t / t.w;
	return {t.x, t.y, t.z};
}

Vec3 Transform::TransformDir(Vec3 dir) {
	Matrix4 rotMat = rotation.GetMatrix();
	Vec4 t = rotMat * Vec4(dir, 1);
	// std::cout << t.x << " " << t.y << " " << t.z << " " << t.w << std::endl;
	// t = t / t.w;
	Vec3 v3dir = {t.x, t.y, t.z};
	return v3dir.normalized();
}



void Transform::UpdateMatrix() {
	Matrix4 rotMat = rotation.GetMatrix();
	Matrix4 translateMat 
		= Matrix4( 
		{position.x, 0, 0, 0},
		{0, position.y, 0, 0},
		{0, 0, position.z, 0},
		{0, 0, 0, 1}
		 );

	modelMatrix = translateMat * rotMat;
}
