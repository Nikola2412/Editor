#pragma once
#include "Editor/Log.h"

struct GLFWcursor;
struct GLFWwindow;

namespace Editor
{
	class Application;
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_Name(name), app(nullptr) {}
		virtual ~Layer() = default;

		void SetUICallBack(const std::function<void()>& UICallback) { m_UICallback = UICallback; }
	protected:
		virtual void onAttach() {};

	private:
		void OnAttach();
		void OnDetach();

		void Begin();
		void End();

		virtual void OnUpdate(float deltaTime = 0);

		void dockSpace(); // maybe make virtual
		void UICallBackRender();

	protected:
		virtual void OnUIRender() {}

		std::string GetName() const { return m_Name; }

		Application* app;

	private:
		std::function<void()> m_UICallback;
		bool m_dockSpace = false;
		std::string m_Name;
		GLFWwindow* window;

		friend class Application;

	private:
		void applicationDecoration();
		struct WindowBounds
		{
			int x = 0;
			int y = 0;
			int width = 0;
			int height = 0;
		};

		void toggleMaximize();
		void startWindowAnimation(const WindowBounds& target, float targetOpacity, bool minimizeAtEnd);
		void updateWindowAnimation();
		void updateWindowResize(GLFWwindow* window);
		bool loadWindowBounds(WindowBounds& bounds);
		void saveWindowBounds();

		bool m_maximized = false;
		bool m_windowAnimationActive = false;
		bool m_minimizeAfterAnimation = false;
		bool m_minimizePending = false;
		bool m_minimizeRestoreMaximized = false;
		float m_animationElapsedSeconds = 0.0f;
		float m_animationStartOpacity = 1.0f;
		float m_animationTargetOpacity = 1.0f;
		WindowBounds m_restoreBounds;
		WindowBounds m_minimizeRestoreBounds;
		WindowBounds m_animationStartBounds;
		WindowBounds m_animationTargetBounds;

		bool m_draggingWindow = false;
		bool m_resizingWindow = false;
		int m_resizeEdges = 0;
		int m_resizeStartMouseX = 0;
		int m_resizeStartMouseY = 0;
		WindowBounds m_resizeStartBounds;
		GLFWcursor* m_resizeCursorHorizontal = nullptr;
		GLFWcursor* m_resizeCursorVertical = nullptr;
		GLFWcursor* m_resizeCursorDiagonalNWSE = nullptr;
		GLFWcursor* m_resizeCursorDiagonalNESW = nullptr;
		double m_dragStartMouseX = 0.0;
		double m_dragStartMouseY = 0.0;
		int m_dragStartWindowX = 0;
		int m_dragStartWindowY = 0;


	};
} // namespace Editor