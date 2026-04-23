#pragma once

#include <Editor.h>

#include <imgui/imgui.h>

using namespace Editor;
using namespace ImGui;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(const std::string& name) : Layer(name), m_ImgSize(256){};
	~ExampleLayer() = default;

	virtual void onAttach() override;

	virtual void OnUIRender() override;

	void AddTexture(const std::string& path) {
		m_TextureList.emplace_back(Texture2D::Load(path));
	}

	bool vSync = true;

private:
	void nextImg(int i) {
       if (m_TextureList.size() == 0)
			return;

		//int size = static_cast<int>(m_TextureList.size());
		//int newIndex = static_cast<int>(m_ListID) + i;
		//newIndex %= size;
		//if (newIndex < 0) newIndex += size;
		//m_ListID = static_cast<unsigned int>(newIndex);

	   if(m_ListID == 0 && i == -1)
		   m_ListID = m_TextureList.size() - 1;
	   else
		   m_ListID = (m_ListID + i) % m_TextureList.size();

		Log::GetClientLogger()->Info(std::to_string(i) + " " + std::to_string(m_TextureList.size()) + " -> " + std::to_string(m_ListID));
	}

	//Ref<Texture2D> m_CheckerBoard;
	Ref<Texture2D> m_Next;
	Ref<Texture2D> m_Prev;
	vec<Ref<Texture2D>> m_TextureList;
	Ref<Texture2D> fallback = Texture2D::Load("temp.png");

	float m_ImgSize = 256.0f;          // current (animated)
	float m_TargetImgSize = 512.0f;    // desired
	float m_SizeSpeed = 12.0f;         // smoothing strength

	float m_Rotation = 0.0f;        // current (rendered)
	float m_TargetRotation = 0.0f;  // desired
	float m_RotationSpeed = 10.0f;  // smoothing strength

	size_t m_ListID = 0;

};

void menuUI(Editor::Application* app);
Editor::Application* Editor::CreateApplication();
