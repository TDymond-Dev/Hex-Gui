#ifndef OPENGL_DEBUG
#define OPENGL_DEBUG

#include "gldef.h"

#include <functional>
#include <string>
#include <mutex>
#include <map>

/*
Automate wrapper generation so every new GL entry point is covered
*/

namespace gl {
    struct ObjectRegistry {
        using Key = std::pair<GLenum, GLuint>;
        std::mutex                mu;
        std::map<Key, std::string> labels;

        static ObjectRegistry& Instance() {
            static ObjectRegistry inst;
            return inst;
        }

        void Store(GLenum identifier, GLuint name, const std::string& label) {
            std::lock_guard<std::mutex> lk(mu);
            labels[{identifier, name}] = label;
        }

        bool Query(GLenum identifier, GLuint name, std::string& outLabel) {
            std::lock_guard<std::mutex> lk(mu);
            auto it = labels.find({ identifier,name });
            if (it == labels.end()) return false;
            outLabel = it->second;
            return true;
        }
    };

    // Central error-check helper
    inline void CheckGLError(const char* funcName) {
        GLenum err = glGetError();
        //if (err != GL_NO_ERROR) {
        //    throw std::runtime_error(
        //        std::string("OpenGL Error in ") + funcName +
        //        ": 0x" + std::to_string(err)
        //    );
        //}
    }

    /*
        GLCall("glGenBuffers", glGenBuffers, 1, &m_id);
        GLCall("glBindBuffer", glBindBuffer, GL_ARRAY_BUFFER, m_id);
    */
    template<typename Fn, typename... Args>
    auto GLCall(const char* funcName, Fn&& fn, Args&&... args) -> decltype((fn)(std::forward<Args>(args)...))
    {
        using ReturnType = decltype((fn)(std::forward<Args>(args)...));

        if constexpr (std::is_same_v< ReturnType, void>) {
            std::forward<Fn>(fn)(std::forward<Args>(args)...);
            CheckGLError(funcName);

            return;
        }
        else {
            auto result = std::forward<Fn>(fn)(std::forward<Args>(args)...);
            return result;
        }
    }

    /*
    class Buffer {
    public:
      Buffer(GLsizeiptr size, const void* data) {
        GLObjectLabel(GL_BUFFER, m_id, "VertexBuffer: MyMesh"); // Annotate in GPU debugger
      }

      ... rest of class
    };
    */
    class GLObjectLabel {
    public:
        GLObjectLabel(GLenum identifier, GLuint name, const std::string& label) {
            // 1) call the core labeling function
            glObjectLabel(identifier, name,
                static_cast<GLsizei>(label.size()),
                label.c_str());

            // 2) store in our registry
            ObjectRegistry::Instance().Store(identifier, name, label);

            // 3) emit a GL marker so the debug?callback sees “I just labeled object X”
            glDebugMessageInsert(
                GL_DEBUG_SOURCE_APPLICATION,
                GL_DEBUG_TYPE_MARKER,
                name,                                      // use the object name as the marker ID
                GL_DEBUG_SEVERITY_NOTIFICATION,
                static_cast<GLsizei>(label.size()),
                label.c_str()
            );
        }

        static std::string GetObjectLabel(GLenum identifier, GLuint name)
        {
            if (name == 0)
                return "<null>";

            constexpr GLsizei MAX_LEN = 256;
            char buffer[MAX_LEN];
            GLsizei length = 0;
            glGetObjectLabel(identifier, name, MAX_LEN, &length, buffer);

            return std::string(buffer, length);
        }
    };


    /*
    {
        GLDebugGroup frameScope("RenderFrame");
        // All GL calls here are tagged under "RenderFrame"
        // …
    }
    */
    class GLDebugGroup
    {
    public:
        explicit GLDebugGroup(const std::string& name) : m_name(name) {
            // push into OpenGL
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)name.size(), name.c_str());

            // maintain our own stack
            getStack().push_back(name);
        }

        ~GLDebugGroup() {
            glPopDebugGroup();
            getStack().pop_back();
        }

        // Expose current full stack
        static const std::vector<std::string>& CurrentStack() {
            return getStack();
        }

    private:
        std::string m_name;

        static std::vector<std::string>& getStack()
        {
            static thread_local std::vector<std::string> stack;
            return stack;
        }
    };
    
    static inline std::function<void(std::string const&)> onDebug = [](std::string const&) { std::cout << "temp\n";};

    class GLDebugContext {
    public:
        GLDebugContext() {
            // Must be a debug-enabled context (via GLFW/SDL hint)
            glEnable(GL_DEBUG_OUTPUT);                              // turn on debug output
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);                   // call callback in the same call stack
            glDebugMessageCallback(&GLDebugContext::glDebugOutput, this);

            // Optionally filter out notifications if too verbose
            //glDebugMessageControl(
            //    GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
            //    0, nullptr, GL_FALSE
            //);
        }

        ~GLDebugContext() {
            glDisable(GL_DEBUG_OUTPUT);
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(nullptr, nullptr);
        }


    private:
        // static callback invoked by driver on errors, performance warnings, etc.
        static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
        {
            auto* self = static_cast<GLDebugContext*>(const_cast<void*>(userParam));

            // 1) Build enriched report
            std::string enriched;

            // 1a) prepend the debug?group stack
            const auto& stack = GLDebugGroup::CurrentStack();
            if (!stack.empty()) {
                enriched += "Scope: ";
                for (size_t i = 0; i < stack.size(); ++i) {
                    enriched += stack[i];
                    if (i + 1 < stack.size()) enriched += " -> ";
                }
                enriched += "\n";
            }

            // 1b) if this is our own object?label marker, fetch the human name
            if (type == GL_DEBUG_TYPE_MARKER &&
                source == GL_DEBUG_SOURCE_APPLICATION)
            {
                std::string label;
                // the id we used in glDebugMessageInsert() was the object name
                // unfortunately the callback doesn't hand us the GLenum identifier,
                // so you can either encode it in 'id' or assume one type per range.
                // For demo, let's try querying across all identifiers:
                for (GLenum ident : {
                    GL_BUFFER, GL_TEXTURE, GL_VERTEX_ARRAY,
                        GL_SHADER, GL_PROGRAM, /*etc…*/ })
                {
                    if (ObjectRegistry::Instance().Query(ident, id, label)) {
                        enriched += "? Labeled object: [" + label + "]\n";
                        break;
                    }
                }
                // fall through—optionally skip printing the raw message
            }

            // 1c) then the usual report body

            // ignore non-significant error/warning codes
            if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
                return;

            enriched += "------------------\n";
            enriched += "Debug message (" + std::to_string(id) + "): " + message + "\n";
            enriched += "------------------\n";

            switch (source)
            {
            case GL_DEBUG_SOURCE_API:             enriched += "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   enriched += "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: enriched += "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     enriched += "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     enriched += "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           enriched += "Source: Other"; break;
            }enriched += "\n";

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:               enriched += "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: enriched += "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  enriched += "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         enriched += "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         enriched += "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              enriched += "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          enriched += "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           enriched += "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               enriched += "Type: Other"; break;
            }enriched += "\n";;

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:         enriched += "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       enriched += "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          enriched += "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: enriched += "Severity: notification"; break;
            }

            enriched += "\n";
            // Call the user-defined callback with the report
            onDebug(enriched);
        }
    };
}

#endif OPENGL_DEBUG