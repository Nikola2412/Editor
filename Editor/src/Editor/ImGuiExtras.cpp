#include "pch.h"
#include "ImGuiExtras.h"

namespace ImGui
{
    void DrawRotatedImage(ImDrawList* draw, ImTextureID tex, ImVec2 pos, ImVec2 size, float angle, ImU32 tint_col)
    {
        ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

        float cos_a = cosf(angle);
        float sin_a = sinf(angle);

        ImVec2 half = ImVec2(size.x * 0.5f, size.y * 0.5f);

        ImVec2 corners[4] = {
            ImVec2(-half.x, -half.y),
            ImVec2(half.x, -half.y),
            ImVec2(half.x,  half.y),
            ImVec2(-half.x,  half.y)
        };

        ImVec2 rotated[4];
        for (int i = 0; i < 4; i++)
        {
            rotated[i] = ImVec2(
                center.x + corners[i].x * cos_a - corners[i].y * sin_a,
                center.y + corners[i].x * sin_a + corners[i].y * cos_a
            );
        }

        draw->AddImageQuad(
            tex,
            rotated[0], rotated[1], rotated[2], rotated[3],
            ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1),
            tint_col
        );


    }

    void AnimateImageSize(float& currentSize, float targetSize, float sizeSpeed) {
        float dt = ImGui::GetIO().DeltaTime;
        currentSize += (targetSize - currentSize) * (1.0f - expf(-sizeSpeed * dt));
    }

    void AnimateImageRotation(float& currentRotation, float targetRotation, float rotationSpeed)
    {
        float dt = ImGui::GetIO().DeltaTime;

        float delta = targetRotation - currentRotation;

        while (delta > IM_PI)  delta -= IM_PI * 2.0f;
        while (delta < -IM_PI) delta += IM_PI * 2.0f;

        currentRotation += delta * (1.0f - expf(-rotationSpeed * dt));
    }
    void AnimatedImage(float& currentRotation, float targetRotation, float rotationSpeed, float& currentSize, float targetSize, float sizeSpeed)
    {
        float dt = ImGui::GetIO().DeltaTime;

        float delta = targetRotation - currentRotation;

        while (delta > IM_PI)  delta -= IM_PI * 2.0f;
        while (delta < -IM_PI) delta += IM_PI * 2.0f;

        currentRotation += delta * (1.0f - expf(-rotationSpeed * dt));
        currentSize += (targetSize - currentSize) * (1.0f - expf(-sizeSpeed * dt));
    }
}