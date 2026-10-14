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
			if (!m_WindowHandle->isMinimized())
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

		const float time = Time::GetTime();
		timestep = time - lastFrameTime;
		lastFrameTime = time;
		if (timestep.getSeconds() >= 10.0f || m_WindowHandle->isMinimized())
			return;

		m_RenderingFrame = true;
		layer->OnUpdate(timestep);
		layer->Begin();
		layer->dockSpace();
		//layer->UICallBackRender();
		layer->OnUIRender();
		layer->End();
		m_RenderingFrame = false;

		m_FPSWindowTime += timestep.getSeconds();
		++m_FPSWindowFrames;
		if (m_FPSWindowTime >= 0.5f)
		{
			m_FPS = static_cast<int>(m_FPSWindowFrames / m_FPSWindowTime + 0.5f);
			m_FPSWindowTime = 0.0f;
			m_FPSWindowFrames = 0;
		}

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