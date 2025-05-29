#pragma once

#include "pch.h"

#include "Layer.h"


int main(int argc, char* argv[]);

namespace Editor
{
	struct ApplicationSpecification
	{
		std::string Name = "App";
		uint32_t Width = 1600;
		uint32_t Height = 900;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application() = default;

		void Run();
		void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }//needs imgui

		template<typename T>
		void PushLayer()
		{
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void PushLayer(const std::shared_ptr<Layer>& layer) 
		{
			m_LayerStack.emplace_back(layer); 
			layer->OnAttach(); 
		}

		void Close();

	private:
		void Init();
		void Shutdown();


	private:
		ApplicationSpecification m_Spec;

		bool m_Running = false;

		std::vector<std::shared_ptr<Layer>> m_LayerStack;
		std::function<void()> m_MenubarCallback;//needs imgui

	};

	Application* CreateApplication(int argc, char** argv);
}