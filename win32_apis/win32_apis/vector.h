#pragma once

struct vec2_t
{
	float x;
	float y;

	vec2_t() : x(0), y(0){}

	vec2_t(float x, float y) { this->x = x; this->y = y; }
	explicit vec2_t(const float* v) { x = v[0], y = v[1]; }

	vec2_t& set(float x, float y)
	{
		this->x = x;
		this->y = y;

		return *this;
	}
};

struct vec3_t
{
	float x;
	float y;
	float z;

	vec3_t() : x(0), y(0), z(0){}

	vec3_t(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	explicit vec3_t(const float* v) { x = v[0]; y = v[1]; z = v[2]; }

	vec3_t& set(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;

		return *this;
	}
};