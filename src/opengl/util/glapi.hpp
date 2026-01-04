#ifndef OPENGL_API
#define OPENGL_API

#include "gldef.h"
#include "gldebug.hpp"

namespace gl {
    inline bool& DebugEnabled()
    {
        static bool enabled = false;
        return enabled;
    }

    // Set or clear debug mode at app startup
    inline void SetDebugMode(bool on) { DebugEnabled() = on; }

    static bool gExtensionsLoaded = false;

    auto load_extensions() {
        if (!load_gl_extensions()) {
            throw std::runtime_error("Failed to load OpenGL extensions");
        }

        int NumberOfExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
        std::cout << NumberOfExtensions << std::endl;
        for (int i = 0; i < NumberOfExtensions; i++)
        {
            const GLubyte* one_string = glGetStringi(GL_EXTENSIONS, i);
            // std::cout << one_string << std::endl;
        }
        gExtensionsLoaded = true;
    }

    bool validOpenGL() {
        return gExtensionsLoaded && DebugEnabled();
    }

    // Central forwarding function
//    template<typename Fn, typename... Args>
//    auto Dispatch(const char* funcName, Fn&& fn, Args&&... args) -> decltype(fn(std::forward<Args>(args)...))
//    {
//        if (DebugEnabled()) {
//            // In debug mode, run your error?check wrapper
//            return GLCall(funcName, std::forward<Fn>(fn), std::forward<Args>(args)...);
//        }
//        else {
//            // In release mode, call the raw function directly
//            return fn(std::forward<Args>(args)...);
//        }
//    }
//
//    /*
//    example
//
//    inline decltype(auto) fn_wglChoosePixelFormatARB(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats)
//    {
//        return Dispatch("wglChoosePixelFormatARB", wglChoosePixelFormatARB, hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
//    }
//
//    inline decltype(auto) fn_wglCreateContextAttribsARB(HDC hdc, HGLRC hShareContext, const int* attribList)
//    {
//        return Dispatch("wglCreateContextAttribsARB", wglCreateContextAttribsARB, hdc, hShareContext, attribList);
//    }
//    */
//
//    //------------------------------------------------------------------------------
//// Macro to auto-generate a forwarding wrapper around ANY GL/WGL function pointer
////------------------------------------------------------------------------------
//#define DEFINE_GL_WRAPPER(name)                                                   \
//    template<typename... Args>                                                    \
//    inline decltype(auto) ##name(Args&&... args)                                         \
//        -> auto                          \
//    {                                                                              \
//        return Dispatch(#name, ::name, std::forward<Args>(args)...);              \
//    }
//
//
//    //==================================================================================================
//    // WGL Extension Functions
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(wglChoosePixelFormatARB)
//    DEFINE_GL_WRAPPER(wglCreateContextAttribsARB)
//    DEFINE_GL_WRAPPER(wglSwapIntervalEXT)
//    DEFINE_GL_WRAPPER(wglGetExtensionsStringARB)
//
//
//    //==================================================================================================
//    // Context & Capability Queries
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glGetString)
//    DEFINE_GL_WRAPPER(glGetStringi)
//    DEFINE_GL_WRAPPER(glGetIntegerv)
//    DEFINE_GL_WRAPPER(glGetFloatv)
//    DEFINE_GL_WRAPPER(glGetBooleanv)
//    DEFINE_GL_WRAPPER(glGetDoublev)
//    DEFINE_GL_WRAPPER(glGetError)
//
//
//    //==================================================================================================
//    // State Management
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glViewport)
//    DEFINE_GL_WRAPPER(glScissor)
//    DEFINE_GL_WRAPPER(glEnable)
//    DEFINE_GL_WRAPPER(glDisable)
//    DEFINE_GL_WRAPPER(glEnablei)
//    DEFINE_GL_WRAPPER(glDisablei)
//    DEFINE_GL_WRAPPER(glDepthFunc)
//    DEFINE_GL_WRAPPER(glDepthMask)
//    DEFINE_GL_WRAPPER(glDepthRange)
//    DEFINE_GL_WRAPPER(glColorMask)
//    DEFINE_GL_WRAPPER(glCullFace)
//    DEFINE_GL_WRAPPER(glFrontFace)
//    DEFINE_GL_WRAPPER(glPolygonMode)
//    DEFINE_GL_WRAPPER(glPolygonOffset)
//    DEFINE_GL_WRAPPER(glLineWidth)
//    DEFINE_GL_WRAPPER(glPointSize)
//    DEFINE_GL_WRAPPER(glBlendFunc)
//    DEFINE_GL_WRAPPER(glBlendFuncSeparate)
//    DEFINE_GL_WRAPPER(glBlendFuncSeparatei)
//    DEFINE_GL_WRAPPER(glBlendEquation)
//    DEFINE_GL_WRAPPER(glBlendEquationSeparate)
//    DEFINE_GL_WRAPPER(glBlendEquationi)
//    DEFINE_GL_WRAPPER(glBlendColor)
//    DEFINE_GL_WRAPPER(glLogicOp)
//    DEFINE_GL_WRAPPER(glStencilFunc)
//    DEFINE_GL_WRAPPER(glStencilMask)
//    DEFINE_GL_WRAPPER(glStencilOp)
//    DEFINE_GL_WRAPPER(glStencilFuncSeparate)
//    DEFINE_GL_WRAPPER(glStencilOpSeparate)
//    DEFINE_GL_WRAPPER(glPixelStorei)
//    DEFINE_GL_WRAPPER(glBlendFunci)
//    DEFINE_GL_WRAPPER(glColorMaski)
//    DEFINE_GL_WRAPPER(glBlendEquationSeparatei)
//
//
//    //==================================================================================================
//    // Shader & Program Management
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glCreateShader)
//    DEFINE_GL_WRAPPER(glShaderSource)
//    DEFINE_GL_WRAPPER(glCompileShader)
//    DEFINE_GL_WRAPPER(glGetShaderiv)
//    DEFINE_GL_WRAPPER(glGetShaderInfoLog)
//    DEFINE_GL_WRAPPER(glGetShaderSource)
//
//    DEFINE_GL_WRAPPER(glCreateProgram)
//    DEFINE_GL_WRAPPER(glAttachShader)
//    DEFINE_GL_WRAPPER(glLinkProgram)
//    DEFINE_GL_WRAPPER(glGetProgramiv)
//    DEFINE_GL_WRAPPER(glGetProgramInfoLog)
//    DEFINE_GL_WRAPPER(glDetachShader)
//    DEFINE_GL_WRAPPER(glDeleteProgram)
//
//    DEFINE_GL_WRAPPER(glDeleteBuffers)
//    DEFINE_GL_WRAPPER(glDeleteVertexArrays)
//    DEFINE_GL_WRAPPER(glIsShader)
//    DEFINE_GL_WRAPPER(glIsProgram)
//    DEFINE_GL_WRAPPER(glDeleteShader)
//    DEFINE_GL_WRAPPER(glUseProgram)
//
//
//    //==================================================================================================
//    // Vertex Arrays & Buffer Objects
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glGenVertexArrays)
//    DEFINE_GL_WRAPPER(glCreateVertexArrays)
//    DEFINE_GL_WRAPPER(glBindVertexArray)
//    DEFINE_GL_WRAPPER(glEnableVertexAttribArray)
//    DEFINE_GL_WRAPPER(glDisableVertexAttribArray)
//    DEFINE_GL_WRAPPER(glVertexAttribPointer)
//    DEFINE_GL_WRAPPER(glVertexAttribFormat)
//    DEFINE_GL_WRAPPER(glVertexAttribBinding)
//    DEFINE_GL_WRAPPER(glVertexAttribDivisor)
//    DEFINE_GL_WRAPPER(glBindVertexBuffer)
//    DEFINE_GL_WRAPPER(glVertexArrayVertexBuffer)
//
//    //==================================================================================================
//    // Buffer Storage & Data
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glGenBuffers)
//    DEFINE_GL_WRAPPER(glCreateBuffers)
//    DEFINE_GL_WRAPPER(glBindBuffer)
//    DEFINE_GL_WRAPPER(glBufferData)
//    DEFINE_GL_WRAPPER(glBufferStorage)
//    DEFINE_GL_WRAPPER(glNamedBufferData)
//    DEFINE_GL_WRAPPER(glNamedBufferStorage)
//    DEFINE_GL_WRAPPER(glBufferSubData)
//    DEFINE_GL_WRAPPER(glNamedBufferSubData)
//    DEFINE_GL_WRAPPER(glMapBufferRange)
//    DEFINE_GL_WRAPPER(glMapBuffer)
//    DEFINE_GL_WRAPPER(glUnmapBuffer)
//    DEFINE_GL_WRAPPER(glGetBufferSubData)
//    DEFINE_GL_WRAPPER(glMapNamedBufferRange)
//    DEFINE_GL_WRAPPER(glUnmapNamedBuffer)
//    DEFINE_GL_WRAPPER(glMapNamedBuffer)
//    DEFINE_GL_WRAPPER(glClearNamedBufferData)
//    DEFINE_GL_WRAPPER(glGetNamedBufferParameteriv)
//    DEFINE_GL_WRAPPER(glInvalidateBufferData)
//    DEFINE_GL_WRAPPER(glInvalidateBufferSubData)
//    DEFINE_GL_WRAPPER(glClearNamedBufferSubData)
//    DEFINE_GL_WRAPPER(glCopyNamedBufferSubData)
//
//
//    //==============================================================================
//    // Framebuffers & Renderbuffers
//    //==============================================================================
//    DEFINE_GL_WRAPPER(glGenFramebuffers)
//    DEFINE_GL_WRAPPER(glCreateFramebuffers)
//    DEFINE_GL_WRAPPER(glBindFramebuffer)
//    DEFINE_GL_WRAPPER(glFramebufferTexture2D)
//    DEFINE_GL_WRAPPER(glFramebufferTexture)
//    DEFINE_GL_WRAPPER(glFramebufferRenderbuffer)
//    DEFINE_GL_WRAPPER(glRenderbufferStorage)
//    DEFINE_GL_WRAPPER(glBindRenderbuffer)
//    DEFINE_GL_WRAPPER(glBlitFramebuffer)
//    DEFINE_GL_WRAPPER(glCheckFramebufferStatus)
//    DEFINE_GL_WRAPPER(glInvalidateFramebuffer)
//    DEFINE_GL_WRAPPER(glInvalidateSubFramebuffer)
//    DEFINE_GL_WRAPPER(glGenRenderbuffers)
//    DEFINE_GL_WRAPPER(glDeleteRenderbuffers)
//    DEFINE_GL_WRAPPER(glDeleteFramebuffers)
//    DEFINE_GL_WRAPPER(glNamedFramebufferTexture)
//    DEFINE_GL_WRAPPER(glDrawBuffer)
//    DEFINE_GL_WRAPPER(glDrawBuffers)
//    DEFINE_GL_WRAPPER(glReadBuffer)
//    DEFINE_GL_WRAPPER(glInvalidateNamedFramebufferData)
//    DEFINE_GL_WRAPPER(glInvalidateNamedFramebufferSubData)
//    DEFINE_GL_WRAPPER(glNamedRenderbufferStorage)
//    DEFINE_GL_WRAPPER(glNamedFramebufferRenderbuffer)
//    DEFINE_GL_WRAPPER(glCreateRenderbuffers)
//    DEFINE_GL_WRAPPER(glNamedFramebufferDrawBuffer)
//    DEFINE_GL_WRAPPER(glNamedFramebufferDrawBuffers)
//    DEFINE_GL_WRAPPER(glCheckNamedFramebufferStatus)
//
//
//    //==============================================================================
//    // Textures & Samplers
//    //==============================================================================
//    DEFINE_GL_WRAPPER(glGenTextures)
//    DEFINE_GL_WRAPPER(glCreateTextures)
//    DEFINE_GL_WRAPPER(glBindTexture)
//    DEFINE_GL_WRAPPER(glTexImage2D)
//    DEFINE_GL_WRAPPER(glTexStorage2D)
//    DEFINE_GL_WRAPPER(glTexStorage3D)
//    DEFINE_GL_WRAPPER(glTextureSubImage2D)
//    DEFINE_GL_WRAPPER(glTexSubImage2D)
//    DEFINE_GL_WRAPPER(glCompressedTexImage2D)
//    DEFINE_GL_WRAPPER(glCompressedTexSubImage2D)
//    DEFINE_GL_WRAPPER(glGenerateMipmap)
//    DEFINE_GL_WRAPPER(glGenerateTextureMipmap)
//    DEFINE_GL_WRAPPER(glActiveTexture)
//    DEFINE_GL_WRAPPER(glTextureParameteri)
//    DEFINE_GL_WRAPPER(glTextureParameterf)
//    DEFINE_GL_WRAPPER(glTexParameteri)
//    DEFINE_GL_WRAPPER(glTexParameterfv)
//    DEFINE_GL_WRAPPER(glTexParameterIiv)
//    DEFINE_GL_WRAPPER(glTexParameterIuiv)
//    DEFINE_GL_WRAPPER(glBindImageTexture)
//    DEFINE_GL_WRAPPER(glGetTexImage)
//    DEFINE_GL_WRAPPER(glGetCompressedTexImage)
//    DEFINE_GL_WRAPPER(glBindTextureUnit)
//    DEFINE_GL_WRAPPER(glDeleteTextures)
//    DEFINE_GL_WRAPPER(glTextureStorage1D)
//    DEFINE_GL_WRAPPER(glTextureStorage2D)
//    DEFINE_GL_WRAPPER(glTextureStorage3D)
//    DEFINE_GL_WRAPPER(glClearTexSubImage)
//    DEFINE_GL_WRAPPER(glClearTexImage)
//
//    //------------------------------------------------------------------------------
//    // Sampler Objects
//    //------------------------------------------------------------------------------
//    DEFINE_GL_WRAPPER(glGenSamplers)
//    DEFINE_GL_WRAPPER(glDeleteSamplers)
//    DEFINE_GL_WRAPPER(glBindSampler)
//    DEFINE_GL_WRAPPER(glSamplerParameteri)
//    DEFINE_GL_WRAPPER(glSamplerParameterf)
//
//
//    //==============================================================================
//    // Uniforms & Uniform Buffer Objects
//    //==============================================================================
//    DEFINE_GL_WRAPPER(glGetUniformLocation)
//    DEFINE_GL_WRAPPER(glUniform1i)
//    DEFINE_GL_WRAPPER(glUniform2i)
//    DEFINE_GL_WRAPPER(glUniform3i)
//    DEFINE_GL_WRAPPER(glUniform4i)
//    DEFINE_GL_WRAPPER(glUniform1f)
//    DEFINE_GL_WRAPPER(glUniform2f)
//    DEFINE_GL_WRAPPER(glUniform3f)
//    DEFINE_GL_WRAPPER(glUniform4f)
//    DEFINE_GL_WRAPPER(glUniform1d)
//    DEFINE_GL_WRAPPER(glUniform2d)
//    DEFINE_GL_WRAPPER(glUniform3d)
//    DEFINE_GL_WRAPPER(glUniform4d)
//    DEFINE_GL_WRAPPER(glUniformMatrix2fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix3fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix4fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix2x3fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix3x2fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix2x4fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix4x2fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix3x4fv)
//    DEFINE_GL_WRAPPER(glUniformMatrix4x3fv)
//
//    DEFINE_GL_WRAPPER(glGetUniformfv)
//    DEFINE_GL_WRAPPER(glGetUniformiv)
//    DEFINE_GL_WRAPPER(glGetUniformBlockIndex)
//    DEFINE_GL_WRAPPER(glUniformBlockBinding)
//    DEFINE_GL_WRAPPER(glBindBufferBase)
//    DEFINE_GL_WRAPPER(glBindBufferRange)
//    DEFINE_GL_WRAPPER(glGetActiveUniformBlockiv)
//    DEFINE_GL_WRAPPER(glGetActiveUniformBlockName)
//
//    DEFINE_GL_WRAPPER(glProgramUniformMatrix3fv)
//    DEFINE_GL_WRAPPER(glUniform2iv)
//    DEFINE_GL_WRAPPER(glUniform3iv)
//    DEFINE_GL_WRAPPER(glUniform4iv)
//
//
//    //------------------------------------------------------------------------------
//    // Direct Program Uniforms (no glUseProgram)
//    //------------------------------------------------------------------------------
//    DEFINE_GL_WRAPPER(glProgramUniform1i)
//    DEFINE_GL_WRAPPER(glProgramUniform2i)
//    DEFINE_GL_WRAPPER(glProgramUniform3i)
//    DEFINE_GL_WRAPPER(glProgramUniform4i)
//    DEFINE_GL_WRAPPER(glProgramUniform1f)
//    DEFINE_GL_WRAPPER(glProgramUniform2f)
//    DEFINE_GL_WRAPPER(glProgramUniform3f)
//    DEFINE_GL_WRAPPER(glProgramUniform4f)
//    DEFINE_GL_WRAPPER(glProgramUniform1d)
//    DEFINE_GL_WRAPPER(glProgramUniformMatrix4fv)
//
//
//    //==============================================================================
//    // Drawing Commands
//    //==============================================================================
//    DEFINE_GL_WRAPPER(glDrawArrays)
//    DEFINE_GL_WRAPPER(glDrawElements)
//    DEFINE_GL_WRAPPER(glMultiDrawArrays)
//    DEFINE_GL_WRAPPER(glMultiDrawElements)
//    DEFINE_GL_WRAPPER(glDrawArraysInstanced)
//    DEFINE_GL_WRAPPER(glDrawElementsInstanced)
//    DEFINE_GL_WRAPPER(glDrawArraysIndirect)
//    DEFINE_GL_WRAPPER(glDrawElementsIndirect)
//    DEFINE_GL_WRAPPER(glMultiDrawArraysIndirect)
//    DEFINE_GL_WRAPPER(glMultiDrawElementsIndirect)
//
//
//    //==============================================================================
//    // Queries & Synchronization
//    //==============================================================================
//    DEFINE_GL_WRAPPER(glGenQueries)
//    DEFINE_GL_WRAPPER(glDeleteQueries)
//    DEFINE_GL_WRAPPER(glBeginQuery)
//    DEFINE_GL_WRAPPER(glEndQuery)
//    DEFINE_GL_WRAPPER(glGetQueryiv)
//    DEFINE_GL_WRAPPER(glGetQueryObjectiv)
//
//    DEFINE_GL_WRAPPER(glFenceSync)
//    DEFINE_GL_WRAPPER(glIsSync)
//    DEFINE_GL_WRAPPER(glDeleteSync)
//    DEFINE_GL_WRAPPER(glClientWaitSync)
//    DEFINE_GL_WRAPPER(glWaitSync)
//    DEFINE_GL_WRAPPER(glGetSynciv)
//
//    //==================================================================================================
//    // Compute & Memory Barriers
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glDispatchCompute)
//    DEFINE_GL_WRAPPER(glDispatchComputeGroupSizeARB)
//    DEFINE_GL_WRAPPER(glMemoryBarrier)
//    DEFINE_GL_WRAPPER(glMemoryBarrierByRegion)
//
//    //==================================================================================================
//    // Transform Feedback
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glGenTransformFeedbacks)
//    DEFINE_GL_WRAPPER(glCreateTransformFeedbacks)
//    DEFINE_GL_WRAPPER(glBindTransformFeedback)
//    DEFINE_GL_WRAPPER(glBeginTransformFeedback)
//    DEFINE_GL_WRAPPER(glEndTransformFeedback)
//    DEFINE_GL_WRAPPER(glPauseTransformFeedback)
//    DEFINE_GL_WRAPPER(glResumeTransformFeedback)
//    DEFINE_GL_WRAPPER(glTransformFeedbackBufferBase)
//    DEFINE_GL_WRAPPER(glTransformFeedbackBufferRange)
//
//    //==================================================================================================
//    // Debug & Labeling (KHR_debug / ARB_debug_output)
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glDebugMessageCallback)
//    DEFINE_GL_WRAPPER(glDebugMessageControl)
//    DEFINE_GL_WRAPPER(glGetDebugMessageLog)
//    DEFINE_GL_WRAPPER(glPushDebugGroup)
//    DEFINE_GL_WRAPPER(glPopDebugGroup)
//    DEFINE_GL_WRAPPER(glObjectLabel)
//    DEFINE_GL_WRAPPER(glGetObjectLabel)
//    DEFINE_GL_WRAPPER(glObjectPtrLabel)
//    DEFINE_GL_WRAPPER(glGetObjectPtrLabel)
//    DEFINE_GL_WRAPPER(glDebugMessageInsert)
//
//    //==================================================================================================
//    // Extensions & Miscellaneous
//    //==================================================================================================
//    DEFINE_GL_WRAPPER(glGetInternalformativ)
//    DEFINE_GL_WRAPPER(glProgramParameteri)
//    DEFINE_GL_WRAPPER(glGenProgramPipelines)
//    DEFINE_GL_WRAPPER(glUseProgramStages)
//    DEFINE_GL_WRAPPER(glBindProgramPipeline)
//    DEFINE_GL_WRAPPER(glGetAttribLocation)
//    DEFINE_GL_WRAPPER(glBindAttribLocation)
//    DEFINE_GL_WRAPPER(glProvokingVertex)
//    DEFINE_GL_WRAPPER(glPatchParameteri)
//    DEFINE_GL_WRAPPER(glFramebufferTextureLayer)
//    DEFINE_GL_WRAPPER(glGetBufferParameteriv)
//    DEFINE_GL_WRAPPER(glGetBufferParameteri64v)
//    DEFINE_GL_WRAPPER(glClearBufferfv)
//    DEFINE_GL_WRAPPER(glClearBufferiv)
//    DEFINE_GL_WRAPPER(glClearBufferuiv)
//    DEFINE_GL_WRAPPER(glClearBufferfi)
//    DEFINE_GL_WRAPPER(glClear)
//    DEFINE_GL_WRAPPER(glClearColor)
//    DEFINE_GL_WRAPPER(glFlush)
//    DEFINE_GL_WRAPPER(glTexPageCommitmentARB)
//    DEFINE_GL_WRAPPER(glGetTextureHandleARB)
//    DEFINE_GL_WRAPPER(glMakeTextureHandleResidentARB)
//    DEFINE_GL_WRAPPER(glMakeTextureHandleNonResidentARB)
}


#endif OPENGL_API