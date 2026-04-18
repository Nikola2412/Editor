#include "App.h"


Editor::Application* Editor::CreateApplication()
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";
	spec.Icon = "icon1.png";

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
		if (ImGui::MenuItem("Add")) {
			std::string s;
			if (FileDialog::OpenFile("Image Files (*.png)\0*.png;", s)) {
				Log::GetCoreLogger()->Info("Selected file: " + s);
				((ExampleLayer*)app->layer.get())->AddTexture(s);
			}
			else
			{
				Log::GetCoreLogger()->Warn("File dialog was canceled or an error occurred.");
			}
		}
		ImGui::EndMenuBar();
	}

	//ImGui::ShowDemoWindow();
}

void ExampleLayer::onAttach()
{
	m_ImgSize = 256 * 2;

	m_Next = Texture2D::Load("assets/next.png");
	m_Prev = Texture2D::Load("assets/prev.png");

	//m_TextureList.reserve(10);

	m_TextureList.emplace_back(Texture2D::Load("assets/0.png"));
	m_TextureList.emplace_back(Texture2D::Load("assets/1.png"));
	m_TextureList.emplace_back(Texture2D::Load("assets/2.png"));
	m_TextureList.emplace_back(Texture2D::Load("assets/3.png"));
	m_TextureList.emplace_back(Texture2D::Load("assets/4.png"));//Error test

}

void ExampleLayer::OnUIRender() {
	//ImGui::Begin("Test Window");
	//ImVec2 size = ImVec2(m_ImgSize, m_ImgSize);
	//ImVec2 pos = ImGui::GetCursorScreenPos();

	//ImGui::InvisibleButton("imgbtn", size);

	//ImDrawList* draw = ImGui::GetWindowDrawList();
	//float rounding = 10.0f;

	//// Draw rounded image manually
	//draw->AddImageRounded(
	//	(ImTextureID)m_CheckerBoard->GetRendererID(),
	//	pos,
	//	ImVec2(pos.x + size.x, pos.y + size.y),
	//	ImVec2(0, 0),
	//	ImVec2(1, 1),
	//	IM_COL32_WHITE,
	//	rounding
	//);

	//ImGui::End();


#pragma region Test2_Window
    ImGui::Begin("Test2 Window");

    if (m_Next && m_Prev)
    {
        float btnSize = 20.0f;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        float totalWidth = btnSize + spacing + m_ImgSize + spacing + btnSize;

        // Center horizontally
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
        if (startX > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        // Center vertically (optional, cleaner way)
        float totalHeight = m_ImgSize;
        float startY = (ImGui::GetContentRegionAvail().y - totalHeight) * 0.5f;
        if (startY > 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + startY);

        // --- Prev Button ---
        if (ImGui::ImageButton("prev", (ImTextureID)m_Prev->GetRendererID(), ImVec2(btnSize, btnSize)))
        {
            nextImg(-1);
        }

        ImGui::SameLine();

        // --- Image ---
        ImVec2 size(m_ImgSize, m_ImgSize);
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton("imgbtn", size);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddImageRounded(
            !m_TextureList.empty() ? (ImTextureID)m_TextureList[m_ListID]->GetRendererID() : (ImTextureID)fallback->GetRendererID(),
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            ImVec2(0, 0),
            ImVec2(1, 1),
            IM_COL32_WHITE,
            10.0f
        );

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Click");

        if (ImGui::IsItemClicked())
            ImGui::OpenPopup("ImageSettings");

        ImGui::SameLine();

        // --- Next Button ---
        if (ImGui::ImageButton("next", (ImTextureID)m_Next->GetRendererID(), ImVec2(btnSize, btnSize)))
        {
            nextImg(1);
        }

        // --- Popup ---
        if (ImGui::BeginPopup("ImageSettings"))
        {
            static float brightness = 1.0f;
            static float contrast = 1.0f;

            ImGui::Text("Image Settings");
            ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
            ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);
            ImGui::SliderFloat("Image Size", &m_ImgSize, 128.0f, 512.0f);

            if (ImGui::Button("Reset"))
            {
                brightness = 1.0f;
                contrast = 1.0f;
                m_ImgSize = 512.0f;
            }

            ImGui::Separator();

            if (ImGui::Button("Delete"))
            {
                m_TextureList.erase(m_ListID);
                nextImg(1);
            }

            ImGui::Separator();

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }

    ImGui::End();
#pragma endregion

}