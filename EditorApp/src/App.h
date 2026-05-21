#pragma once

#include <Editor.h>

#include <imgui/imgui.h>

using namespace Editor;
using namespace ImGui;

class ExampleLayer : public Layer
{
public:
	ExampleLayer(const std::string& name) : Layer(name), m_ImgWidth(0.0f), m_ImgHeight(0.0f), m_CurrentTexID(0),m_PreviousTexID(0){};
	~ExampleLayer() = default;

	virtual void onAttach() override;

	virtual void OnUIRender() override;

	void AddTexture(const std::string& path) {
		m_TextureList.emplace_back(Texture2D::Load(path));
		if (m_TextureList.size() == 1)
			this->setImg();
	}

	inline void setImg() {
		m_CurrentTexID = (ImTextureID)GetCurrentTexture()->GetRendererID();
	}
	int m_AnimationSelector = 0;
	bool vSync = true;
	inline Ref<Texture2D> GetCurrentTexture() { return m_TextureList.empty() ? fallback : m_TextureList[m_ListID]; }
private:
	void nextImg(int dir)
	{
		m_PreviousTexID = m_CurrentTexID;
		m_SlideDirection = dir > 0 ? SlideDirection::R : SlideDirection::L;

		if (m_ListID == 0 && dir < 0)
			m_ListID = m_TextureList.size() - 1;
		else
			m_ListID = (m_ListID + dir) % m_TextureList.size();

		setImg();
	}

	Ref<Texture2D> m_Next;
	Ref<Texture2D> m_Prev;
	Ref<Texture2D> fallback = Texture2D::Load("temp.png");
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


	float m_AnimationSpeed = 10.0f;


	SlideDirection m_SlideDirection = SlideDirection::R;

	ImTextureID m_CurrentTexID = 0;
	ImTextureID m_PreviousTexID = 0;


	size_t m_ListID = 0;

};

void menuUI(Editor::Application* app);
Editor::Application* Editor::CreateApplication();
