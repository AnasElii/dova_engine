#include "projection.hpp"

Projection::Projection(): m_fov_factor(1500.0f) {};

Projection::Projection(float fov) : m_fov_factor(fov) {};

Projection::Projection(size_t init_size) : m_fov_factor(1500.0f)
{
	m_projected_points.resize(init_size);
};

Projection::Projection(float fov, size_t init_size) : m_fov_factor(fov)
{
	m_projected_points.resize(init_size);
};


vec2_t Projection::OrthographicProject(vec3_t point)
{
	vec2_t projected_point = {
		point.x * m_fov_factor,
		point.y * m_fov_factor
	};

	return projected_point;
}

vec2_t Projection::PerspectiveProject(vec3_t point)
{
	// Clamp z to near plane (prevents division by zero, infinity, and smaller values)
	float z = point.z <= NEAR_PLANE ? point.z = NEAR_PLANE : point.z;

	vec2_t projected_point = {
		(point.x * m_fov_factor) / z,
		(point.y * m_fov_factor) / z
	};

	return projected_point;
}

void Projection::SetProjectedPoints(int idx, vec2_t& point)
{
	if (idx < m_projected_points.size())
		m_projected_points[idx] = point;
}

void Projection::ResizeProjectedPoints(size_t new_size)
{
	m_projected_points.resize(new_size);
}


vec2_t Projection::ToScreenSpace(const vec2_t& projected_point, int screen_width, int screen_height)
{
	return {
		projected_point.x + (screen_width / 2.0f),
		projected_point.y + (screen_height / 2.0f)
	};
}

void Projection::ProjectAllPoints(const vec3_t* world_points, int count, const vec3_t& camera_position)
{
	for (int i = 0; i < count && i < m_projected_points.size(); i++)
	{
		vec3_t point = world_points[i];
		point.z -= camera_position.z;
		m_projected_points[i] = PerspectiveProject(point);
	}
}



