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

	/*Application::~Application()
	{
		Shutdown();
		Instance = nullptr;
	}*/

	void Application::Run()
	{

		while (m_Running)
		{
			float time = Time::GetTime();
			timestep = time - lastFrameTime;

			if (glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_WindowHandle->GetNativeWindow()), GLFW_FOCUSED))
				lastFrameTime = time;
			

			if (timestep.getSeconds() < 10 && !m_WindowHandle->isMinimized())
				RenderOneFrame();

			m_WindowHandle->Update();
		}
		layer->OnDetach();
		//Shutdown();
	}

	void Application::RenderOneFrame()
	{
		// This mirrors the per-frame UI steps used in Run(), but performs only the UI render
		// so it is safe to call from the refresh callback.
		if (!layer || m_RenderingFrame)
			return;

		m_RenderingFrame = true;
		layer->OnUpdate(timestep);
		layer->Begin();
		layer->dockSpace();
		layer->UICallBackRender();
		layer->OnUIRender();
		layer->End();
		m_RenderingFrame = false;

		//m_WindowHandle->Update();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Shutdown()
	{
		if (m_WindowHandle)
		{
			delete m_WindowHandle;
			m_WindowHandle = nullptr;
		}
	}

}