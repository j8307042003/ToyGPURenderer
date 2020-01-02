#pragma once
#ifndef Quaternion_H
#define Quaternion_H

#include "Vec3.h"
#include "Matrix4.h"


class Quaternion {
public:
	float x;
	float y;
	float z;
	float w;

	Quaternion() : x(0), y(0), z(0), w(1) {}

	Quaternion( Vec3 axis, float angle ) {
	// assume axis vector is normalized
		float s = sin(0.5f * angle);

		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
		w = 0.5f * angle;
	}

	Quaternion( float x, float y, float z, float w ) : x(x), y(y), z(z), w(w) {}

	Matrix4 GetMatrix();


	Quaternion operator*(const Quaternion& rhs) {
  		Quaternion q;
   
		const Vec3 vThis(x, y, z);
		const Vec3 rhsV3(rhs.x, rhs.y, rhs.z);
		q.w = w * rhs.w - Vec3::Dot(vThis, rhsV3);
		 
		const Vec3 newV = 
		    rhs.w * vThis + w * rhsV3 + Vec3::Cross(vThis, rhsV3);
		q.x = newV.x;
		q.y = newV.y;
		q.z = newV.z;
		 
		return q;
	}
};

#endif