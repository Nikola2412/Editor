#include "pch.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stb_image.h>

#include "App.h"

namespace Editor {
#define BAR_HEIGHT 40

	int dragging = 0;
	double dragStartX, dragStartY;
	int winStartX, winStartY;
	static uint8_t GLFWWindowCount = 0;

	static void on_window_size_callback(GLFWwindow* window, int width, int height)
	{
		auto m_W = static_cast<Window*>(glfwGetWindowUserPointer(window));
		m_W->Resize(width,height);
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
		m_Data = spec;

		if (GLFWWindowCount == 0)
		{
			int success = glfwInit();
			glfwSetErrorCallback(glfw_error_callback);
		}

		{
			glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
			m_Window = glfwCreateWindow((int)spec.Width, (int)spec.Height, spec.Title.c_str(), nullptr, nullptr);
			GLFWWindowCount++;
		}

		glfwSetWindowUserPointer(m_Window, this);


		{
			GLFWimage images[1];
			images[0].pixels = stbi_load(spec.Icon.c_str(), &images[0].width, &images[0].height, 0, 4);

			glfwSetWindowIcon(m_Window, 1, images);

			stbi_image_free(images[0].pixels);
		}

		{
			glfwSetWindowSizeCallback(m_Window, on_window_size_callback);
			glfwSetWindowCloseCallback(m_Window, on_window_close_callback);

			glfwMakeContextCurrent(m_Window);

			SetVSync(spec.VSync);
		}

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);


		if (!status)
		{
			Log::GetCoreLogger()->Error("Failed to initialize OpenGL context!");
			exit(-1);
		}

		//std::cout << spec.Title << '\n';
		Log::GetCoreLogger()->Info(spec);

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
			//std::cout << "Terminating GLFW\n";
			Log::GetCoreLogger()->Info("Terminating GLFW");
		}
	}

	void Window::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Window::Resize(uint32_t w, uint32_t h)
	{
		m_Data.Width = w;
		m_Data.Height = h;

		minimized = (w == 0 || h == 0);

		glViewport(0, 0, m_Data.Width, m_Data.Height);
	}
	void Window::SetVSync(bool interval)
	{
		glfwSwapInterval(interval ? 1 : 0);

		Log::GetCoreLogger()->Info("VSync: " + std::string(interval ? "Enabled" : "Disabled"));

		m_Data.VSync = interval;
	}
}