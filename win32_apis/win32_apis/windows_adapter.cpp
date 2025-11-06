#include "windows_adapter.hpp"

#include <iostream>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNING
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define TARGET_DISPLAY 1 // 0 based system

// Define the static member
std::vector<WindowsAdapter::MonitorInfo> WindowsAdapter::m_monitors;

WindowsAdapter::WindowsAdapter() : m_hdc(nullptr), m_hWnd(nullptr), m_color_buffer(nullptr) {}
WindowsAdapter::~WindowsAdapter()
{
	delete m_application;
}

#pragma comment( lib, "winmm.lib")   
#include <mmsystem.h>
WindowsAdapter& WindowsAdapter::GetInstance()
{
	static WindowsAdapter instance;
	return instance;
}

unsigned long WindowsAdapter::getTime()
{
	return timeGetTime();
}

// Callback function for EnumDisplayMonitors
BOOL CALLBACK WindowsAdapter::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	WindowsAdapter::MonitorInfo m_info;
	m_info.hMonitor = hMonitor;
	m_info.rect = *lprcMonitor;
	m_info.index = static_cast<int>(m_monitors.size());
	m_monitors.push_back(m_info);
	return TRUE; // Continue enumeration
}

// Function to fetch monitor count and info
int WindowsAdapter::GetMonitorCount()
{
	m_monitors.clear();
	EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
	return static_cast<int>(m_monitors.size());
}

// Function to et monitor rectangle by index (0-based)
RECT WindowsAdapter::GetMonitorRect(int monitorIndex)
{
	if (monitorIndex >= 0 && monitorIndex < static_cast<int>(m_monitors.size()))
	{
		return m_monitors[monitorIndex].rect;
	}

	// Return primary monitor rect if index is invalid
	RECT primary_rect = {
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN)
	};
	return primary_rect;
}

#pragma comment(lib, "winmm.lib")   
#pragma comment(lib, "Msimg32.lib")  // Add this line for AlphaBlend
#include <mmsystem.h>

// Initialize pixel buffer
void WindowsAdapter::InitializePixelBuffer(HDC hdc)
{
	// Set buffer dimensions
	m_buffer_width = static_cast<int>(m_monitor_width);
	m_buffer_height = static_cast<int>(m_monitor_height);

	// Create memory device context
	m_memory_dc = CreateCompatibleDC(hdc);

	if (!m_memory_dc)
	{
		std::cerr << "Failed loading device context memory. Error: " << GetLastError() << "\n";
		return;
	}

	// Create bitmap info
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof BITMAPINFOHEADER;
	bmi.bmiHeader.biWidth = m_buffer_width;
	bmi.bmiHeader.biHeight = -m_buffer_height; // Negative from top-bottom bitmap
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Create DIB section
	m_bitmap = CreateDIBSection(m_memory_dc, &bmi, DIB_RGB_COLORS,
		reinterpret_cast<void**>(&m_color_buffer), nullptr, 0);

	if (!m_bitmap)
	{
		std::cerr << "Failed to create DIB section. Error: " << GetLastError() << "\n";
		DeleteDC(hdc);
		m_memory_dc = nullptr;
		return;
	}

	if (!m_color_buffer)
	{
		std::cerr << "CreateDIBSection succeeded but color buffer is null!\n";
		DeleteObject(m_bitmap);
		DeleteDC(m_memory_dc);
		m_bitmap = nullptr;
		m_memory_dc = nullptr;
		return;
	}

	// Select the bitmap into the memory DC
	HGDIOBJ old_obj = SelectObject(m_memory_dc, m_bitmap);
	if (!old_obj || old_obj == HGDI_ERROR)
	{
		std::cerr << "Failed to select bitmap into DC. Error: " << GetLastError() << "\n";
		DeleteObject(m_bitmap);
		DeleteDC(hdc);
		m_bitmap = nullptr;
		m_memory_dc = nullptr;
		m_color_buffer = nullptr;
		return;
	}

	// Clear buffer initially
	size_t buffer_size = static_cast<size_t>(m_buffer_width * m_buffer_height * sizeof(uint32_t));
	memset(m_color_buffer, 0, buffer_size);
}

void WindowsAdapter::PresentPixelBuffer(HDC hdc)
{
	if (!m_memory_dc || !m_bitmap || !m_color_buffer)
	{
		std::cerr << "Cannot present pixel buffer, Initialization failed\n";
		return;
	}

	// Blit from memory DC (your pixel buffer) to window DChat
	BOOL result = BitBlt(hdc, 0, 0, m_buffer_width, m_buffer_height, m_memory_dc, 0, 0, SRCCOPY);

	if (!result)
	{
		std::cerr << "BitBlt failed. Error: " << GetLastError() << "\n";
	}
}

// Cleanup pixel buffer resources
void WindowsAdapter::CleanupPixelBuffer()
{
	if (m_bitmap)
	{
		DeleteObject(m_bitmap);
		m_bitmap = nullptr;
	} if (m_memory_dc)
	{
		DeleteDC(m_memory_dc);
		m_memory_dc = nullptr;
	}
	m_color_buffer = nullptr;
}

void WindowsAdapter::finish(Application& app)
{
	m_application = &app;

	if (!m_hWnd)
		return;

	m_application->ShutDown();

	CleanupPixelBuffer();
	ReleaseDC(m_hWnd, m_hdc);
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;
	m_hdc = nullptr;
}

