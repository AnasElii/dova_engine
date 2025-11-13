#include <iostream>

#include "application.hpp"
#include "renderer.hpp"
#include "projection.hpp"
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
					m_cube_points[number_count++] = { x, y, z };
				}

			}
		}

	}

	void Update(int inDeltaTime) override
	{
		for (int i = 0; i < P_NUMBER; i++)
		{
			vec3_t point = m_cube_points[i];

			// Move point relative to camera
			point.z -= camera_position.z;

			// Project the current point
			vec2_t projected_point = m_projection->PerspectiveProject(point);

			m_projection->GetProjectedPoints()[i] = projected_point;
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
			const vec2_t point = m_projection->GetProjectedPoints()[i];

			// Check the Z axis of the point the far the point the color gets darker and get smaller
			
			renderer->DrawRectangle(
				static_cast<uint16_t>(point.x) + (renderer->GetBufferWidth() / 2),
				static_cast<uint16_t>(point.y) + (renderer->GetBufferHeight() / 2),
				4,
				4,
				0xFF57A649
			);
		
		}

	}

	void ShutDown() override
	{
		std::cout << "RuleEngine shutting down\n";
	}

private:
	vec3_t m_cube_points[P_NUMBER];
	Projection *m_projection = new Projection(static_cast<size_t>(P_NUMBER));

	vec3_t camera_position = {0, 0, -5};

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