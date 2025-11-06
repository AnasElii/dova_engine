#include <iostream>

#include "renderer.hpp"

Renderer::Renderer() : m_color_buffer(nullptr), m_monitor{ 0,0 } {}
Renderer::~Renderer()
{
	// Note: We don't delete m_color_buffer since it's owned by the platform adapter
}

void Renderer::Initialize(uint32_t* color_buffer, int width, int height)
{
	if (!color_buffer) return;
	if (width <= 0 || height <= 0) return;

	m_color_buffer = color_buffer;
	m_monitor.buffer_width = width;
	m_monitor.buffer_height = height;

}

void Renderer::Shutdown()
{
	m_color_buffer = nullptr;
	m_monitor.buffer_width = 0;
	m_monitor.buffer_height = 0;
}


void Renderer::ClearColorBuffer(uint32_t color)
{

	if (!m_color_buffer) return;
	if (m_monitor.buffer_width <= 0 || m_monitor.buffer_height <= 0) return;

	for (int i = 0; i < m_monitor.buffer_width * m_monitor.buffer_height; ++i)
	{
		m_color_buffer[i] = color;
	}

}

void Renderer::DrawPixel(int x, int y, uint32_t color)
{
	if (x < 0 || x >= m_monitor.buffer_width || y < 0 || y >= m_monitor.buffer_height)
		return;

	uint32_t* pixel = &m_color_buffer[y * m_monitor.buffer_width + x];

	// Extract ARGB components
	// 0xAARRGGBB >> 24 = 0x000000AA
	uint8_t src_a = (color >> 24) & 0xFF;
	// 0xAARRGGBB >> 16 = 0x0000AARR
	uint8_t src_r = (color >> 16) & 0xFF;
	// 0xAARRGGBB >> 8 = 0x00AARRGG
	uint8_t src_g = (color >> 8) & 0xFF;
	// 0xAARRGGBB >> 0xAARRGGBB
	uint8_t src_b = color & 0xFF;

	if (src_a == 255) {
		// Fully opaque - just replace
		*pixel = color;
	}
	else if (src_a > 0) {
		// Alpha blend manually
		uint32_t dst = *pixel;
		uint8_t dst_r = (dst >> 16) & 0xFF;
		uint8_t dst_g = (dst >> 8) & 0xFF;
		uint8_t dst_b = dst & 0xFF;

		// Normal / Over blending
		uint8_t inv_alpha = 255 - src_a;
		uint8_t final_r = (src_r * src_a + dst_r * inv_alpha) / 255; // <-- Divided by 255 to be normalized
		uint8_t final_g = (src_g * src_a + dst_g * inv_alpha) / 255;
		uint8_t final_b = (src_b * src_a + dst_b * inv_alpha) / 255;

		*pixel = 0xFF000000 | (final_r << 16) | (final_g << 8) | final_b;
	}
	// If src_a == 0, don't draw anything
}

void Renderer::DrawGrid(uint32_t color, int spacing)
{
	if (!m_color_buffer)
		return;

	if (m_monitor.buffer_width <= 0 || m_monitor.buffer_height <= 0)
		return;

	// Vertical lines
	for (int dy = 0; dy <= m_monitor.buffer_width; dy += spacing)
	{
		for (int dx = 0; dx <= m_monitor.buffer_width; dx += spacing)
		{
			DrawPixel(dx, dy, color);
		}
	}
}

void Renderer::DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	if (!m_color_buffer)
		return;

	for (int dy = 0; dy <= height; dy++)
	{
		for (int dx = 0; dx <= width; dx++)
		{
			int new_x = x + dx;
			int new_y = y + dy;

			DrawPixel(new_x, new_y, color);
		}
	}
}