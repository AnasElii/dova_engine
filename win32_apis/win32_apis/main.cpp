#include <iostream>

#include "application.hpp"
#include "renderer.hpp"
#include "vector.h"

#define P_NUMBER (9 * 9 * 9)


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
					std::cout << "x: " << x << " y: " << y << " z: " << z << "\n";
					m_cube_points[number_count++] = { x, y, z };
				}

			}
		}

	}

	vec2_t project(vec3_t point)
	{
		vec2_t projected_point = {
			(point.x * m_fov_factor),
			(point.y * m_fov_factor)
		};

		return projected_point;
	}

	void Update(int inDeltaTime) override
	{
		for (int i = 0; i < P_NUMBER; i++)
		{
			const vec3_t point = m_cube_points[i];

			// Project the current point
			vec2_t projected_point = project(point);

			m_projected_points[i] = projected_point;
		}
	}

	// Render Happen Before Update ERROR Should be fixed
	void Render(float inAspectRatio) override
	{
		// Use the Application's renderer, not our own
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
			renderer->DrawRectangle(
				static_cast<uint16_t>(point.x) + (renderer->GetBufferWidth() / 2), 
				static_cast<uint16_t>(point.y) + (renderer->GetBufferHeight() / 2), 
				4, 
				4, 
				0xFF579e42
			);
		}

		//renderer->DrawRectangle(10, 10, 300, 150, 0xFF579e42);
	}

	void ShutDown() override
	{
		std::cout << "RuleEngine shutting down\n";
	}

private:
	vec3_t m_cube_points[P_NUMBER];
	vec2_t m_projected_points[P_NUMBER];
	const float m_fov_factor = 128;

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
