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
	m_CheckerBoard = Texture2D::Load("assets/Checkerboard.png");
    m_CheckerBoardSize = 256 * 2;

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
#pragma region Test_Window
    ImGui::Begin("Test Window");

    if (m_CheckerBoard)
    {
        ImVec2 size = ImVec2(m_CheckerBoardSize, m_CheckerBoardSize);
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton("imgbtn", size);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        float rounding = 10.0f;

        // Draw rounded image manually
        draw->AddImageRounded(
            (ImTextureID)m_CheckerBoard->GetRendererID(),
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            ImVec2(0, 0),
            ImVec2(1, 1),
            IM_COL32_WHITE,
            rounding
        );
    }
    else
    {
        ImGui::Text("Error: Checkerboard texture not loaded!");
    }

    ImGui::End();

#pragma endregion
#pragma region Test2_Window
    ImGui::Begin("Test2 Window");
    if (m_Next && m_Prev) {

        ImGui::SetCursorPosY((ImGui::GetWindowContentRegionMax().y / 2 - m_CheckerBoardSize / 2));

        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x / 2  - m_CheckerBoardSize / 2));


        if (ImGui::ImageButton("prev", (ImTextureID)m_Prev->GetRendererID(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1)))
        {
            nextImg(-1);
			Log::GetClientLogger()->Info("Prev button clicked");
			Log::GetClientLogger()->Info("Current List ID: " + std::to_string(this->m_ListID) + " / " + std::to_string(m_TextureList.size() - 1));
        }

		ImGui::SameLine();
        
        ImVec2 size = ImVec2(m_CheckerBoardSize, m_CheckerBoardSize);
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton("imgbtn", size);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        float rounding = 10.0f;

        draw->AddImageRounded(
            (ImTextureID)m_TextureList[m_ListID]->GetRendererID(),
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            ImVec2(0, 0),
            ImVec2(1, 1),
            IM_COL32_WHITE,
            rounding
        );
        

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Click");

        if (ImGui::IsItemClicked()) {
            std::string s;
            if (FileDialog::OpenFile("Image Files (*.png)\0*.png;", s)) {
                Log::GetCoreLogger()->Info("Selected file: " + s);
                AddTexture(s);
            }
            else
            {
                Log::GetCoreLogger()->Warn("File dialog was canceled or an error occurred.");
            }
        }
       

        ImGui::SameLine();

        if (ImGui::ImageButton("next",(ImTextureID)m_Next->GetRendererID(), ImVec2(20, 20)))
        {
            nextImg(1);
            Log::GetClientLogger()->Info("Next button clicked");
            Log::GetClientLogger()->Info("Current List ID: " + std::to_string(this->m_ListID) + " / " + std::to_string(m_TextureList.size() - 1));
        }

    }
    ImGui::End();
#pragma endregion

}