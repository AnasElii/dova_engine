#pragma once
#include <stdint.h>

class Renderer
{
public:
	Renderer();
	~Renderer();

public:
	void Initialize(uint32_t* color_buffer, int width, int height);
	void Shutdown();

private:
	struct monitor
	{
		int buffer_width;
		int buffer_height;
	};

public:
	int GetBufferWidth() const { return m_monitor.buffer_width; }
	int GetBufferHeight() const { return m_monitor.buffer_height; }

public:
	void ClearColorBuffer(uint32_t color);
	void DrawPixel(int x, int y, uint32_t color);
	void DrawGrid(uint32_t color, int spacing = 10);
	void DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

private:

	uint32_t* m_color_buffer;
	monitor m_monitor;
};
