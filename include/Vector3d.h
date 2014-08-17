#pragma once

#include "Utils.h"
#include <math.h>

class Vector3d {
public:
	Vector3d() {x = y = z = 0;}
	
	Vector3d(Vector3d* p_vec) {x = p_vec->x; y = p_vec->y; z = p_vec->z; }

	Vector3d(float p_x, float p_y, float p_z) {
		x = p_x;
		y = p_y;
		z = p_z;
	}

	float x,y,z; //the values

	void setX(float p_x) {x = p_x;}
	void setY(float p_y) {y = p_y;}
	void setZ(float p_z) {z = p_z;}
	void setValues(float p_x, float p_y, float p_z) {
		x = p_x;
		y = p_y;
		z = p_z;
	}

	float length();
	float length2();

	float dot(const Vector3d p_vec);
	Vector3d cross(const Vector3d p_vec);

	Vector3d unitVector();
	void normalize();

	Vector3d operator-() const;

	friend Vector3d operator+(const Vector3d p_vec1, const Vector3d p_vec2);
	friend Vector3d operator-(const Vector3d p_vec1, const Vector3d p_vec2);
	friend Vector3d operator*(const Vector3d p_vec1, const Vector3d p_vec2);
	friend Vector3d operator/(const Vector3d p_vec1, const Vector3d p_vec2);

	friend Vector3d operator*(float p_f, const Vector3d p_vec1);
	friend Vector3d operator/(float p_f, const Vector3d p_vec1);
	friend Vector3d operator*(const Vector3d p_vec1, float p_f);
	friend Vector3d operator/(const Vector3d p_vec1, float p_f);

	friend bool operator==(const Vector3d p_vec1, const Vector3d p_vec2);
};

inline float Vector3d::length() {
	return sqrt(x * x + y * y + z * z);
}

inline float Vector3d::length2() {
	return x * x + y * y + z * z;
}

inline float Vector3d::dot(const Vector3d p_vec) {
	return x * p_vec.x + y * p_vec.y + z * p_vec.z;
}

inline Vector3d Vector3d::cross(const Vector3d p_vec) {
	Vector3d result;
	result.x = y * p_vec.z - z * p_vec.y;
	result.y = z * p_vec.x - x * p_vec.z;
	result.z = x * p_vec.y - y * p_vec.x;

	return result;
}

inline Vector3d Vector3d::unitVector() {
	float length = (*this).length();
	return *this / length;
}

inline void Vector3d::normalize() {
	float length = (*this).length();
	x /= length;
	y /= length;
	z /= length;
}

inline Vector3d Vector3d::operator-() const {
	return Vector3d(-x, -y, -z);
}

inline Vector3d operator+(const Vector3d p_vec1, const Vector3d p_vec2) {
	Vector3d vec(p_vec1.x + p_vec2.x, p_vec1.y + p_vec2.y, p_vec1.z + p_vec2.z);
	return vec;
}

inline Vector3d operator-(const Vector3d p_vec1, const Vector3d p_vec2) {
	Vector3d vec(p_vec1.x - p_vec2.x, p_vec1.y - p_vec2.y, p_vec1.z - p_vec2.z);
	return vec;
}

inline Vector3d operator*(const Vector3d p_vec1, const Vector3d p_vec2){
	Vector3d vec(p_vec1.x * p_vec2.x, p_vec1.y * p_vec2.y, p_vec1.z * p_vec2.z);
	return vec;
}

inline Vector3d operator/(const Vector3d p_vec1, const Vector3d p_vec2){
	Vector3d vec(p_vec1.x / p_vec2.x, p_vec1.y / p_vec2.y, p_vec1.z / p_vec2.z);
	return vec;
}

inline Vector3d operator*(float p_f, const Vector3d p_vec1){
	Vector3d vec(p_vec1.x * p_f, p_vec1.y * p_f, p_vec1.z * p_f);
	return vec;
}

inline Vector3d operator/(float p_f, const Vector3d p_vec1){
	Vector3d vec(p_vec1.x / p_f, p_vec1.y / p_f, p_vec1.z / p_f);
	return vec;
}

inline Vector3d operator*(const Vector3d p_vec1, float p_f){
	return p_f * p_vec1;
}

inline Vector3d operator/(const Vector3d p_vec1, float p_f){
	return p_f / p_vec1;
}

inline bool operator==(const Vector3d p_vec1, const Vector3d p_vec2){
	bool equalX = fabs(p_vec1.x - p_vec2.x) < EPSILON;
	bool equalY = fabs(p_vec1.y - p_vec2.y) < EPSILON;
	bool equalZ = fabs(p_vec1.z - p_vec2.z) < EPSILON;

	return equalX && equalY && equalZ;
}