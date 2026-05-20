#include "pch.h"
#include "ImGuiExtras.h"
#include <algorithm> 

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

    void MorphImage(ImTextureID currentTex, ImTextureID prevTex, ImVec2 pos, ImVec2 size, float& morphSpeed, float angle)
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

    void startSlide(int direction, float width)
    {
        isSliding = true;
        m_SlideOffset = 0.0f;
        m_SlideDirection = -direction;
        m_TargetSlideOffset =  width;
    }

    void SlideImage(ImTextureID currentTex, ImTextureID previousTex, const ImVec2& pos, const ImVec2& size, float& speed, float rotation)
    {
        if (isSliding)
        {
            float dt = ImGui::GetIO().DeltaTime;

            // Animate
            m_SlideOffset +=
                (m_TargetSlideOffset - m_SlideOffset)
                * speed * dt;

            // Snap
            if (fabs(m_TargetSlideOffset - m_SlideOffset) < 0.5f)
                m_SlideOffset = m_TargetSlideOffset;

            // Progress 0 -> 1
            float t = m_SlideOffset / size.x;

            t = std::clamp(t, 0.0f, 1.0f);

            // Smoothstep easing
            t = t * t * (3.0f - 2.0f * t);

            // Widths
            float newWidth = size.x * t;
            float oldWidth = size.x * (1.0f - t);

            ImVec2 oldPos;
            ImVec2 newPos;

            // ==================================================
            // DIRECTION
            // ==================================================
            // m_SlideDirection = -1  -> prev  (left -> right)
            // m_SlideDirection = 1 -> next  (right -> left)
            // ==================================================

            if (m_SlideDirection == -1)
            {
                // OLD shrinks to left
                oldPos = pos;

                // NEW grows from right
                newPos = ImVec2(
                    pos.x + oldWidth,
                    pos.y
                );
            }
            else
            {
                // OLD shrinks to right
                oldPos = ImVec2(
                    pos.x + newWidth,
                    pos.y
                );

                // NEW grows from left
                newPos = pos;
            }

            ImVec2 oldSize(oldWidth, size.y);
            ImVec2 newSize(newWidth, size.y);

            // Draw old image
            if (oldWidth > 1.0f)
            {
                DrawImage(
                    previousTex,
                    oldPos,
                    oldSize,
                    rotation
                );
            }

            // Draw new image
            if (newWidth > 1.0f)
            {
                DrawImage(
                    currentTex,
                    newPos,
                    newSize,
                    rotation
                );
            }

            // Finish
            if (m_SlideOffset >= m_TargetSlideOffset - 5.0f)
            {
                m_SlideOffset = 0.0f;
                m_TargetSlideOffset = 0.0f;
                isSliding = false;
            }
        }
        else
        {
            DrawImage(
                currentTex,
                pos,
                size,
                rotation,
                IM_COL32_WHITE
            );
        }
    }


    void AnimateImageSize(float& currentSize, float targetSize, float& sizeSpeed) {
        float dt = IM_DELTA_TIME;
        currentSize += (targetSize - currentSize) * (1.0f - expf(-sizeSpeed * dt));
    }

    void AnimateImageSize(float& currentWidth, float& currentHeight, float targetWidth, float targetHeight, float& sizeSpeed)
    {
        float dt = IM_DELTA_TIME;
        currentWidth += (targetWidth - currentWidth) * (1.0f - expf(-sizeSpeed * dt));
        currentHeight += (targetHeight - currentHeight) * (1.0f - expf(-sizeSpeed * dt));
    }


    void AnimateImageRotation(float& currentRotation, float targetRotation, float& rotationSpeed)
    {
        float dt = IM_DELTA_TIME;

        float delta = targetRotation - currentRotation;

        while (delta > MY_PI)  delta -= MY_PI * 2.0f;
        while (delta < -MY_PI) delta += MY_PI * 2.0f;

        currentRotation += delta * (1.0f - expf(-rotationSpeed * dt));
    }
    void AnimatedImage(float& currentRotation, float targetRotation, float& rotationSpeed, float& currentSize, float targetSize, float& sizeSpeed)
    {
        float dt = IM_DELTA_TIME;

        float delta = targetRotation - currentRotation;

        while (delta > MY_PI)  delta -= MY_PI * 2.0f;
        while (delta < -MY_PI) delta += MY_PI * 2.0f;

        currentRotation += delta * (1.0f - expf(-rotationSpeed * dt));
        currentSize += (targetSize - currentSize) * (1.0f - expf(-sizeSpeed * dt));
    }
}