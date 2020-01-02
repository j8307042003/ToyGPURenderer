#pragma once
#ifndef Vec3_H
#define Vec3_H
#include <string>
#include <math.h>
#include <iostream>     // std::cout, std::ios
#include <sstream>

class Vec3 {

public:
	float x;
	float y;
	float z;

	Vec3(){}
	Vec3(float x, float y, float z);


	float length() const {
		return sqrt(x * x + y * y + z * z);
	}


	void normalize(){
		float len = length();
		x /= len;
		y /= len;
		z /= len;
	}

	Vec3 normalized() const {
		float len = length();
		return Vec3(x/len, y/len, z/len);
	}	

	std::string tostring() const {
  		std::ostringstream stringStream;
  		stringStream << x << " " << y << " " << z;
  		return stringStream.str();		
	}

	static float Angle(Vec3 a, Vec3 b);

	static float Dot(Vec3 a, Vec3 b);
	
	static Vec3 Cross(Vec3 a, Vec3 b);

	static Vec3 Reflect(Vec3 a, Vec3 b);


	Vec3 operator+(const Vec3& rhs);
	friend Vec3 operator+(const Vec3 & lhs,const Vec3& rhs)
  	{
  		return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
  	}
	Vec3 operator+(const float& f);
	Vec3 operator-(const Vec3& rhs);
  	friend Vec3 operator-(const Vec3& lhs,const Vec3& rhs)
  	{
  		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
  	}

	Vec3 operator-(const float& f);
	Vec3 operator-();
	Vec3 operator*(const Vec3& rhs);
	Vec3 operator*(const float& f);
  	friend Vec3 operator*(float lhs,const Vec3& rhs)
  	{
  		return Vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
  	}


	Vec3 operator/(const Vec3& rhs);
	Vec3 operator/(const float& f);

	Vec3& operator+=(const Vec3& rhs);
	Vec3& operator-=(const Vec3& rhs);
};

#endif
