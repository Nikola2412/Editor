#include "pch.h"
#include "ImGuiExtras.h"

#include "Log.h"

#define IM_DELTA_TIME ImGui::GetIO().DeltaTime

namespace ImGui
{
    void DrawImage(ImTextureID tex, ImVec2 pos, ImVec2 size, float angle, ImU32 tint_col)
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();

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

    void DrawCircularImage(ImTextureID texture, ImVec2 center, float radius, float angle, ImU32 tint)
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();

        const int segments = 64;

        draw->PathClear();

        for (int i = 0; i < segments; i++)
        {
            float a = (2.0f * MY_PI * i) / segments;

            float x = cosf(a) * radius;
            float y = sinf(a) * radius;

            draw->PathLineTo(ImVec2(center.x + x, center.y + y));
        }

        draw->PathFillConvex(IM_COL32_WHITE);

        ImVec2 corners[4] =
        {
            ImVec2(-radius, -radius),
            ImVec2(radius, -radius),
            ImVec2(radius,  radius),
            ImVec2(-radius,  radius)
        };

        ImVec2 rotated[4];

        float c = cosf(angle);
        float s = sinf(angle);

        for (int i = 0; i < 4; i++)
        {
            rotated[i].x = center.x + corners[i].x * c - corners[i].y * s;
            rotated[i].y = center.y + corners[i].x * s + corners[i].y * c;
        }

        draw->AddImageQuad(
            texture,
            rotated[0], rotated[1], rotated[2], rotated[3],
            ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1),
            tint
        );
    }

    void startMorph()
    {
        morph = 0.0f; 
        isMorphing = true;
    }

    void MorphImage(ImTextureID currentTex, ImTextureID prevTex, float& morphSpeed, ImVec2 pos, ImVec2 size, float angle)
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();

        if (isMorphing)
        {
            morph += IM_DELTA_TIME * morphSpeed;

            if (morph >= 1.0f)
            {
                morph = 1.0f;
                isMorphing = false;
            }

            int oldAlpha = (int)((1.0f - morph) * 255.0f);
            int newAlpha = (int)(morph * 255.0f);

            // old image
            DrawImage(
                prevTex,
                pos,
                size,
                angle,
                IM_COL32(255, 255, 255, oldAlpha)
            );

            // new image
            DrawImage(
                currentTex,
                pos,
                size,
                angle,
                IM_COL32(255, 255, 255, newAlpha)
            );
        }
        else
        {
            DrawImage(
                currentTex,
                pos,
                size,
                angle,
                IM_COL32_WHITE
            );
        }
    }

    void AnimateImageSize(float& currentSize, float targetSize, float sizeSpeed) {
        float dt = IM_DELTA_TIME;
        currentSize += (targetSize - currentSize) * (1.0f - expf(-sizeSpeed * dt));
    }

    void AnimateImageSize(float& currentWidth, float& currentHeight, float targetWidth, float targetHeight, float sizeSpeed)
    {
        float dt = IM_DELTA_TIME;
        currentWidth += (targetWidth - currentWidth) * (1.0f - expf(-sizeSpeed * dt));
		currentHeight += (targetHeight - currentHeight) * (1.0f - expf(-sizeSpeed * dt));
    }


    void AnimateImageRotation(float& currentRotation, float targetRotation, float rotationSpeed)
    {
        float dt = IM_DELTA_TIME;

        float delta = targetRotation - currentRotation;

        while (delta > MY_PI)  delta -= MY_PI * 2.0f;
        while (delta < -MY_PI) delta += MY_PI * 2.0f;

        currentRotation += delta * (1.0f - expf(-rotationSpeed * dt));
    }
    void AnimatedImage(float& currentRotation, float targetRotation, float rotationSpeed, float& currentSize, float targetSize, float sizeSpeed)
    {
        float dt = IM_DELTA_TIME;

        float delta = targetRotation - currentRotation;

        while (delta > MY_PI)  delta -= MY_PI * 2.0f;
        while (delta < -MY_PI) delta += MY_PI * 2.0f;

        currentRotation += delta * (1.0f - expf(-rotationSpeed * dt));
        currentSize += (targetSize - currentSize) * (1.0f - expf(-sizeSpeed * dt));
    }
}