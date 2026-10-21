#include "App.h"
#include <algorithm>

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
    if (ImGui::MenuItem("Save"))
    {
        std::string s;
        if (FileDialog::SaveFile(PNG, s)) {
            Ref<Texture2D> tex = (app->GetLayer<ExampleLayer>())->GetCurrentTexture();
            if (!tex)
            {
                CORE_WARN("There is no image to save.");
                return;
            }
            tex->Save(s);
        }
        else {
            CORE_WARN("File dialog was canceled or an error occurred.");
        }
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
    if (ImGui::MenuItem("Settings")) {
		ImGui::OpenPopup("SettingsPopup");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f));
    if (ImGui::BeginPopup("SettingsPopup"))
    {
        bool* vsyncPtr = &(app->GetLayer<ExampleLayer>()->vSync);
        ImGui::Text("Settings");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
        if (ImGui::Checkbox("VSync", vsyncPtr)) {
            app->SetVSync(*vsyncPtr);
        }
        ImGui::PopStyleVar();
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
    ImGui::PopStyleVar();
}

void ExampleLayer::onAttach()
{

    m_TextureList.reserve(5);

    AddTexture("assets/0.png");
    AddTexture("assets/1.png");
    AddTexture("assets/2.png");
    AddTexture("assets/3.png");
    AddTexture("assets/4.png");

    this->setImg();

    m_Next = Texture2D::Load("assets/next.png");
    m_Prev = Texture2D::Load("assets/prev.png");
}

