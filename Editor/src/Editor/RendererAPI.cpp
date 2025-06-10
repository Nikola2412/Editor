#include "pch.h"
#include "RendererAPI.h"

namespace Editor {

	RendererAPI::API RendererAPI::_API = RendererAPI::API::OpenGL;

    //Scope<RendererAPI> RendererAPI::Create()
    //{
    //    switch (_API)
    //    {
    //    case RendererAPI::API::None:    ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
    //    case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
    //    }

    //    ASSERT(false, "Unknown RendererAPI!");
    //    return nullptr;
    //}
}