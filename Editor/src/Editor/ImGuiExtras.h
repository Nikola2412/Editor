#pragma once
#include <imgui.h>

#define MY_PI 3.14159265358979323846f

namespace ImGui 
{
	static float	morph = 0;
	static bool		isMorphing = false;
	static bool		isSliding = false;

	static float	m_SlideOffset = 0.0f;
	static float	m_TargetSlideOffset = 0.0f;
	static int		m_SlideDirection = 1;
	static float	m_AnimSpeed = 10.0f;

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
		ImVec2 pos, ImVec2 size,
		float& speed = m_AnimSpeed, float angle = 0);

	void startSlide(int direction, float width);
	void SlideImage(
		ImTextureID currentTex,
		ImTextureID previousTex,
		const ImVec2& pos,
		const ImVec2& size,
		float& speed = m_AnimSpeed,
		float rotation = 0.0f);

	void AnimateImageSize(float& currentSize, float targetSize, float& sizeSpeed = m_AnimSpeed);
	void AnimateImageSize(float& currentWidth, float& currentHeight, float targetWidth, float targetHeight, float& sizeSpeed = m_AnimSpeed);
	void AnimateImageRotation(float& currentRotation, float targetRotation, float& rotationSpeed = m_AnimSpeed);
	void AnimatedImage(
		float& currentRotation,
		float targetRotation,
		float rotationSpeed,
		float& currentSize,
		float targetSize,
		float sizeSpeed);
}