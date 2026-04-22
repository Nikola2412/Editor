#include "pch.h"
#include "App.h"

namespace Editor
{
	Application* Application::Instance = nullptr;

	Application::Application(const ApplicationSpecification& appSpec) : m_Spec(appSpec)
	{
		ASSERT(!Instance, "Application already exists!");
		if (Instance) exit(-1);
		Instance = this;
		m_WindowHandle = new Window(WindowProps{
			appSpec.Name,
			appSpec.Icon,
			appSpec.Width,
			appSpec.Height,
			appSpec.VSync
			}
		);
	}

	void Application::Run()
	{
		while (m_Running)
		{

			float time = Time::GetTime();
			Timestep timestep = time - lastFrameTime;
			if (glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_WindowHandle->GetNativeWindow()), GLFW_FOCUSED))
				lastFrameTime = time;

			if (timestep < 10 && !m_WindowHandle->isMinimized())
			{
				layer->OnUpdate(timestep);
				layer->Begin();
				layer->dockSpace();
				layer->UICallBackRender();
				layer->OnUIRender();
				layer->End();
			}
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