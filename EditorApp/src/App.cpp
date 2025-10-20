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
}

void ExampleLayer::OnUIRender() {

	ImGui::Begin("Test Window");

	if (m_CheckerBoard)
	{
        if (ImGui::ImageButton((ImTextureID)m_CheckerBoard->GetRendererID(), ImVec2(256, 256)))
        {
            ImGui::OpenPopup("TextureSettings");
        }

        // Tooltip when hovering over image
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Click to open texture settings.");
        }

        // Popup for Texture Settings
        if (ImGui::BeginPopup("TextureSettings"))
        {
            ImGui::Text("Texture Properties:");

            // Example sliders to manipulate texture properties (assuming these are set somewhere in the texture class)
            static float textureScale = 1.0f;
            ImGui::SliderFloat("Scale", &textureScale, 0.1f, 5.0f, "x%.2f");

            static float textureRotation = 0.0f;
            ImGui::SliderFloat("Rotation", &textureRotation, 0.0f, 360.0f, "%.1f°");

            static bool invertColors = false;
            ImGui::Checkbox("Invert Colors", &invertColors);

            // Apply button for texture properties
            if (ImGui::Button("Apply Changes"))
            {
                // Apply texture scale, rotation, and color inversion (your actual texture class logic will be here)
                //ApplyTextureSettings(m_CheckerBoard, textureScale, textureRotation, invertColors);
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
}