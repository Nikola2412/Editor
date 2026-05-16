#pragma once

#include <Editor.h>

#include <imgui/imgui.h>

using namespace Editor;
using namespace ImGui;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(const std::string& name) : Layer(name), m_ImgWidth(0.0f), m_ImgHeight(0.0f), m_CurrentTex(0),m_PreviousTex(0){};
	~ExampleLayer() = default;

	virtual void onAttach() override;

	virtual void OnUIRender() override;

	void AddTexture(const std::string& path) {
		m_TextureList.emplace_back(Texture2D::Load(path));
		if (m_TextureList.size() == 1)
			this->setImg();
	}

	inline void setImg() {
		m_CurrentTex = !m_TextureList.empty()
			? (ImTextureID)m_TextureList[m_ListID]->GetRendererID()
			: (ImTextureID)fallback->GetRendererID();
	}

	bool vSync = true;
	Ref<Texture2D> fallback = Texture2D::Load("temp.png");
	Ref<Texture2D> checkerBoard = Texture2D::Load("assets/Checkerboard.png");
private:
	void nextImg(int dir)
	{
		m_PreviousTex = m_CurrentTex;

		m_ListID += dir;

		if (m_ListID < 0)
			m_ListID = (int)m_TextureList.size() - 1;

		if (m_ListID >= m_TextureList.size())
			m_ListID = 0;

		m_CurrentTex = (ImTextureID)m_TextureList[m_ListID]->GetRendererID();

		ImGui::startMorph();
	}

	Ref<Texture2D> m_Next;
	Ref<Texture2D> m_Prev;
	vec<Ref<Texture2D>> m_TextureList;

	//float m_ImgSize = 256.0f;				// current (animated)
	float m_ImgWidth = 256.0f;				// current (animated)
	float m_ImgHeight = 256.0f;				// current (animated)
	//float m_TargetImgSize = 512.0f;		// desired
	float m_TargetImgWidth = 512.0f;		// desired
	float m_TargetImgHeight = 512.0f;		// desired
	float m_SizeSpeed = 12.0f;				// smoothing strength

	float m_Rotation = 0.0f;				// current (rendered)
	float m_TargetRotation = 0.0f;			// desired
	float m_RotationSpeed = 10.0f;			// smoothing strength

	ImTextureID m_CurrentTex = 0;
	ImTextureID m_PreviousTex = 0;

	float m_MorphSpeed = 1.0f;

	size_t m_ListID = 0;
};

void menuUI(Editor::Application* app);
Editor::Application* Editor::CreateApplication();
