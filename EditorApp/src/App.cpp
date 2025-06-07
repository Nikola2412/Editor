#include "App.h"

void UI(Editor::Application* app)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Save"))
		{
			app->Close();
		}
		if (ImGui::MenuItem("Exit"))
		{
			app->Close();
		}
		ImGui::EndMenuBar();
	}

	ImGui::ShowDemoWindow();
}