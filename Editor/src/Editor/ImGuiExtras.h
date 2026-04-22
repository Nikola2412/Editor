#pragma once
#include <imgui.h>

#define IM_PI 3.14159265358979323846f

namespace ImGui 
{
	void DrawRotatedImage(
		ImDrawList* draw,
		ImTextureID tex,
		ImVec2 pos,
		ImVec2 size,
		float angle,
		ImU32 tint_col = IM_COL32_WHITE
	);
	void AnimateImageSize(float& currentSize, float targetSize, float sizeSpeed);
	void AnimateImageRotation(float& currentRotation, float targetRotation, float rotationSpeed);
	void AnimatedImage(
		float& currentRotation,
		float targetRotation,
		float rotationSpeed,
		float& currentSize,
		float targetSize,
		float sizeSpeed);
}