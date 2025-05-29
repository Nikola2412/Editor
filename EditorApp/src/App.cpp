#include <Editor.h>

class ExampleLayer : public Editor::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	virtual void OnUIRender() override
	{

		ImGui::ShowDemoWindow();
	}
};


Editor::Application* Editor::CreateApplication(int argc, char** argv) 
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";
	spec.Width = 1600;
	spec.Height = 900;


	Editor::Application* app = new Editor::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app] ()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save"))
				{
					app->Close();
				}
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}

