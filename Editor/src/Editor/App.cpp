#include "pch.h"
#include "App.h"

namespace Editor
{
	Application* Application::Instance = nullptr;

	Application::Application(const ApplicationSpecification& applicationSpecification) : m_Spec(applicationSpecification)
	{
		ASSERT(!Instance, "Application already exists!");
		Instance = this;
		m_WindowHandle = new Window(applicationSpecification.Name);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_WindowHandle->Update();
		}
		Shutdown();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Shutdown()
	{
		delete m_WindowHandle;
	}
}