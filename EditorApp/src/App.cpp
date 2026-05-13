#include "App.h"

Editor::Application* Editor::CreateApplication()
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";
	spec.Icon = "AppAssets/icon1.png";
	spec.Width = 1600;
	spec.Height = 900;
	spec.VSync = true;

	Editor::Application* app = new Editor::Application(spec);

	auto layer = CreateRef<ExampleLayer>("layer1");

	app->SetLayer(layer);
	layer->SetUICallBack([app]()
		{
			menuUI(app);
		});

	return app;
}

void menuUI(Editor::Application* app)
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
		
        if(ImGui::MenuItem("Settings"))
            ImGui::OpenPopup("SettingsPopup");
        
        if (ImGui::BeginPopup("SettingsPopup"))
        {
            bool* vsyncPtr = &((ExampleLayer*)app->layer.get())->vSync;
            ImGui::Text("Settings");
            if (ImGui::Checkbox("VSync", vsyncPtr)) {
				app->SetVSync(*vsyncPtr);
            }
            ImGui::EndPopup();
        }
		ImGui::EndMenuBar();
	}

	//ImGui::ShowDemoWindow();
}



void ExampleLayer::onAttach()
{
	

}

void ExampleLayer::OnUIRender() {

}