#include <iostream>

#include "application.hpp"
#include "renderer.hpp"
#include "vector.h"
#include "windows_adapter.hpp"

#define P_NUMBER (9 * 9 * 9)
#define NEAR_PLANE 0.1f

class RuleEngine : public Application
{
	void Initialize() override
	{
		std::cout << "RuleEngine initialized!\n";

		// Load my array of vectors
		// From -1 to 1 (in this 9 * 9 * 9 array)
		int number_count = 0;

		for (float x = -1; x <= 1; x += 0.25)
		{
			for (float y = -1; y <= 1; y += 0.25)
			{
				for (float z = -1; z <= 1; z += 0.25)
				{
					m_cube_points[number_count++] = { x, y, z };
				}

			}
		}

	}

	vec2_t orthographic_project(vec3_t point)
	{
		vec2_t projected_point = {
			(point.x * m_fov_factor),
			(point.y * m_fov_factor)
		};

		return projected_point;
	}

	vec2_t perspective_project(vec3_t point)
	{
		// Clamp z to near plane (prevents division by zero, infinity, and smaller values)
		float z = point.z <= NEAR_PLANE ? point.z = NEAR_PLANE : point.z;

		vec2_t projected_point = {
			(point.x * m_fov_factor) / z,
			(point.y * m_fov_factor) / z
		};

		return projected_point;
	}

	void Update(int inDeltaTime) override
	{
		for (int i = 0; i < P_NUMBER; i++)
		{
			vec3_t point = m_cube_points[i];

			// Change color base on Z distance
			const uint32_t main_color = 0xFF9090FF;

			if (point.z == -1)
				m_color_point[i] = main_color;
			else
			{
				uint8_t r_color = (main_color >> 16) & 0xFF;
				uint8_t g_color = (main_color >> 8) & 0xFF;
				uint8_t b_color = main_color & 0xFF;

				// Calculate fade factor (0.0 = no fade, 1.0 = full fade to black)
				float fade_factory = point.z; // point.z ranges from 1 to -1

				uint8_t final_r_color = static_cast<uint8_t>(r_color * (1.0f - fade_factory));
				uint8_t final_g_color = static_cast<uint8_t>(g_color * (1.0f - fade_factory));
				uint8_t final_b_color = static_cast<uint8_t>(b_color * (1.0f - fade_factory));

				m_color_point[i] = 0xFF000000 | (final_r_color << 16) | (final_g_color << 8) | final_b_color;

				// Move point relative to camera
				point.z -= camera_position.z;

				// Project the current point
				vec2_t projected_point = perspective_project(point);

				m_projected_points[i] = projected_point;
			}
		}
	}

	void Render(float inAspectRatio) override
	{
		Renderer* renderer = GetRenderer();
		if (!renderer)
		{
			std::cerr << "No renderer available in RuleEngine!\n";
			return;
		}

		renderer->ClearColorBuffer(0xFF020202);
		renderer->DrawGrid(0xFF333333);

		for (int i = 0; i < P_NUMBER; i++)
		{
			const vec2_t point = m_projected_points[i];

			// Check the Z axis of the point the far the point the color gets darker and get smaller
			
			renderer->DrawRectangle(
				static_cast<uint16_t>(point.x) + (renderer->GetBufferWidth() / 2),
				static_cast<uint16_t>(point.y) + (renderer->GetBufferHeight() / 2),
				4,
				4,
				m_color_point[i]
			);
		
		}

	}

	void ShutDown() override
	{
		std::cout << "RuleEngine shutting down\n";
	}

private:
	vec3_t m_cube_points[P_NUMBER];
	vec2_t m_projected_points[P_NUMBER];

	vec3_t camera_position = {0, 0, -5};

	const float m_fov_factor = 1500;
	uint32_t m_color_point[P_NUMBER];

};

#if _DEBUG
#pragma comment(linker, "/subsystem:console")

#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
#endif
int main()
{
	RuleEngine engine;
	engine.StartWindowed(0, 0, 100, 100, 0);
	return 0;
}
#else
#pragma comment(linker, "/subsystem:windows");
#endif