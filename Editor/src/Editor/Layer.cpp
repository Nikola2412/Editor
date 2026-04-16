#include "pch.h"
#include "Layer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "App.h"

namespace Editor
{
	void Layer::OnAttach()
	{
		//std::cout << "Layer: " << this->GetName() << " attached" << '\n';
		Log::GetCoreLogger()->Info("Layer: " + this->GetName() + " attached");
		
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

		Application& app = Application::Get();

		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		this->onAttach();

	}
	void Layer::OnDetach()
	{
		//std::cout << "Layer: " << this->GetName() << " detached"<<'\n';
		Log::GetCoreLogger()->Info("Layer: " + this->GetName() + " detached");
		// Cleanup ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Layer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
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
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
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
}