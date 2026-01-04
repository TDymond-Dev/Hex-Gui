#ifndef GL_OBJECT
#define GL_OBJECT

#include "glapi.hpp"

namespace gl {
	/*
	Textures
	Buffers
	Shaders
	Programs
	*/
    class GlObject {
    protected:
        GLuint mID = 0;
        GLenum mType = 0; // GL_BUFFER, GL_TEXTURE, etc.

    public:
        GlObject() = default;
        GlObject(GLenum type, std::string const& label) : mType(type) {
            setLabel(label);
        }

        virtual ~GlObject() = default;

        GLuint id() const { return mID; }
        GLenum type() const { return mType; }

        void setLabel(const std::string& label) const {
            if (mID && mType) {
                glObjectLabel(mType, mID, (GLsizei)label.size(), label.c_str());
            }
        }

        std::string getLabel() const {
            if (mID && mType) {
                char buf[256] = {};
                glGetObjectLabel(mType, mID,sizeof(buf), nullptr, buf);
                return buf;
            }
            return "<unlabeled>";
        }

        // Optional: for debug printing
        void debugPrint() const {
            std::string label = getLabel();
            std::cout << "GLObject [" << label << "] (Type: 0x" << std::hex << mType  << ", ID: " << mID << ")" << std::dec << std::endl;
        }

        // Optional: move semantics
        GlObject(GlObject&& other) noexcept : mID(other.mID), mType(other.mType) {
            other.mID = 0;
            other.mType = 0;
        }

        GlObject& operator=(GlObject&& other) noexcept {
            if (this != &other) {
                mID = other.mID;
                mType = other.mType;
                other.mID = 0;
                other.mType = 0;
            }
            return *this;
        }

        // Prevent accidental copies
        GlObject(const GlObject&) = delete;
        GlObject& operator=(const GlObject&) = delete;
    };
}


#endif GL_OBJECT