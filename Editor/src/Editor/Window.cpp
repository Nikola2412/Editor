#include "pch.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "App.h"

namespace Editor {

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
			m_Window = glfwCreateWindow((int)spec.Width, (int)spec.Height, spec.Title.c_str(), nullptr, nullptr);
			GLFWWindowCount++;
		}

		glfwSetWindowUserPointer(m_Window, this);

		glfwSetWindowSizeCallback(m_Window, on_window_size_callback);
		glfwSetWindowCloseCallback(m_Window, on_window_close_callback);

		glfwMakeContextCurrent(m_Window);

		SetVSync(spec.VSync);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status)
		{
			fprintf(stderr, "Failed to initialize OpenGL context!\n");
			exit(-1);
		}

		std::cout << spec.Title << '\n';

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
			std::cout << "Terminating GLFW\n";
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

		glViewport(0, 0, m_Data.Width, m_Data.Height);
	}
	void Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
}