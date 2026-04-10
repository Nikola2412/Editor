#include "App.h"


Editor::Application* Editor::CreateApplication()
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";

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
		ImGui::EndMenuBar();
	}

	//ImGui::ShowDemoWindow();
}

void ExampleLayer::onAttach()
{
	m_CheckerBoard = Texture2D::Load("assets/Checkerboard.png");
    m_CheckerBoardSize = 256;

    m_Next = Texture2D::Load("assets/next.png");
    m_Prev = Texture2D::Load("assets/prev.png");

	m_TextureList.reserve(4);

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

        // Interaction
        if (ImGui::IsItemClicked())
            ImGui::OpenPopup("TextureSettings");

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Click to open texture settings.");

        // Popup for Texture Settings
        if (ImGui::BeginPopup("TextureSettings"))
        {
            ImGui::Text("Texture Properties:");

            // Example sliders to manipulate texture properties (assuming these are set somewhere in the texture class)
            static float textureScale = 5.0f;
            ImGui::SliderFloat("Scale", &textureScale, 1.0f, 10.0f, "x%.2f");

            m_CheckerBoardSize = textureScale * 256 / 5.0f;

            /* Not implemented
            static float textureRotation = 0.0f;
            ImGui::SliderFloat("Rotation", &textureRotation, 0.0f, 360.0f, "%.1f deg");

            static bool invertColors = false;
            ImGui::Checkbox("Invert Colors", &invertColors);
            */

            // Apply button for texture properties
            if (ImGui::Button("Reset Changes"))
            {
                textureScale = 5.0f;
            }

            ImGui::EndPopup();
        }
    }
    else
    {
        ImGui::Text("Error: Checkerboard texture not loaded!");
    }

    // Example for some general UI elements like a checkbox and a slider
    static bool enableFeatureX = false;
    ImGui::Checkbox("Enable Feature X", &enableFeatureX);

    static int featureSlider = 50;
    ImGui::SliderInt("Feature Slider", &featureSlider, 0, 100);

    // Buttons that trigger events
    if (ImGui::Button("Reset Settings"))
    {

    }

    ImGui::End();

#pragma endregion
#pragma region Test2_Window
    ImGui::Begin("Test2 Window");
    if (m_Next && m_Prev) {

        ImGui::SetCursorPosY((ImGui::GetWindowContentRegionMax().y / 2 - m_CheckerBoardSize / 2));

        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x / 2  - m_CheckerBoardSize / 2));


        if (ImGui::ImageButton((ImTextureID)m_Prev->GetRendererID(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1)))
        {
            nextImg(-1);
			//std::cout << "Prev button clicked\n";
			Log::GetClientLogger()->Info("Prev button clicked");
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

       

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)m_Next->GetRendererID(), ImVec2(20, 20)))
        {
            nextImg(1);
            //std::cout << "Next button clicked\n";
            Log::GetClientLogger()->Info("Next button clicked");
        }

    }
    ImGui::End();
#pragma endregion

}