void WindowsAdapter::StartWindowed(int x, int y, unsigned int w, unsigned int h, int antialiasing, Application& app, int nCmdShow)
{
	m_application = &app;

	const wchar_t* CLASS_NAME = L"Sample OpenGL Window";

	WNDCLASS wn = {};
	wn.hInstance = m_instance;
	wn.lpfnWndProc = WindowsAdapter::WinProc;
	wn.lpszClassName = CLASS_NAME;
	wn.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (!RegisterClass(&wn))
	{
		std::cerr << "Failed to register window class. Error code: " << GetLastError() << "\n";
		return;
	}

	// -----------------// Window Detection //----------------------
	m_monitor_count = GetMonitorCount();
	std::wcout << L"Number of monitors detected: " << m_monitor_count << "\n";

	// Print information about each monitor
	for (int i = 0; i < m_monitor_count; i++)
	{
		RECT monitor_rect = GetMonitorRect(i);
		std::wcout << L"Monitor " << (i + 1) << L": "
			<< L"Left= " << monitor_rect.left << L", "
			<< L"Top= " << monitor_rect.top << L", "
			<< L"Right= " << monitor_rect.right << L", "
			<< L"Bottom= " << monitor_rect.bottom << L", "
			<< L"Width= " << (monitor_rect.right - monitor_rect.left) << L", "
			<< L"Height= " << (monitor_rect.bottom - monitor_rect.top) << L"\n";
	}

	// -----------------// Window //----------------------
	RECT target_monitor_rect;

	if (TARGET_DISPLAY < m_monitor_count)
	{
		target_monitor_rect = GetMonitorRect(TARGET_DISPLAY);
		std::wcout << L"Placing window on monitor " << (TARGET_DISPLAY + 1) << "\n";
	}
	else
	{
		target_monitor_rect = GetMonitorRect(0);
		std::wcout << L"Third monitor not available. Placing window on primary monitor.\n";
	}

	// Calculate monitor dimensions
	m_monitor_width = target_monitor_rect.right - target_monitor_rect.left;
	m_monitor_height = target_monitor_rect.bottom - target_monitor_rect.top;

	// Create window rectangle (this represents the CLIENT area)
	RECT windowRect;
	SetRect(&windowRect,
		target_monitor_rect.left,
		target_monitor_rect.top,
		target_monitor_rect.right,    // Fixed: add width to x position
		target_monitor_rect.bottom    // Fixed: add height to y position
	);

	std::wcout << L"Window client rect before adjust: Left=" << windowRect.left
		<< L", Top=" << windowRect.top
		<< L", Right=" << windowRect.right
		<< L", Bottom=" << windowRect.bottom << L"\n";


	m_hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"OpenGL Window",
		WS_POPUP | WS_VISIBLE,
		windowRect.left,
		windowRect.top,
		m_monitor_width,
		m_monitor_height,

		nullptr,
		nullptr,
		m_instance,
		nullptr
	);

	if (m_hWnd == nullptr)
	{
		std::cout << "Failed Loading Window\n";
		return;
	}

	// Set the user data
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
	if (!SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this))
	{
		std::cerr << "Failed to set window user data\n";
		return;
	}

	m_hdc = GetDC(m_hWnd);

	std::cout << "-----------------// Renderer //----------------------\n";

	// Initialize pixel buffer with window client area size
	InitializePixelBuffer(m_hdc);

	if (m_color_buffer)
	{
		// Setup renderer with our pixel buffer
		m_application->SetupRenderer(m_color_buffer, m_buffer_width, m_buffer_height);
	}
	else
	{
		std::cerr << "Cannot setup renderer - pixel buffer initialization failed!\n";
		return;
	}

	//  ---------------------------------------
	// Show window
	if (m_application)
	{
		m_application->Initialize();

		ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);

	}
	else
	{
		std::cout << "Failed to Initialize the Application";
		return;
	}

	//  ---------------------------------------
	// Main loop
	MSG msg = {};

	while (m_hWnd) {

		if (PeekMessage(&msg,
			nullptr,
			0,
			0,
			PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			unsigned long time = getTime();
			unsigned int dt = static_cast<unsigned>(time - m_time);
			m_time = time;
			float aspect = m_buffer_width / (float)m_buffer_height;

			m_application->Update(dt);
			m_application->Render(aspect);

			if (m_hdc)
				PresentPixelBuffer(m_hdc);

		}
	}


	finish(app);
}

LRESULT CALLBACK WindowsAdapter::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowsAdapter* adapter = reinterpret_cast<WindowsAdapter*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (!adapter)
		return DefWindowProc(
			hWnd, 
			message, 
			wParam, 
			lParam
		);


	if (hWnd == adapter->m_hWnd)
	{
		switch (message)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			// Present to screen
			adapter->PresentPixelBuffer(hdc);

			EndPaint(hWnd, &ps);
			return 0;
		}

		case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case 'W':
				std::wcout << L"Move Forward\n";
				break;
			case 'S':
				std::wcout << L"Move Backward\n";
				break;
			case 'A':
				std::wcout << L"Move Left\n";
				break;
			case 'D':
				std::wcout << L"Move Right\n";
				break;
			case VK_SPACE:
				std::wcout << L"Move Up\n";
				break;
			case VK_CONTROL:
				std::wcout << L"Move Down\n";
				break;
			}

			// Trigger repaint to show changes
			InvalidateRect(hWnd, nullptr, FALSE);
			break;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(
		hWnd,
		message,
		wParam,
		lParam);
}
#endif