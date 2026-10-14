#include "pch.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <mmsystem.h>
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <X11/Xatom.h>
#endif
#include <glad/glad.h>
#include <stb_image.h>

#include "Log.h"

#include "App.h"

namespace Editor {
#define BAR_HEIGHT 40

	int dragging = 0;
	double dragStartX, dragStartY;
	int winStartX, winStartY;
	static uint8_t GLFWWindowCount = 0;

#ifdef _WIN32
	static WNDPROC glfwWindowProc = nullptr;
	constexpr UINT_PTR NativeMoveTimerId = 1;
	static void renderNativeMoveFrame();

	static LRESULT CALLBACK borderlessWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_TIMER && wParam == NativeMoveTimerId)
		{
			renderNativeMoveFrame();
			return 0;
		}

		if (message == WM_NCCALCSIZE)
		{
			if (wParam && (GetWindowLongPtrW(window, GWL_STYLE) & WS_MAXIMIZE))
			{
				auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
				MONITORINFO monitorInfo = { sizeof(monitorInfo) };
				const HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
				if (GetMonitorInfoW(monitor, &monitorInfo))
					params->rgrc[0] = monitorInfo.rcWork;
			}
			return 0;
		}

		if (message == WM_NCPAINT)
			return 0;

		if (message == WM_NCACTIVATE)
			return TRUE;

		return CallWindowProcW(glfwWindowProc, window, message, wParam, lParam);
	}

	static bool enableNativeBorderlessFrame(GLFWwindow* glfwWindow)
	{
		HWND window = glfwGetWin32Window(glfwWindow);
		const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
		const LONG_PTR nativeStyle = (style & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;

		SetLastError(ERROR_SUCCESS);
		const LONG_PTR previousProc = SetWindowLongPtrW(
			window,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(borderlessWindowProc));
		if (!previousProc && GetLastError() != ERROR_SUCCESS)
		{
			CORE_ERROR("Failed to enable native borderless window handling: {}", GetLastError());
			return false;
		}

		glfwWindowProc = reinterpret_cast<WNDPROC>(previousProc);

		SetLastError(ERROR_SUCCESS);
		SetWindowLongPtrW(window, GWL_STYLE, nativeStyle);
		if (GetLastError() != ERROR_SUCCESS)
		{
			CORE_ERROR("Failed to apply native window styles: {}", GetLastError());
			SetWindowLongPtrW(window, GWLP_WNDPROC, previousProc);
			glfwWindowProc = nullptr;
			return false;
		}

		if (!SetWindowPos(window, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED))
		{
			CORE_ERROR("Failed to refresh native window styles: {}", GetLastError());
			SetWindowLongPtrW(window, GWL_STYLE, style);
			SetWindowLongPtrW(window, GWLP_WNDPROC, previousProc);
			glfwWindowProc = nullptr;
			return false;
		}

		return true;
	}
#endif

#if defined(__APPLE__)
extern "C" void EditorRenderNativeMoveFrame();
extern "C" bool EditorStartCocoaWindowMove(void* nativeWindow);
#endif

#if defined(_WIN32) || defined(__APPLE__)
	static void renderNativeMoveFrame()
	{
		Application& app = Application::Get();
		app.RenderOneFrame();
		glfwSwapBuffers(static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow()));
	}
#endif

#if defined(__APPLE__)
	extern "C" void EditorRenderNativeMoveFrame()
	{
		renderNativeMoveFrame();
	}
#endif

	static void on_window_size_callback(GLFWwindow* window, int width, int height)
	{
		auto* appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

		if (!appWindow)
			return;

		appWindow->Resize(width, height);
	}

	static void on_framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);

		auto* appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

		if (appWindow)
			appWindow->Resize(width, height);
	}

	static void on_window_refresh_callback(GLFWwindow* window)
	{
		auto* appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

		if (!appWindow)
			return;

		Application::Get().RenderOneFrame();

		glfwSwapBuffers(window);
	}

	static void on_window_close_callback(GLFWwindow* window)
	{
		Application::Get().Close();
	}

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	void Window::Init(const WindowProps& spec)
	{
		printf("%s\n", glfwGetVersionString());
		m_Data = spec;

		if (GLFWWindowCount == 0)
		{
			int success = glfwInit();
			glfwSetErrorCallback(glfw_error_callback);
		}

		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			m_Window = glfwCreateWindow((int)spec.Width, (int)spec.Height, spec.Title.c_str(), nullptr, nullptr);
			GLFWWindowCount++;
		}

#ifdef _WIN32
		enableNativeBorderlessFrame(m_Window);
