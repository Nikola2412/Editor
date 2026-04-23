#pragma once
#include "pch.h"
#include "Core.h"

#include "Window.h"
#include "Layer.h"
#include "Log.h"
#include "Time.h"
#include "Timestep.h"
#include "vec.h"
#include "FileDialog.h"

struct GLFWwindow;

namespace Editor
{
	struct ApplicationSpecification
	{
		std::string Name = "App";
		std::string Icon = "icon1.png";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool VSync = true;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application() = default;

		void Run();
		//void SetUICallBack(const std::function<void()>& UICallback) { m_UICallback = UICallback; }//needs imgui

		Window& GetWindow() { return *m_WindowHandle; };

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

		template<typename T>
		void SetLayer()
		{
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			layer = std::make_shared<T>();
			layer->OnAttach();
		}

		void SetLayer(const std::shared_ptr<Layer>& layer)
		{
			this->layer = layer;
			this->layer->OnAttach();
		}

		void SetVSync(bool interval) { m_WindowHandle->SetVSync(interval); };

		Ref<Layer> GetLayer() { return layer; };

		void Close();

		static Application& Get() { return *Instance; };

	private:
		void Shutdown();

	private:
		ApplicationSpecification m_Spec;
		Window* m_WindowHandle;
		bool m_Running = true;
		float lastFrameTime = 0.0f;

		std::vector<std::shared_ptr<Layer>> m_LayerStack;
		std::function<void()> m_UICallback;

	private:
		static Application* Instance;
	public:
		std::shared_ptr<Layer> layer;
	};

	Application* CreateApplication();
}