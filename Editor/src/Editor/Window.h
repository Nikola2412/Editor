#pragma once

struct GLFWwindow;

namespace Editor {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		bool VSync;

		WindowProps(const std::string& title = "Piranha Engine",
			uint32_t width = 1600,
			uint32_t height = 900,
			bool VSync = true) : Title(title), Width(width), Height(height), VSync(VSync) {
		}
	};

	class Window
	{
	public:
		Window(const WindowProps& spec = WindowProps());
		~Window();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }

		void Update();

		void Resize(uint32_t w,uint32_t h);

		void SetVSync(bool enable = true);

		void Init(const WindowProps& spec);

		void* GetNativeWindow() const { return m_Window; };

	private:
		GLFWwindow* m_Window;
		WindowProps m_Data;
	};

}