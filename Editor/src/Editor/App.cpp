#include "pch.h"
#include "App.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Editor
{
	static uint8_t GLFWWindowCount = 0;

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	Application::Application(const ApplicationSpecification& applicationSpecification) : m_Spec(applicationSpecification)
	{
		Init();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glfwPollEvents();

			// Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			{
				static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
				// because it would be confusing to have two docking targets within each others.
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
				if (m_MenubarCallback)
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
				ImGui::PopStyleVar();

				ImGui::PopStyleVar(2);

				// Submit the DockSpace
				ImGuiIO& io = ImGui::GetIO();
				if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
				{
					ImGuiID dockspace_id = ImGui::GetID("AppDockspace");
					ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
				}

				if (m_MenubarCallback)
				{
					if (ImGui::BeginMenuBar())
					{
						m_MenubarCallback();
						ImGui::EndMenuBar();
					}
				}

				for (auto& layer : m_LayerStack)
					layer->OnUIRender();

				ImGui::End();
			}

			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)m_Spec.Width, (float)m_Spec.Height);

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(m_WindowHandle, &display_w, &display_h);

			glViewport(0, 0, display_w, display_h);
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow* backup_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_context);
			}


			// Swap buffers to display the rendered frame for the main window
			glfwSwapBuffers(m_WindowHandle);
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		Shutdown();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Init()
	{
		if (GLFWWindowCount == 0)
		{
			int success = glfwInit();
			glfwSetErrorCallback(glfw_error_callback);
		}

		{
			m_WindowHandle = glfwCreateWindow((int)m_Spec.Width, (int)m_Spec.Height, m_Spec.Name.c_str(), nullptr, nullptr);
			GLFWWindowCount++;
		}

		glfwMakeContextCurrent(m_WindowHandle);

		// Enable VSync for smoother rendering (optional)
		glfwSwapInterval(1);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status)
		{
			fprintf(stderr, "Failed to initialize OpenGL context!\n");
			exit(-1);
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void Application::Shutdown()
	{
		glfwDestroyWindow(m_WindowHandle);
		GLFWWindowCount--;
		if (GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}
}