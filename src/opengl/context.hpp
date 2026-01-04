#ifndef GL_CONTEXT
#define GL_CONTEXT

#include <vector>
#include <iostream>
#include "util/glapi.hpp"

namespace gl {

    /*
        Demo

        gl::load_extensions();
        gl::SetDebugMode(true);

        gl::onDebug = [](std::string const& report) {
            std::cout << report << std::endl;
        };

        gl::GLDebugContext debugContext;
        gl::GLDebugGroup group("Main Group");
    */

    // -------------------------------
    // Basic OpenGL Context
    // -------------------------------
    struct Context {
        HGLRC rc = nullptr;
        bool initialized = false;

        Context() = default;

        explicit Context(HDC hdc) {
            rc = wglCreateContext(hdc);
            if (!rc || !wglMakeCurrent(hdc, rc)) {
                initialized = false;
            }
            else {
                initialized = true;
            }
        }

        void erase() {
            wglMakeCurrent(nullptr, nullptr);
            if (rc) wglDeleteContext(rc);
            rc = nullptr;
            initialized = false;
        }

        void makeCurrent(HDC hdc) {
            if (rc) wglMakeCurrent(hdc, rc);
        }
    };

    // -------------------------------
    // Modern OpenGL Context
    // -------------------------------
    //struct ModernContext {
    //    HGLRC rc = nullptr;
    //    bool initialized = false;
    //
    //    ModernContext() = default;
    //
    //    ModernContext(HDC hdc, int major = 4, int minor = 6, bool use_core = true) {
    //        initialized = load(hdc, major, minor, use_core);
    //    }
    //
    //    ~ModernContext() {
    //        wglMakeCurrent(nullptr, nullptr);
    //        if (rc) wglDeleteContext(rc);
    //    }
    //
    //    void makeCurrent(HDC hdc) {
    //        if (rc) wglMakeCurrent(hdc, rc);
    //    }
    //
    //    bool load(HDC hdc, int major, int minor, bool use_core) {
    //        const int attribs[] = {
    //            WGL_CONTEXT_MAJOR_VERSION_ARB, major,
    //            WGL_CONTEXT_MINOR_VERSION_ARB, minor,
    //            WGL_CONTEXT_LAYER_PLANE_ARB, 0,
    //            WGL_CONTEXT_PROFILE_MASK_ARB, use_core ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    //            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
    //            0
    //        };
    //
    //        rc = wglCreateContextAttribsARB(hdc, nullptr, attribs);
    //        if (!rc || !wglMakeCurrent(hdc, rc)) return false;
    //
    //        return true;
    //    }
    //
    //    operator bool() const { return initialized; }
    //    operator HGLRC() const { return rc; }
    //};
    //
    //// -------------------------------
    //// Pixel Format Helpers
    //// -------------------------------
    //namespace detail {
    //    inline bool setPixelFormat(HDC hdc, int pixelFormat) {
    //        PIXELFORMATDESCRIPTOR pfd{};
    //        DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd);
    //        return SetPixelFormat(hdc, pixelFormat, &pfd);
    //    }
    //}
    //
    //struct PixelFormatARB {
    //    PixelFormatARB() = default;
    //
    //    explicit PixelFormatARB(HDC hdc) { set(hdc); }
    //
    //    void set(HDC hdc) {
    //        int pixelFormat;
    //        UINT numFormats;
    //        const int attribs[] = {
    //            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    //            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    //            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    //            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    //            WGL_COLOR_BITS_ARB, 32,
    //            WGL_DEPTH_BITS_ARB, 24,
    //            WGL_STENCIL_BITS_ARB, 8,
    //            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
    //            WGL_SAMPLES_ARB, 1,
    //            0
    //        };
    //
    //        if (!wglChoosePixelFormatARB(hdc, attribs, nullptr, 1, &pixelFormat, &numFormats) ||
    //            !detail::setPixelFormat(hdc, pixelFormat)) {
    //            MessageBox(nullptr, L"Failed to set ARB pixel format", L"Pixel Format Error", MB_ICONEXCLAMATION | MB_OK);
    //        }
    //    }
    //};

   
}

#endif // GL_CONTEXT