void ExampleLayer::OnUIRender()
{
    ImGui::Begin("Image Studio");

    ImGui::TextColored(ImVec4(0.40f, 0.70f, 1.00f, 1.00f), "IMAGE");
    ImGui::SameLine(0.0f, 2.0f);
    ImGui::Text("STUDIO");
    ImGui::SameLine(0.0f, 14.0f);
    ImGui::TextDisabled("A calm space for your images");

    const char* slideStatus = m_AutoSlide ? "AUTO PLAY ON" : "AUTO PLAY OFF";
    const float statusWidth = ImGui::CalcTextSize(slideStatus).x;
    const float statusX = ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - statusWidth;
    if (statusX > ImGui::GetCursorPosX() + 24.0f)
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(statusX);
        ImGui::TextColored(
            m_AutoSlide ? ImVec4(0.40f, 0.82f, 0.65f, 1.0f) : ImVec4(0.55f, 0.58f, 0.64f, 1.0f),
            "%s",
            slideStatus
        );
    }
    ImGui::Separator();

    if (m_AutoSlide && !m_TextureList.empty())
    {
        m_SlideTimer += ImGui::GetIO().DeltaTime;

        if (m_SlideTimer >= m_SlideInterval)
        {
            m_SlideTimer = 0.0f;
            nextImg(1);
        }
    }

    AnimateImageRotation(
        m_Rotation,
        m_TargetRotation,
        m_RotationSpeed
    );

    AnimateImageSize(
        m_ImgWidth,
        m_ImgHeight,
        m_TargetImgWidth,
        m_TargetImgHeight,
        m_SizeSpeed
    );

    if (ImGui::BeginChild("PreviewCanvas", ImVec2(0.0f, -100.0f), ImGuiChildFlags_Borders))
    {
        ImGui::TextDisabled("CANVAS");
        ImGui::SameLine();
        ImGui::TextDisabled("|  Click the image to adjust its settings");
        ImGui::Separator();

        if (m_Next && m_Prev && !m_TextureList.empty())
        {
            const float buttonSize = 36.0f;
            const ImVec2 available = ImGui::GetContentRegionAvail();
            const float imageWidth = (std::min)(m_ImgWidth, (std::max)(64.0f, available.x - buttonSize * 2.0f - 64.0f));
            const float imageHeight = (std::min)(m_ImgHeight, (std::max)(64.0f, available.y - 16.0f));
            const ImGuiStyle& style = ImGui::GetStyle();
            const float buttonWidth = buttonSize + style.FramePadding.x * 2.0f;
            const float rowWidth = buttonWidth * 2.0f + imageWidth + style.ItemSpacing.x * 2.0f;
            const float rowHeight = (std::max)(imageHeight, buttonSize);
            const float startX = (std::max)(0.0f, (available.x - rowWidth) * 0.5f);
            const float startY = (std::max)(0.0f, (available.y - rowHeight) * 0.5f);
            const float imageTopY = ImGui::GetCursorPosY() + startY;
            const float buttonTopOffset = (imageHeight - buttonSize) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);
            ImGui::SetCursorPosY(imageTopY + buttonTopOffset);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7.0f, 7.0f));
            if (ImGui::ImageButton(
                "prev",
                (ImTextureID)m_Prev->GetRendererID(),
                ImVec2(buttonSize, buttonSize)))
            {
                nextImg(-1);
                m_SlideTimer = 0.0f;
            }

            ImGui::SameLine();

            ImGui::SetCursorPosY(imageTopY);
            const ImVec2 imageSize(imageWidth, imageHeight);
            const ImVec2 imagePosition = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("imgbtn", imageSize);

            if (m_AnimationSelector == 0)
            {
                SlideImage(
                    m_CurrentTexID,
                    m_PreviousTexID,
                    imagePosition,
                    imageSize,
                    m_SlideDirection,
                    m_AnimationSpeed,
                    m_Rotation
                );
            }
            else if (m_AnimationSelector == 1)
            {
                MorphImage(
                    m_CurrentTexID,
                    m_PreviousTexID,
                    imagePosition,
                    imageSize,
                    m_AnimationSpeed,
                    m_Rotation
                );
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Click to open image settings");

            if (ImGui::IsItemClicked())
                ImGui::OpenPopup("ImageSettings");

            ImGui::SameLine();

            ImGui::SetCursorPosY(imageTopY + buttonTopOffset);
            if (ImGui::ImageButton(
                "next",
                (ImTextureID)m_Next->GetRendererID(),
                ImVec2(buttonSize, buttonSize)))
            {
                nextImg(1);
                m_SlideTimer = 0.0f;
            }
            ImGui::PopStyleVar();

            if (ImGui::BeginPopup("ImageSettings"))
            {
                ImGui::Text("Image Settings");

                ImGui::SliderFloat(
                    "Image Width",
                    &m_TargetImgWidth,
                    128.0f,
                    512.0f
                );

                ImGui::SliderFloat(
                    "Image Height",
                    &m_TargetImgHeight,
                    128.0f,
                    512.0f
                );

                ImGui::SliderFloat(
                    "Image Animation speed",
                    &m_AnimationSpeed,
                    1.0f,
                    20.0f
                );

                ImGui::Separator();
                ImGui::Checkbox("Auto Slide", &m_AutoSlide);

                if (m_AutoSlide)
                {
                    ImGui::SliderFloat(
                        "Slide Timer",
                        &m_SlideInterval,
                        0.5f,
                        30.0f,
                        "%.1f seconds"
                    );

                    float progress = 0.0f;

                    if (m_SlideInterval > 0.0f)
                        progress = m_SlideTimer / m_SlideInterval;

                    progress = std::clamp(progress, 0.0f, 1.0f);
                    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
                }

                ImGui::Separator();
                ImGui::SliderAngle("Rotation", &m_TargetRotation);

                if (ImGui::Button("0°"))
                    m_TargetRotation = 0.0f;

                ImGui::SameLine();
                if (ImGui::Button("90°"))
                    m_TargetRotation = MY_PI * 0.5f;

                ImGui::SameLine();
                if (ImGui::Button("180°"))
                    m_TargetRotation = MY_PI;

                ImGui::SameLine();
                if (ImGui::Button("270°"))
                    m_TargetRotation = MY_PI * 1.5f;

                ImGui::Separator();
                if (ImGui::Button("Reset"))
                {
                    m_TargetImgWidth = 512.0f;
                    m_TargetImgHeight = 512.0f;
                    m_TargetRotation = 0.0f;
                    m_AnimationSpeed = 10.0f;
                    m_AutoSlide = false;
                    m_SlideTimer = 0.0f;
                    m_SlideInterval = 3.0f;
                }

                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    if (m_ListID < m_TextureList.size())
                        m_TextureList.erase(m_ListID);
                    m_ListID = m_TextureList.empty()
                        ? 0
                        : (std::min)(m_ListID, m_TextureList.size() - 1);
                    setImg();
                    m_PreviousTexID = m_CurrentTexID;
                    m_SlideTimer = 0.0f;
                }

                ImGui::SameLine();
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("Preview is unavailable until images and navigation controls are loaded.");
        }
    }
    ImGui::EndChild();

    ImGui::PushStyleVar(
        ImGuiStyleVar_ItemSpacing,
        ImVec2(ImGui::GetStyle().ItemSpacing.x, 3.0f)
    );
    ImGui::TextDisabled("COLLECTION");
    ImGui::SameLine();
    ImGui::TextDisabled("%02d images", static_cast<int>(m_TextureList.size()));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 2.0f));
    if (ImGui::BeginChild("ThumbnailStrip", ImVec2(0.0f, 76.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float thumbnailSize = 48.0f;
        const float thumbnailWidth = thumbnailSize + style.FramePadding.x * 2.0f;
        const float collectionWidth = m_TextureList.empty()
            ? 0.0f
            : thumbnailWidth * m_TextureList.size() + style.ItemSpacing.x * (m_TextureList.size() - 1);
        const ImVec2 available = ImGui::GetContentRegionAvail();
        if (collectionWidth <= available.x)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (available.x - collectionWidth) * 0.5f);
        ImGui::SetCursorPosY(
            ImGui::GetCursorPosY() + (std::max)(0.0f, (available.y - thumbnailSize - style.FramePadding.y * 2.0f) * 0.5f)
        );

        for (size_t i = 0; i < m_TextureList.size(); ++i)
        {
            if (i > 0)
                ImGui::SameLine();

            ImGui::PushID(static_cast<int>(i));
            const bool selected = i == m_ListID;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.42f, 0.68f, 1.0f));

            if (ImGui::ImageButton(
                "##thumbnail",
                (ImTextureID)m_TextureList[i]->GetRendererID(),
                ImVec2(thumbnailSize, thumbnailSize)))
            {
                if (i != m_ListID)
                {
                    m_PreviousTexID = m_CurrentTexID;
                    m_SlideDirection = i > m_ListID ? SlideDirection::R : SlideDirection::L;
                    m_ListID = i;
                    setImg();
                    m_SlideTimer = 0.0f;
                }
            }

            if (selected)
                ImGui::PopStyleColor();
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::End();

#ifdef DEBUG

    ImGui::Begin("Debug Window");

    ImGui::Text(
        "FPS: %d, Frame Time: %.3f ms",
        this->app->GetFPS(),
        this->app->GetTimestep().getMilliseconds()
    );

    ImGui::End();

#endif
}