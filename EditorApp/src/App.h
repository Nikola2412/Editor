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

	virtual void onAttach() override;

	virtual void OnUIRender() override;


	bool vSync = true;
private:
};

void menuUI(Editor::Application* app);
Editor::Application* Editor::CreateApplication();
