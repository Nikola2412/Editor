#pragma once
#include <Editor.h>

#include <imgui/imgui.h>

using namespace Editor;
using namespace ImGui;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(const std::string& name) : Layer(name), m_CheckerBoardSize(256){};
	~ExampleLayer() = default;

	virtual void onAttach() override;

	virtual void OnUIRender() override;
private:
	Ref<Texture2D> m_CheckerBoard;
	unsigned int m_CheckerBoardSize;
};

void menuUI(Editor::Application* app);
Editor::Application* Editor::CreateApplication();
