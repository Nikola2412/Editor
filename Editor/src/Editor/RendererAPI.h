#pragma once

#include "Editor/Core.h"

#include <glm/glm.hpp>

namespace Editor {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		virtual ~RendererAPI() = default;

		//virtual void Init() = 0;
		//virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		//virtual void SetClearColor(const glm::vec4& color) = 0;
		//virtual void Clear() = 0;

		static API GetAPI() { return _API; }

		//static Scope<RendererAPI> Create();
	private:
		static API _API;
	};

}