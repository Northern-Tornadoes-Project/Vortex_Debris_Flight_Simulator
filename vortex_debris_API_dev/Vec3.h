#pragma once

//simple header only 3d vector class
class Vec3{
public:
	double x, y, z;

	Vec3() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	Vec3(const double x, const double y, const double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3 const& operator+(const Vec3& v) {
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3 const& operator+=(const Vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vec3 const& operator*(const Vec3& v) {
		return Vec3(x * v.x, y * v.y, z * v.z);
	}

	Vec3 const& operator*(const double m) {
		return Vec3(x * m, y * m, z * m);
	}

	Vec3 const& operator*=(const Vec3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	double const& operator[](const int i) const
	{
		switch (i) {
			case 0:
				return x;
				break;
			case 1:
				return y;
				break;
			case 2:
				return z;
				break;
		}
	}
};

