#pragma once

struct GLFWwindow;

namespace Editor {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Piranha Engine",
			uint32_t width = 1600,
			uint32_t height = 900) : Title(title), Width(width), Height(height) {
		}
	};

	class Window
	{
	public:
		Window(const WindowProps& spec = WindowProps());
		~Window();

		void Update();

		void Init(const WindowProps& spec);

	private:
		GLFWwindow* m_Window;
	};

}