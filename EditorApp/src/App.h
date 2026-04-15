#pragma once
#include "../resource.h"

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
	void nextImg(int i) { m_ListID = (m_ListID + i) % m_TextureList.size(); }

	Ref<Texture2D> m_CheckerBoard;
	Ref<Texture2D> m_Next;
	Ref<Texture2D> m_Prev;
	vec<Ref<Texture2D>> m_TextureList;

	float m_CheckerBoardSize;

	unsigned int m_ListID = 0;
};

void menuUI(Editor::Application* app);
Editor::Application* Editor::CreateApplication();
