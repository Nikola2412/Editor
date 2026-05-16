#pragma once
#include <imgui.h>

#define MY_PI 3.14159265358979323846f

namespace ImGui 
{
	static float morph = 0;
	static bool isMorphing = false;

	void DrawImage(
		ImTextureID tex,
		ImVec2 pos,
		ImVec2 size,
		float angle = 0,
		ImU32 tint_col = IM_COL32_WHITE
	);
	void DrawCircularImage(
		ImTextureID texture,
		ImVec2 center,
		float radius,
		float angle = 0,
		ImU32 tint = IM_COL32_WHITE);

	void startMorph();
	void MorphImage(ImTextureID currentTex, ImTextureID prevTex,
		float& morphSpeed,
		ImVec2 pos, ImVec2 size, float angle = 0);

	void AnimateImageSize(float& currentSize, float targetSize, float sizeSpeed);
	void AnimateImageSize(float& currentWidth, float& currentHeight, float targetWidth, float targetHeight, float sizeSpeed);
	void AnimateImageRotation(float& currentRotation, float targetRotation, float rotationSpeed);
	void AnimatedImage(
		float& currentRotation,
		float targetRotation,
		float rotationSpeed,
		float& currentSize,
		float targetSize,
		float sizeSpeed);
}