#endif

		glfwSetWindowUserPointer(m_Window, this);


		{
			GLFWimage images[1];
			images[0].pixels = stbi_load(spec.Icon.c_str(), &images[0].width, &images[0].height, 0, 4);

			glfwSetWindowIcon(m_Window, 1, images);

			stbi_image_free(images[0].pixels);
		}

		{
			glfwSetWindowSizeCallback(m_Window, on_window_size_callback);
			glfwSetFramebufferSizeCallback(m_Window, on_framebuffer_size_callback);
			glfwSetWindowRefreshCallback(m_Window, on_window_refresh_callback);
			glfwSetWindowCloseCallback(m_Window, on_window_close_callback);
			// This callback is invoked when the window needs to be redrawn, including
			// during native move/resize modal loops on some platforms (notably Windows).

			glfwMakeContextCurrent(m_Window);

			SetVSync(spec.VSync);
		}

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);


		if (!status)
		{
			CORE_ERROR("Failed to initialize OpenGL context!");
			exit(-1);
		}

		//std::cout << spec.Title << '\n';
		CORE_INFO(spec);

	}

	Window::Window(const WindowProps& spec) : m_Data(spec)
	{
		Init(spec);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		GLFWWindowCount--;
		if (GLFWWindowCount == 0)
		{
			glfwTerminate();
			CORE_INFO("Terminating GLFW");
		}
	}

	void Window::Update()
	{
		glfwSwapBuffers(m_Window);
		if (m_NativeMovePending)
		{
			m_NativeMovePending = false;
			BeginNativeMove();
		}
		glfwPollEvents();
	}

	bool Window::isMinimized() const
	{
		return minimized;// || glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == GLFW_TRUE;
	}

	void Window::Resize(uint32_t w, uint32_t h)
	{
		m_Data.Width = w;
		m_Data.Height = h;

		minimized = (w == 0 || h == 0);

		//CORE_INFO("Resize: {0} {1}", std::to_string(w), std::to_string(h));
		glViewport(0, 0, w, h);
	}
	void Window::SetVSync(bool interval)
	{
		glfwSwapInterval(interval ? 1 : 0);

		CORE_INFO("VSync: " + std::string(interval ? "Enabled" : "Disabled"));

		m_Data.VSync = interval;
	}

	bool Window::StartNativeMove()
	{
#ifdef _WIN32
		m_NativeMovePending = true;
		return true;
#elif defined(__APPLE__)
		m_NativeMovePending = true;
		return true;
#elif defined(__linux__)
		if (glfwGetPlatform() != GLFW_PLATFORM_X11)
		{
			static bool warnedAboutWayland = false;
			if (!warnedAboutWayland)
			{
				CORE_WARN("Native title-bar dragging is unavailable on Wayland with this GLFW backend.");
				warnedAboutWayland = true;
			}
			return false;
		}

		m_NativeMovePending = true;
		return true;
#else
		return false;
#endif
	}

	void Window::BeginNativeMove()
	{
#ifdef _WIN32
		const HWND window = glfwGetWin32Window(m_Window);
		const bool raisedTimerResolution = timeBeginPeriod(1) == TIMERR_NOERROR;
		if (!raisedTimerResolution)
			CORE_WARN("Could not raise the timer resolution for native dragging.");

		if (!SetTimer(window, NativeMoveTimerId, 1, nullptr))
		{
			CORE_ERROR("Failed to start the native-move render timer: {}", GetLastError());
			if (raisedTimerResolution)
				timeEndPeriod(1);
			return;
		}

		POINT cursorPosition;
		if (!GetCursorPos(&cursorPosition))
		{
			CORE_ERROR("Failed to get cursor position for native window move: {}", GetLastError());
			KillTimer(window, NativeMoveTimerId);
			if (raisedTimerResolution)
				timeEndPeriod(1);
			return;
		}

		ReleaseCapture();
		SendMessageW(
			window,
			WM_NCLBUTTONDOWN,
			HTCAPTION,
			MAKELPARAM(cursorPosition.x, cursorPosition.y));
		KillTimer(window, NativeMoveTimerId);
		if (raisedTimerResolution)
			timeEndPeriod(1);
#elif defined(__APPLE__)
		if (!EditorStartCocoaWindowMove(glfwGetCocoaWindow(m_Window)))
			CORE_WARN("AppKit could not start a native window move.");
#elif defined(__linux__)
		Display* display = glfwGetX11Display();
		const ::Window x11Window = glfwGetX11Window(m_Window);
		if (!display || !x11Window)
		{
			CORE_WARN("Could not access the native X11 window; using the GLFW fallback.");
			return;
		}

		int windowX = 0;
		int windowY = 0;
		double cursorX = 0.0;
		double cursorY = 0.0;
		glfwGetWindowPos(m_Window, &windowX, &windowY);
		glfwGetCursorPos(m_Window, &cursorX, &cursorY);

		XEvent event = {};
		event.xclient.type = ClientMessage;
		event.xclient.window = x11Window;
		const Atom moveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
		if (moveResize == None)
		{
			CORE_WARN("The X11 window manager does not support native window moving.");
			return;
		}
		event.xclient.message_type = moveResize;
		event.xclient.format = 32;
		event.xclient.data.l[0] = windowX + static_cast<int>(cursorX);
		event.xclient.data.l[1] = windowY + static_cast<int>(cursorY);
		event.xclient.data.l[2] = 8;
		event.xclient.data.l[3] = Button1;
		event.xclient.data.l[4] = 1;

		if (!XSendEvent(
			display,
			DefaultRootWindow(display),
			False,
			SubstructureRedirectMask | SubstructureNotifyMask,
			&event))
		{
			CORE_WARN("The X11 window manager rejected the native move request.");
			return;
		}
		XFlush(display);
#endif
	}
}