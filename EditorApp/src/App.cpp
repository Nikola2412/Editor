#include "App.h"

Editor::Application* Editor::CreateApplication()
{
	Editor::ApplicationSpecification spec;
	spec.Name = "Editor";
	spec.Icon = "AppAssets/icon1.png";
	spec.Width = 1600;
	spec.Height = 900;
	spec.VSync = true;

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
            std::string s;
            if (FileDialog::SaveFile(PNG, s)) {
                Ref<Texture2D> tex = (app->GetLayer<ExampleLayer>())->GetCurrentTexture();
                int res = tex->Save(s);
            }
            else {
                CORE_WARN("File dialog was canceled or an error occurred.");
            }
        }
        if (ImGui::MenuItem("Exit"))
        {
            app->Close();
        }
        if (ImGui::MenuItem("Add")) {
            std::string s;
            if (FileDialog::OpenFile(PNG, s)) {
                CORE_INFO("Selected file: {}", s);
                (app->GetLayer<ExampleLayer>())->AddTexture(s);
            }
            else
            {
                CORE_WARN("File dialog was canceled or an error occurred.");
            }
        }
        if (ImGui::MenuItem("Settings"))
            ImGui::OpenPopup("SettingsPopup");

        if (ImGui::BeginPopup("SettingsPopup"))
        {
            bool* vsyncPtr = &(app->GetLayer<ExampleLayer>()->vSync);
            ImGui::Text("Settings");
            if (ImGui::Checkbox("VSync", vsyncPtr)) {
                app->SetVSync(*vsyncPtr);
            }
			Timestep ts = app->GetTimestep();
			ImGui::Text("Frame Time: %.3f ms (%d FPS)", ts.getMilliseconds(), app->GetFPS());

            ImGui::Separator();
            const char* availableAnimations[] =
            {
                "Slide Animation",
                "Morph Animation"
            };

            ImGui::Text("Choose animation type:");
            ImGui::Combo("##animation_combo",
                &(app->GetLayer<ExampleLayer>()->m_AnimationSelector),
                availableAnimations,
                IM_ARRAYSIZE(availableAnimations));
            ImGui::EndPopup();
        }
        ImGui::EndMenuBar();
    }

    //ImGui::ShowDemoWindow();
}



void ExampleLayer::onAttach()
{
	m_Next = Texture2D::Load("assets/next.png");
	m_Prev = Texture2D::Load("assets/prev.png");

	m_TextureList.reserve(5);

	AddTexture("assets/0.png");
	AddTexture("assets/1.png");
	AddTexture("assets/2.png");
	AddTexture("assets/3.png");
    AddTexture("assets/4.png");
    this->setImg();

}

void ExampleLayer::OnUIRender() {

#pragma region Test_Window
    ImGui::Begin("Test Window");

    if (m_Next && m_Prev)
    {
		AnimateImageRotation(m_Rotation, m_TargetRotation, m_RotationSpeed);
        AnimateImageSize(m_ImgWidth, m_ImgHeight, m_TargetImgWidth, m_TargetImgHeight, m_SizeSpeed);
		//AnimatedImage(m_Rotation, m_TargetRotation, m_RotationSpeed, m_ImgSize, m_TargetImgSize, m_SizeSpeed);

        float btnSize = 20.0f;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        float totalWidth = btnSize + spacing + m_ImgWidth + spacing + btnSize;

        // Center horizontally
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
        if (startX > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        // Center vertically
        float totalHeight = m_ImgHeight;
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
        ImVec2 size(m_ImgWidth, m_ImgHeight);
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton("imgbtn", size);


        // Draw rotated image
        if (m_AnimationSelector == 0) {
            SlideImage(
                m_CurrentTexID,
                m_PreviousTexID,
                pos,
                size,
                m_SlideDirection,
                m_AnimationSpeed,
                m_Rotation
            );
        }
        else if(m_AnimationSelector == 1){
            MorphImage(
                m_CurrentTexID,
                m_PreviousTexID,
                pos,
                size,
                m_AnimationSpeed,
                m_Rotation
            );
		}
       

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

        if (ImGui::BeginPopup("ImageSettings"))
        {

            ImGui::Text("Image Settings");
            
            ImGui::SliderFloat("Image Width", &m_TargetImgWidth, 128.0f, 512.0f);
            ImGui::SliderFloat("Image Height", &m_TargetImgHeight, 128.0f, 512.0f);
            ImGui::SliderFloat("Image Animation speed", &m_AnimationSpeed, 1.0f, 20.0f);

            ImGui::Separator();

            ImGui::SliderAngle("Rotation", &m_TargetRotation);

            if (ImGui::Button("0°"))   m_TargetRotation = 0.0f;
            ImGui::SameLine();
            if (ImGui::Button("90°"))  m_TargetRotation = MY_PI * 0.5f;
            ImGui::SameLine();
            if (ImGui::Button("180°")) m_TargetRotation = MY_PI;
            ImGui::SameLine();
            if (ImGui::Button("270°")) m_TargetRotation = MY_PI * 1.5f;

            ImGui::Separator();

            if (ImGui::Button("Reset"))
            {
                m_TargetImgWidth = 512.0f;
                m_TargetImgHeight = 512.0f;
                m_TargetRotation = 0.0f;
                m_AnimationSpeed = 10.0f;
            }

            ImGui::Separator();

            if (ImGui::Button("Delete"))
            {
                m_TextureList.erase(m_ListID);
                nextImg(0);
            }

            ImGui::Separator();

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }

    ImGui::End();
#pragma endregion
#ifdef DEBUG
	ImGui::Begin("Debug Window");
	ImGui::Text("FPS: %d, Frame Time: %.3f ms", this->app->GetFPS(), this->app->GetTimestep().getMilliseconds());
	ImGui::End();

#endif // DEBUG


}