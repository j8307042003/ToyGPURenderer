#ifndef Matrix4_H
#define Matrix4_H

#include "Vec4.h"

class Matrix4 {
public:
	Vec4 x = {0, 0, 0, 0};
	Vec4 y = {0, 0, 0, 0};
	Vec4 z = {0, 0, 0, 0};
	Vec4 w = {0, 0, 0, 0};

	Matrix4() : x({1, 0, 0, 0}), y({0, 1, 0 , 0}), z({0, 0, 1, 0}), w({0, 0, 0, 1}) {}
	Matrix4(const Vec4 & x, const Vec4 & y, const Vec4 & z, const Vec4 & w) : x(x), y(y), z(z), w(w) {}

	Vec4&       operator[](int i) { return (&x)[i]; }
	const Vec4& operator[](int i) const { return (&x)[i]; }
};



inline Vec4 operator*(const Matrix4 & lhs, const Vec4 & v ) {
	return lhs.x * v.x + lhs.y * v.y + lhs.z * v.z + lhs.w * v.w;
}

inline Matrix4 operator*(const Matrix4& a, const Matrix4& b) {
  return {a * b.x, a * b.y, a * b.z, a * b.w};
}

#endif