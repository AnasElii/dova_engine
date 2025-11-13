#pragma once
#include "vector.h"
#include <vector>

#define NEAR_PLANE 0.1f

class Projection
{
public:
	Projection();
	explicit Projection(float fov);
	explicit Projection(size_t init_size);
	Projection(float fov, size_t init_size);

	vec2_t OrthographicProject(vec3_t point);
	vec2_t PerspectiveProject(vec3_t point);

public:
	std::vector<vec2_t>& GetProjectedPoints() { return m_projected_points; }
	const std::vector<vec2_t>& GetProjectedPoints() const { return m_projected_points; }

	void SetProjectedPoints(int idx, vec2_t& point);
	void ResizeProjectedPoints(size_t new_size);
	size_t GetPointCount() const { return m_projected_points.size(); }

	float GetFOVFactors() const { return m_fov_factor; }
	void SetFOVFactors(float fov) { m_fov_factor = fov; }

	vec2_t ToScreenSpace(const vec2_t& projected_point, int screen_width, int screen_height);
	void ProjectAllPoints(const vec3_t* world_points, int count, const vec3_t& camera_position);

private:
	std::vector<vec2_t> m_projected_points;
	float m_fov_factor;
};
