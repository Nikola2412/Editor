#pragma once
#include <Editor.h>

#include <imgui/imgui.h>

void UI(Editor::Application* app);

Editor::Application* Editor::CreateApplication()
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";

	Editor::Application* app = new Editor::Application(spec);

	auto layer = std::make_shared<Editor::Layer>("layer1");

	app->SetLayer(layer);
	layer->SetUICallBack([app]()
		{
			UI(app);
		});

	return app;
}

