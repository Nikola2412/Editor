#pragma once
#include <Editor.h>

#include <imgui/imgui.h>

using namespace Editor;
using namespace ImGui;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(const std::string& name) : Layer(name) {};
	~ExampleLayer() = default;

	virtual void onAttach() override
	{
		m_CheckerBoard = Texture2D::Load("assets/Checkerboard.png");
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("test");
		Image((ImTextureID)m_CheckerBoard->GetRendererID(), ImVec2(128, 128));
		ImGui::End();
	}
private:
	Ref<Texture2D> m_CheckerBoard;
};

void UI(Editor::Application* app);

Editor::Application* Editor::CreateApplication()
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";

	Editor::Application* app = new Editor::Application(spec);

	auto layer = CreateRef<ExampleLayer>("layer1");

	app->SetLayer(layer);
	layer->SetUICallBack([app]()
		{
			UI(app);
		});

	return app;
}

