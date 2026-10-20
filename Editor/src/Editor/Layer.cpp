#include "pch.h"
#include "Layer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <cmath>
#include <fstream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "App.h"

namespace Editor
{
	namespace
	{
		constexpr float ApplicationTitleBarHeight = 40.0f;
		constexpr int WindowResizeBorder = 6;
	}

	void Layer::OnAttach()
	{
		//std::cout << "Layer: " << this->GetName() << " attached" << '\n';
		CORE_INFO("Layer: {0} attached", this->GetName());

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
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(12.0f, 10.0f);
		style.FramePadding = ImVec2(10.0f, 6.0f);
		style.CellPadding = ImVec2(8.0f, 5.0f);
		style.ItemSpacing = ImVec2(10.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
		style.IndentSpacing = 22.0f;
		style.ScrollbarSize = 12.0f;
		style.GrabMinSize = 12.0f;

		style.WindowRounding = 0.0f;
		style.ChildRounding = 6.0f;
		style.FrameRounding = 5.0f;
		style.PopupRounding = 7.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabRounding = 5.0f;
		style.TabRounding = 5.0f;

		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		style.Alpha = 1.0f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.91f, 0.92f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.53f, 0.56f, 0.62f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.22f, 0.24f, 0.29f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.21f, 0.26f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.25f, 0.31f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.27f, 0.33f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.32f, 0.36f, 0.43f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.45f, 0.53f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.43f, 0.66f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.57f, 0.91f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.49f, 0.70f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.17f, 0.19f, 0.23f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.34f, 0.51f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.29f, 0.43f, 0.65f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.17f, 0.21f, 0.29f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.31f, 0.46f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.39f, 0.58f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.25f, 0.30f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.51f, 0.75f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.66f, 1.00f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.35f, 0.57f, 0.91f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.35f, 0.57f, 0.91f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.43f, 0.66f, 1.00f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.36f, 0.53f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.18f, 0.22f, 0.30f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.43f, 0.66f, 1.00f, 1.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.15f, 0.17f, 0.22f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.35f, 0.57f, 0.91f, 0.55f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.68f, 0.85f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.43f, 0.66f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.43f, 0.66f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.58f, 0.75f, 1.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.14f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.24f, 0.27f, 0.32f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.19f, 0.21f, 0.26f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.15f, 0.17f, 0.21f, 0.35f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.43f, 0.65f, 0.45f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.78f, 1.00f, 1.00f);
		colors[ImGuiCol_NavCursor] = ImVec4(0.43f, 0.66f, 1.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.75f, 0.82f, 0.93f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.05f, 0.06f, 0.08f, 0.65f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.04f, 0.05f, 0.07f, 0.70f);

		this->app = Application::GetInstance();
		m_ApplicationIcon = Texture2D::Load(app->GetSpecification().Icon);
		this->window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());
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
		const bool nativeMoveCompleted = app->GetWindow().ConsumeNativeMoveCompleted();
		if (nativeMoveCompleted)
			ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Left, false);
		ImGui::NewFrame();
		updateMinimizeState();
		if (m_nativeMoveTracking &&
			(nativeMoveCompleted || !ImGui::IsMouseDown(ImGuiMouseButton_Left)))
		{
			m_nativeMoveTracking = false;
			m_draggingWindow = false;
			m_maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
			if (!m_maximized)
			{
				glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
				glfwGetWindowSize(window, &m_restoreBounds.width, &m_restoreBounds.height);
			}
		}
		updateWindowResize(window);
	}
	void Layer::End()
	{

		if (m_dockSpace)
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
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
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
		const float menuBarPaddingY = (ApplicationTitleBarHeight - ImGui::GetFontSize()) * 0.5f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
			ImVec2(ImGui::GetStyle().FramePadding.x, menuBarPaddingY));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		m_dockSpace = true;
		applicationDecoration();
		ImGui::PopStyleVar(2);

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

		WindowBounds bounds;
		if (m_maximized)
			bounds = m_restoreBounds;
		else if (m_minimizePending)
			bounds = m_minimizeRestoreBounds;
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
		if (m_minimizePending)
			return;

		if (m_maximized)
		{
			m_maximized = false;
			glfwRestoreWindow(window);
			return;
		}

		glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
		glfwGetWindowSize(window, &m_restoreBounds.width, &m_restoreBounds.height);
		if (m_restoreBounds.width <= 0 || m_restoreBounds.height <= 0)
			return;

		m_maximized = true;
		glfwMaximizeWindow(window);
	}

	void Layer::updateMinimizeState()
	{
		if (m_minimizePending && !glfwGetWindowAttrib(window, GLFW_ICONIFIED))
		{
			m_minimizePending = false;
			m_maximized = m_minimizeRestoreMaximized;
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
		const bool canResize = !m_maximized && !m_minimizePending;
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

		if (newWidth != windowWidth || newHeight != windowHeight)
			glfwSetWindowSize(window, newWidth, newHeight);
		if (newX != windowX || newY != windowY)
			glfwSetWindowPos(window, newX, newY);
	}

	void Layer::applicationDecoration()
	{
		GLFWwindow* window =
			static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const ImVec2 windowPos = ImGui::GetWindowPos();
		const ImVec2 windowSize = ImGui::GetWindowSize();
		const float buttonWidth = 50.0f;
		drawList->AddRectFilled(
			windowPos,
			ImVec2(windowPos.x + windowSize.x, windowPos.y + ApplicationTitleBarHeight),
			IM_COL32(18, 20, 25, 255)
		);
		drawList->AddLine(
			ImVec2(windowPos.x, windowPos.y + ApplicationTitleBarHeight - 1),
			ImVec2(windowPos.x + windowSize.x, windowPos.y + ApplicationTitleBarHeight - 1),
			IM_COL32(45, 48, 58, 255)
		);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(18.0f / 255.0f, 20.0f / 255.0f, 25.0f / 255.0f, 1.0f));
		if (ImGui::BeginMenuBar())
		{
			if (m_ApplicationIcon && m_ApplicationIcon->IsLoaded())
			{
				const ImVec2 iconMin(
					windowPos.x + 12.0f,
					windowPos.y + (ApplicationTitleBarHeight - 19.0f) * 0.5f);
				drawList->AddImage(
					(ImTextureID)m_ApplicationIcon->GetRendererID(),
					iconMin,
					ImVec2(iconMin.x + 19.0f, iconMin.y + 19.0f),
					ImVec2(0.0f, 0.0f),
					ImVec2(1.0f, 1.0f),
					IM_COL32(235, 238, 245, 255)
				);
			}
			ImGui::SetCursorPosX(40.0f);
			const ImGuiStyle& style = ImGui::GetStyle();
			ImGui::PushStyleVar(
				ImGuiStyleVar_FramePadding,
				ImVec2(6.0f, (ApplicationTitleBarHeight - ImGui::GetFontSize()) * 0.5f));
			UICallBackRender();
			ImGui::PopStyleVar();

			const ImVec2 mousePosition = ImGui::GetMousePos();
			const bool overWindowControls = mousePosition.x >=
				windowPos.x + windowSize.x - buttonWidth * 3.0f;
			const bool overTitleBar = mousePosition.y >= windowPos.y &&
				mousePosition.y < windowPos.y + ApplicationTitleBarHeight;
			const bool titleBarHit = overTitleBar && ImGui::IsWindowHovered() &&
				!ImGui::IsAnyItemHovered() && !overWindowControls;
			const bool titleBarDoubleClicked = titleBarHit &&
				ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
			const bool titleBarClicked = titleBarHit &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			if (titleBarDoubleClicked && !m_resizingWindow && !m_minimizePending)
			{
				toggleMaximize();
				m_draggingWindow = false;
				m_nativeMoveTracking = false;
			}
			else if (titleBarClicked && !m_resizingWindow && !m_minimizePending)
			{
				glfwGetWindowPos(window, &m_dragStartWindowX, &m_dragStartWindowY);
				double cursorX = 0.0;
				double cursorY = 0.0;
				glfwGetCursorPos(window, &cursorX, &cursorY);
				m_dragStartMouseX = m_dragStartWindowX + cursorX;
				m_dragStartMouseY = m_dragStartWindowY + cursorY;
				if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED) != GLFW_TRUE)
				{
					glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
					glfwGetWindowSize(
						window, &m_restoreBounds.width, &m_restoreBounds.height);
				}
				m_draggingWindow = true;
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
						glfwRestoreWindow(window);
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
						const double dragDistanceX = screenX - m_dragStartMouseX;
						const double dragDistanceY = screenY - m_dragStartMouseY;
						if (dragDistanceX * dragDistanceX + dragDistanceY * dragDistanceY > 9.0 &&
							app->GetWindow().StartNativeMove())
						{
							m_nativeMoveTracking = true;
							m_draggingWindow = false;
						}
						else
						{
							const int targetX = m_dragStartWindowX +
								static_cast<int>(dragDistanceX);
							const int targetY = m_dragStartWindowY +
								static_cast<int>(dragDistanceY);
							glfwSetWindowPos(window, targetX, targetY);
						}
					}
				}
				else
				{
					m_draggingWindow = false;
					if (!m_maximized)
					{
						glfwGetWindowPos(window, &m_restoreBounds.x, &m_restoreBounds.y);
						glfwGetWindowSize(window, &m_restoreBounds.width, &m_restoreBounds.height);

						int windowX = 0;
						int windowY = 0;
						double localCursorX = 0.0;
						double localCursorY = 0.0;
						glfwGetWindowPos(window, &windowX, &windowY);
						glfwGetCursorPos(window, &localCursorX, &localCursorY);
						const int cursorX = windowX + static_cast<int>(localCursorX);
						const int cursorY = windowY + static_cast<int>(localCursorY);

						int monitorCount = 0;
						GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
						for (int index = 0; index < monitorCount; ++index)
						{
							int workX = 0;
							int workY = 0;
							int workWidth = 0;
							int workHeight = 0;
							glfwGetMonitorWorkarea(
								monitors[index], &workX, &workY, &workWidth, &workHeight);

							if (cursorX >= workX && cursorX < workX + workWidth &&
								cursorY <= workY + 2)
							{
								m_maximized = true;
								glfwMaximizeWindow(window);
								break;
							}
						}
					}
				}
			}

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

			const float controlsStartX = windowPos.x + windowSize.x - buttonWidth * 3.0f;
			ImGui::SetCursorScreenPos(ImVec2(controlsStartX, windowPos.y));

			// Minimize
			if (ImGui::Button("##Minimize", ImVec2(buttonWidth, ApplicationTitleBarHeight)))
			{
				if (!m_minimizePending)
				{
					glfwGetWindowPos(window, &m_minimizeRestoreBounds.x, &m_minimizeRestoreBounds.y);
					glfwGetWindowSize(window, &m_minimizeRestoreBounds.width, &m_minimizeRestoreBounds.height);
					m_minimizeRestoreMaximized = m_maximized;
					m_minimizePending = true;
					glfwIconifyWindow(window);
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

			// Maximize / restore
			ImGui::SetCursorScreenPos(ImVec2(controlsStartX + buttonWidth, windowPos.y));
			if (ImGui::Button("##Maximize", ImVec2(buttonWidth, ApplicationTitleBarHeight)))
			{
				toggleMaximize();
			}

			{
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();

				float cx = (min.x + max.x) * 0.5f;
				float cy = (min.y + max.y) * 0.5f;

				if (m_maximized)
				{
					drawList->AddRect(
						ImVec2(cx - 2, cy - 6),
						ImVec2(cx + 6, cy + 2),
						IM_COL32(210, 210, 215, 255),
						0.0f,
						0,
						1.5f
					);
					drawList->AddRect(
						ImVec2(cx - 6, cy - 2),
						ImVec2(cx + 2, cy + 6),
						IM_COL32(210, 210, 215, 255),
						0.0f,
						0,
						1.5f
					);
				}
				else
				{
					drawList->AddRect(
						ImVec2(cx - 6, cy - 5),
						ImVec2(cx + 6, cy + 5),
						IM_COL32(210, 210, 215, 255),
						0.0f,
						0,
						1.5f
					);
				}
			}

			// Close
			ImGui::SetCursorScreenPos(ImVec2(controlsStartX + buttonWidth * 2.0f, windowPos.y));
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
			ImGui::EndMenuBar();
		}
		ImGui::PopStyleColor();
	}

}