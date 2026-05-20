#include "pch.h"
#include "RendererAPI.h"

#include <glad/glad.h>

#include "Log.h"

namespace Editor {

	RendererAPI::API RendererAPI::_API = RendererAPI::API::OpenGL;

    void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:            CORE_ERROR("OpenGL HIGH severity: {}", message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:          CORE_ERROR("OpenGL MEDIUM severity: {}", message); return;
        case GL_DEBUG_SEVERITY_LOW:             CORE_WARN("OpenGL LOW severity: {}", message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION:    CORE_INFO("OpenGL NOTIFICATION: {}", message); return;
        }

        ASSERT(false, "Unknown severity level!");
    }

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

    void RendererAPI::Init()
    {
#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
    }

}