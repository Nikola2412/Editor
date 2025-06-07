#include "pch.h"
#include "App.h"

namespace Editor
{
	Application* Application::Instance = nullptr;

	Application::Application(const ApplicationSpecification& applicationSpecification) : m_Spec(applicationSpecification)
	{
		ASSERT(!Instance, "Application already exists!");
		if (Instance) exit(-1);
		Instance = this;
		m_WindowHandle = new Window(applicationSpecification.Name);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			layer->OnUpdate();

			layer->Begin();
			{
				layer->dockSpace();
				layer->UICallBackRender();
				layer->OnUIRender();
			}
			layer->End();

			m_WindowHandle->Update();
		}
		layer->OnDetach();
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