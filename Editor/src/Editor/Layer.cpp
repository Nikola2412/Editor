#include "pch.h"
#include "Layer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <cmath>
#include <fstream>
#include <limits>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "App.h"

namespace Editor
{
	namespace
	{
		constexpr float ApplicationTitleBarHeight = 35.0f;
		constexpr double WindowAnimationDurationSeconds = 0.28;
		constexpr int WindowResizeBorder = 6;
	}

	void Layer::OnAttach()
	{
		//std::cout << "Layer: " << this->GetName() << " attached" << '\n';
		CORE_INFO("Layer: " + this->GetName() + " attached");
		
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// ===== Layout & Shape =====
			style.WindowRounding = 10.0f;
			style.FrameRounding = 6.0f;
			style.PopupRounding = 10.0f;
			style.ScrollbarRounding = 12.0f;
			style.GrabRounding = 6.0f;

			style.WindowPadding = ImVec2(10.0f, 10.0f);
			style.FramePadding = ImVec2(10.0f, 6.0f);
			style.ItemSpacing = ImVec2(10.0f, 8.0f);

			style.ScrollbarSize = 12.0f;
			style.GrabMinSize = 14.0f;

			style.Alpha = 1.0f;

			// ===== Colors (Dark Glass Theme) =====
			ImVec4* colors = style.Colors;

			colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.95f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.13f, 0.90f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.98f);

			colors[ImGuiCol_Border] = ImVec4(0.20f, 0.22f, 0.27f, 0.6f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

			colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.15f, 0.18f, 1.00f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.22f, 0.28f, 1.00f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.28f, 0.35f, 1.00f);

			colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.06f, 0.75f);

			colors[ImGuiCol_Button] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.35f, 0.55f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.30f, 0.50f, 1.00f);

			colors[ImGuiCol_Header] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.35f, 0.55f, 1.00f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.30f, 0.50f, 1.00f);

			colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.35f, 0.55f, 1.00f);
			colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.30f, 0.50f, 1.00f);

			colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.55f, 0.85f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);

			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.60f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.22f, 0.27f, 0.8f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.35f, 0.45f, 0.9f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.40f, 0.50f, 1.0f);

			colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.35f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.35f, 0.50f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.30f, 0.45f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.20f, 0.30f, 0.45f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.40f, 0.60f, 1.00f);
		}

		this->app = Application::GetInstance();
		GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
		glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
		glfwGetWindowSize(window, &m_restoreBounds.width, &m_restoreBounds.height);
		if (loadWindowBounds(m_restoreBounds))
		{
			glfwSetWindowPos(window, m_restoreBounds.x, m_restoreBounds.y);
			glfwSetWindowSize(window, m_restoreBounds.width, m_restoreBounds.height);
		}
		m_resizeCursorHorizontal = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		m_resizeCursorVertical = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		m_resizeCursorDiagonalNWSE = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
		m_resizeCursorDiagonalNESW = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		this->onAttach();

	}
	void Layer::OnDetach()
	{
		//std::cout << "Layer: " << this->GetName() << " detached"<<'\n';
		CORE_INFO("Layer: {} detached", this->GetName());
		saveWindowBounds();
		// Cleanup ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		/*GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
		glfwSetCursor(window, nullptr);
		if (m_resizeCursorHorizontal)
			glfwDestroyCursor(m_resizeCursorHorizontal);
		if (m_resizeCursorVertical)
			glfwDestroyCursor(m_resizeCursorVertical);
		if (m_resizeCursorDiagonalNWSE)
			glfwDestroyCursor(m_resizeCursorDiagonalNWSE);
		if (m_resizeCursorDiagonalNESW)
			glfwDestroyCursor(m_resizeCursorDiagonalNESW);*/
		ImGui::DestroyContext();
	}

	void Layer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		applicationDecoration();
	}
	void Layer::End()
	{

		if(m_dockSpace)
			ImGui::End();
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
	void Layer::OnUpdate(float deltaTime)
	{
		//Log::GetCoreLogger()->Info("Layer: " + this->GetName() + " updated");
		glClearColor(0.1f, 0.1f, 0.1f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Layer::dockSpace()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		
		window_flags |= ImGuiWindowFlags_MenuBar;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(
			viewport->WorkPos.x,
			viewport->WorkPos.y + ApplicationTitleBarHeight
		));
		ImGui::SetNextWindowSize(ImVec2(
			viewport->WorkSize.x,
			viewport->WorkSize.y - ApplicationTitleBarHeight
		));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		m_dockSpace = true;
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("AppDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

	}
	void Layer::UICallBackRender()
	{
		if (m_UICallback)
		{
			m_UICallback();
		}
	}
	void Layer::startWindowAnimation(const WindowBounds& target, float targetOpacity, bool minimizeAtEnd)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
		glfwGetWindowPos(window, &m_animationStartBounds.x, &m_animationStartBounds.y);
		glfwGetWindowSize(window, &m_animationStartBounds.width, &m_animationStartBounds.height);

		m_animationTargetBounds = target;
		m_animationStartOpacity = glfwGetWindowOpacity(window);
		m_animationTargetOpacity = targetOpacity;
		m_animationElapsedSeconds = 0.0f;
		m_minimizeAfterAnimation = minimizeAtEnd;
		m_windowAnimationActive = true;
	}

	bool Layer::loadWindowBounds(WindowBounds& bounds)
	{
		std::ifstream stateFile("window-state.ini");
		WindowBounds savedBounds;
		if (!(stateFile >> savedBounds.x >> savedBounds.y >> savedBounds.width >> savedBounds.height) ||
			savedBounds.width < 320 || savedBounds.height < 200)
			return false;

		bounds = savedBounds;
		return true;
	}

	void Layer::saveWindowBounds()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
		WindowBounds bounds;
		if (m_maximized)
			bounds = m_restoreBounds;
		else if (m_minimizePending || (m_windowAnimationActive && m_minimizeAfterAnimation))
			bounds = m_minimizeRestoreBounds;
		else if (m_windowAnimationActive)
			bounds = m_animationTargetBounds;
		else
		{
			glfwGetWindowPos(window, &bounds.x, &bounds.y);
			glfwGetWindowSize(window, &bounds.width, &bounds.height);
		}

		if (bounds.width < 320 || bounds.height < 200)
			return;

		std::ofstream stateFile("window-state.ini", std::ios::trunc);
		if (stateFile)
			stateFile << bounds.x << ' ' << bounds.y << ' '
				<< bounds.width << ' ' << bounds.height << '\n';
	}

	void Layer::toggleMaximize()
	{
		if (m_windowAnimationActive || m_minimizePending)
			return;

		if (m_maximized)
		{
			m_maximized = false;
			startWindowAnimation(m_restoreBounds, 1.0f, false);
			return;
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
		glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
		glfwGetWindowSize(window, &m_restoreBounds.width, &m_restoreBounds.height);
		if (m_restoreBounds.width <= 0 || m_restoreBounds.height <= 0)
			return;

		int windowCenterX = m_restoreBounds.x + m_restoreBounds.width / 2;
		int windowCenterY = m_restoreBounds.y + m_restoreBounds.height / 2;
		int monitorCount = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		GLFWmonitor* targetMonitor = glfwGetPrimaryMonitor();
		long long nearestDistance = (std::numeric_limits<long long>::max)();

		for (int index = 0; index < monitorCount; ++index)
		{
			int areaX = 0;
			int areaY = 0;
			int areaWidth = 0;
			int areaHeight = 0;
			glfwGetMonitorWorkarea(monitors[index], &areaX, &areaY, &areaWidth, &areaHeight);

			const int dx = windowCenterX < areaX ? areaX - windowCenterX :
				windowCenterX > areaX + areaWidth ? windowCenterX - areaX - areaWidth : 0;
			const int dy = windowCenterY < areaY ? areaY - windowCenterY :
				windowCenterY > areaY + areaHeight ? windowCenterY - areaY - areaHeight : 0;
			const long long distance = static_cast<long long>(dx) * dx + static_cast<long long>(dy) * dy;
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				targetMonitor = monitors[index];
			}
		}

		if (!targetMonitor)
			return;

		WindowBounds targetBounds;
		glfwGetMonitorWorkarea(targetMonitor, &targetBounds.x, &targetBounds.y,
			&targetBounds.width, &targetBounds.height);
		m_maximized = true;
		startWindowAnimation(targetBounds, 1.0f, false);
	}

	void Layer::updateWindowAnimation()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());

		if (m_minimizePending && !glfwGetWindowAttrib(window, GLFW_ICONIFIED))
		{
			m_minimizePending = false;
			m_maximized = m_minimizeRestoreMaximized;
			glfwSetWindowOpacity(window, 0.0f);
			startWindowAnimation(m_minimizeRestoreBounds, 1.0f, false);
		}

		if (!m_windowAnimationActive)
			return;

		m_animationElapsedSeconds += ImGui::GetIO().DeltaTime;
		float progress = m_animationElapsedSeconds /
			static_cast<float>(WindowAnimationDurationSeconds);
		if (progress < 0.0f)
			progress = 0.0f;
		if (progress > 1.0f)
			progress = 1.0f;
		const float easedProgress = progress * progress * (3.0f - 2.0f * progress);

		const int x = static_cast<int>(m_animationStartBounds.x +
			(m_animationTargetBounds.x - m_animationStartBounds.x) * easedProgress);
		const int y = static_cast<int>(m_animationStartBounds.y +
			(m_animationTargetBounds.y - m_animationStartBounds.y) * easedProgress);
		const int width = static_cast<int>(m_animationStartBounds.width +
			(m_animationTargetBounds.width - m_animationStartBounds.width) * easedProgress);
		const int height = static_cast<int>(m_animationStartBounds.height +
			(m_animationTargetBounds.height - m_animationStartBounds.height) * easedProgress);
		const float opacity = m_animationStartOpacity +
			(m_animationTargetOpacity - m_animationStartOpacity) * easedProgress;

		glfwSetWindowPos(window, x, y);
		glfwSetWindowSize(window, width > 0 ? width : 1, height > 0 ? height : 1);
		glfwSetWindowOpacity(window, opacity);

		if (progress >= 1.0f)
		{
			m_windowAnimationActive = false;
			const bool minimize = m_minimizeAfterAnimation;
			m_minimizeAfterAnimation = false;
			if (minimize)
			{
				m_minimizePending = true;
				glfwIconifyWindow(window);
			}
		}
	}

	void Layer::updateWindowResize(GLFWwindow* window)
	{
		constexpr int ResizeLeft = 1;
		constexpr int ResizeRight = 2;
		constexpr int ResizeTop = 4;
		constexpr int ResizeBottom = 8;
		constexpr int MinimumWidth = 320;
		constexpr int MinimumHeight = 200;

		const ImGuiIO& io = ImGui::GetIO();
		const bool canResize = !m_maximized && !m_windowAnimationActive && !m_minimizePending;
		int windowX = 0;
		int windowY = 0;
		int windowWidth = 0;
		int windowHeight = 0;
		double cursorX = 0.0;
		double cursorY = 0.0;
		glfwGetWindowPos(window, &windowX, &windowY);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glfwGetCursorPos(window, &cursorX, &cursorY);

		int hoveredEdges = 0;
		if (canResize && !m_resizingWindow && windowWidth > WindowResizeBorder * 2 && windowHeight > WindowResizeBorder * 2)
		{
			if (cursorX <= WindowResizeBorder)
				hoveredEdges |= ResizeLeft;
			else if (cursorX >= windowWidth - WindowResizeBorder)
				hoveredEdges |= ResizeRight;

			if (cursorY <= WindowResizeBorder)
				hoveredEdges |= ResizeTop;
			else if (cursorY >= windowHeight - WindowResizeBorder)
				hoveredEdges |= ResizeBottom;
		}

		if (m_resizingWindow)
			hoveredEdges = m_resizeEdges;

		GLFWcursor* cursor = nullptr;
		const bool horizontalEdge = (hoveredEdges & (ResizeLeft | ResizeRight)) != 0;
		const bool verticalEdge = (hoveredEdges & (ResizeTop | ResizeBottom)) != 0;
		if (horizontalEdge && verticalEdge)
		{
			const bool northwestSoutheast =
				((hoveredEdges & ResizeLeft) && (hoveredEdges & ResizeTop)) ||
				((hoveredEdges & ResizeRight) && (hoveredEdges & ResizeBottom));
			cursor = northwestSoutheast ? m_resizeCursorDiagonalNWSE : m_resizeCursorDiagonalNESW;
		}
		else if (horizontalEdge)
			cursor = m_resizeCursorHorizontal;
		else if (verticalEdge)
			cursor = m_resizeCursorVertical;
		glfwSetCursor(window, cursor);

		if (!m_resizingWindow && hoveredEdges != 0 && io.MouseClicked[ImGuiMouseButton_Left])
		{
			m_resizingWindow = true;
			m_resizeEdges = hoveredEdges;
			m_resizeStartBounds = { windowX, windowY, windowWidth, windowHeight };
			m_resizeStartMouseX = windowX + static_cast<int>(cursorX);
			m_resizeStartMouseY = windowY + static_cast<int>(cursorY);
		}

		if (!m_resizingWindow)
			return;

		if (!io.MouseDown[ImGuiMouseButton_Left])
		{
			m_resizingWindow = false;
			m_resizeEdges = 0;
			glfwSetCursor(window, nullptr);
			return;
		}

		const int cursorScreenX = windowX + static_cast<int>(cursorX);
		const int cursorScreenY = windowY + static_cast<int>(cursorY);
		const int deltaX = cursorScreenX - m_resizeStartMouseX;
		const int deltaY = cursorScreenY - m_resizeStartMouseY;
		int newX = m_resizeStartBounds.x;
		int newY = m_resizeStartBounds.y;
		int newWidth = m_resizeStartBounds.width;
		int newHeight = m_resizeStartBounds.height;

		if (m_resizeEdges & ResizeLeft)
		{
			newX += deltaX;
			newWidth -= deltaX;
		}
		else if (m_resizeEdges & ResizeRight)
			newWidth += deltaX;

		if (m_resizeEdges & ResizeTop)
		{
			newY += deltaY;
			newHeight -= deltaY;
		}
		else if (m_resizeEdges & ResizeBottom)
			newHeight += deltaY;

		if (newWidth < MinimumWidth)
		{
			if (m_resizeEdges & ResizeLeft)
				newX = m_resizeStartBounds.x + m_resizeStartBounds.width - MinimumWidth;
			newWidth = MinimumWidth;
		}
		if (newHeight < MinimumHeight)
		{
			if (m_resizeEdges & ResizeTop)
				newY = m_resizeStartBounds.y + m_resizeStartBounds.height - MinimumHeight;
			newHeight = MinimumHeight;
		}

		glfwSetWindowSize(window, newWidth, newHeight);
		glfwSetWindowPos(window, newX, newY);
	}

	void Layer::applicationDecoration()
	{
		GLFWwindow* window =
			static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
		updateWindowAnimation();
		updateWindowResize(window);

		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(
			ImVec2(viewport->Pos.x, viewport->Pos.y)
		);

		ImGui::SetNextWindowSize(
			ImVec2(viewport->Size.x, ApplicationTitleBarHeight)
		);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("##ApplicationTitleBar", nullptr, flags);

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const ImVec2 windowPos = ImGui::GetWindowPos();
		const ImVec2 windowSize = ImGui::GetWindowSize();
		const float buttonWidth = 45.0f;

		ImGui::SetCursorPos(ImVec2(static_cast<float>(WindowResizeBorder), static_cast<float>(WindowResizeBorder)));
		ImGui::InvisibleButton(
			"##TitleBarDrag",
			ImVec2(windowSize.x - buttonWidth * 3.0f - WindowResizeBorder * 2.0f,
				ApplicationTitleBarHeight - WindowResizeBorder)
		);

		const bool titleBarClicked = ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		if (titleBarClicked && !m_resizingWindow && !m_windowAnimationActive && !m_minimizePending)
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				toggleMaximize();
				m_draggingWindow = false;
			}
			else
			{
				glfwGetWindowPos(window, &m_dragStartWindowX, &m_dragStartWindowY);
				double cursorX = 0.0;
				double cursorY = 0.0;
				glfwGetCursorPos(window, &cursorX, &cursorY);
				m_dragStartMouseX = m_dragStartWindowX + cursorX;
				m_dragStartMouseY = m_dragStartWindowY + cursorY;
				m_draggingWindow = true;
			}
		}

		if (m_draggingWindow)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				int currentWindowX = 0;
				int currentWindowY = 0;
				glfwGetWindowPos(window, &currentWindowX, &currentWindowY);
				double cursorX = 0.0;
				double cursorY = 0.0;
				glfwGetCursorPos(window, &cursorX, &cursorY);
				const double screenX = currentWindowX + cursorX;
				const double screenY = currentWindowY + cursorY;

				if (m_maximized &&
					(std::abs(screenX - m_dragStartMouseX) > 3.0 ||
						std::abs(screenY - m_dragStartMouseY) > 3.0))
				{
					int currentWidth = 0;
					glfwGetWindowSize(window, &currentWidth, nullptr);
					const double cursorRatio = currentWidth > 0
						? (m_dragStartMouseX - m_dragStartWindowX) / currentWidth
						: 0.5;
					const double titleBarOffset = m_dragStartMouseY - m_dragStartWindowY;
					const int restoredWidth = m_restoreBounds.width > 0
						? m_restoreBounds.width : currentWidth;
					const int restoredHeight = m_restoreBounds.height > 0
						? m_restoreBounds.height : ApplicationTitleBarHeight;
					m_dragStartWindowX = static_cast<int>(screenX - restoredWidth * cursorRatio);
					m_dragStartWindowY = static_cast<int>(screenY - titleBarOffset);
					glfwSetWindowSize(window, restoredWidth, restoredHeight);
					glfwSetWindowPos(window, m_dragStartWindowX, m_dragStartWindowY);
					m_dragStartMouseX = screenX;
					m_dragStartMouseY = screenY;
					m_maximized = false;
					m_restoreBounds.x = m_dragStartWindowX;
					m_restoreBounds.y = m_dragStartWindowY;
				}
				else if (!m_maximized)
				{
					glfwSetWindowPos(
						window,
						m_dragStartWindowX + static_cast<int>(screenX - m_dragStartMouseX),
						m_dragStartWindowY + static_cast<int>(screenY - m_dragStartMouseY)
					);
				}
			}
			else
			{
				m_draggingWindow = false;
				if (!m_maximized)
				{
					glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
					glfwGetWindowSize(window, &m_restoreBounds.width, &m_restoreBounds.height);
				}
			}
		}

		// Background
		drawList->AddRectFilled(
			windowPos,
			ImVec2(
				windowPos.x + windowSize.x,
				windowPos.y + ApplicationTitleBarHeight
			),
			IM_COL32(18, 20, 25, 255)
		);

		// Bottom separator
		drawList->AddLine(
			ImVec2(windowPos.x, windowPos.y + ApplicationTitleBarHeight - 1),
			ImVec2(windowPos.x + windowSize.x,
				windowPos.y + ApplicationTitleBarHeight - 1),
			IM_COL32(45, 48, 58, 255)
		);

		// Application name
		ImGui::SetCursorPos(ImVec2(12.0f, 0.0f));

		ImGui::PushStyleColor(
			ImGuiCol_Text,
			ImVec4(0.85f, 0.87f, 0.92f, 1.0f)
		);

		ImGui::PopStyleColor();

		// ---------------------------------------------------------
		// Window buttons
		// ---------------------------------------------------------

		ImGui::SetCursorPos(
			ImVec2(
				windowSize.x - buttonWidth * 3.0f,
				0.0f
			)
		);

		// Minimize
		if (ImGui::Button("##Minimize", ImVec2(buttonWidth, ApplicationTitleBarHeight)))
		{
			if (!m_windowAnimationActive && !m_minimizePending)
			{
				glfwGetWindowPos(window, &m_minimizeRestoreBounds.x, &m_minimizeRestoreBounds.y);
				glfwGetWindowSize(window, &m_minimizeRestoreBounds.width, &m_minimizeRestoreBounds.height);
				m_minimizeRestoreMaximized = m_maximized;

				WindowBounds minimizedBounds = m_minimizeRestoreBounds;
				const int minimizedWidth = minimizedBounds.width > 72 ? 72 : minimizedBounds.width;
				const int minimizedHeight = minimizedBounds.height > 48 ? 48 : minimizedBounds.height;
				minimizedBounds.x += (minimizedBounds.width - minimizedWidth) / 2;
				minimizedBounds.y += (minimizedBounds.height - minimizedHeight) / 2;
				minimizedBounds.width = minimizedWidth;
				minimizedBounds.height = minimizedHeight;
				startWindowAnimation(minimizedBounds, 0.0f, true);
			}
		}

		// Draw minimize icon
		{
			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			float cx = (min.x + max.x) * 0.5f;
			float cy = (min.y + max.y) * 0.5f;

			drawList->AddLine(
				ImVec2(cx - 7, cy + 3),
				ImVec2(cx + 7, cy + 3),
				IM_COL32(210, 210, 215, 255),
				1.5f
			);
		}

		ImGui::SameLine(0, 0);

		// Maximize / restore
		if (ImGui::Button("##Maximize", ImVec2(buttonWidth, ApplicationTitleBarHeight)))
		{
			toggleMaximize();
		}

		{
			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			float cx = (min.x + max.x) * 0.5f;
			float cy = (min.y + max.y) * 0.5f;

			drawList->AddRect(
				ImVec2(cx - 6, cy - 5),
				ImVec2(cx + 6, cy + 5),
				IM_COL32(210, 210, 215, 255),
				0.0f,
				0,
				1.5f
			);
		}

		ImGui::SameLine(0, 0);

		// Close
		if (ImGui::Button("##Close", ImVec2(buttonWidth, ApplicationTitleBarHeight)))
		{
			app->Close();
		}

		{
			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			float cx = (min.x + max.x) * 0.5f;
			float cy = (min.y + max.y) * 0.5f;

			drawList->AddLine(
				ImVec2(cx - 6, cy - 6),
				ImVec2(cx + 6, cy + 6),
				IM_COL32(220, 220, 225, 255),
				1.5f
			);

			drawList->AddLine(
				ImVec2(cx + 6, cy - 6),
				ImVec2(cx - 6, cy + 6),
				IM_COL32(220, 220, 225, 255),
				1.5f
			);
		}



		ImGui::End();

		ImGui::PopStyleVar(3);
	}

}