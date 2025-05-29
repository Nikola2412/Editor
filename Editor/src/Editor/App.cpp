#include "pch.h"
#include "App.h"

namespace Editor
{
	Application::Application(const ApplicationSpecification& applicationSpecification) : m_Spec(applicationSpecification)
	{
		Init();
	}
	void Application::Run()
	{
		while (1)
		{
			std::cout << m_Spec.Name << '\n';
			

			if (m_MenubarCallback)
			{
				/*
				if (ImGui::BeginMenuBar())
				{
					m_MenubarCallback();
					ImGui::EndMenuBar();
				}
				*/
			}

			for(auto& layer : m_LayerStack)
			{
				std::cout << "Layer: " << layer.get()->GetName() << '\n';
				layer->OnUIRender();
			}
			
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}
	void Application::Init()
	{

	}
	void Application::Shutdown()
	{

	}
}