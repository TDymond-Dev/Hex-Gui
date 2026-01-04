#ifndef GL_FRAMEBUFFER2
#define GL_FRAMEBUFFER2

#include "util/gldef.h"

#include <string>
#include <unordered_map>

namespace gl {
    struct FrameBufferData {
        std::string name;
        GLenum      internalFormat;
        GLenum      format;
        GLenum      type;
    };


    class Framebuffer {
    public:
        Framebuffer(int w, int h) : _w(w), _h(h) {
            glCreateFramebuffers(1, &_fbo);
        }

        Framebuffer(Framebuffer&& o) noexcept :
            _fbo(o._fbo), _w(o._w), _h(o._h),
            _colorMap(std::move(o._colorMap)),
            _rboMap(std::move(o._rboMap))
        {
            o._fbo = 0;
        }

        ~Framebuffer() {
            if (_fbo)
                glDeleteFramebuffers(1, &_fbo);
            for (auto& [_, tex] : _colorMap)
                glDeleteTextures(1, &tex);
            for (auto& [_, rbo] : _rboMap)
                glDeleteRenderbuffers(1, &rbo);
        }

        void addColor(std::string const& name, GLenum ifmt, GLenum fmt, GLenum type) {
            GLuint tex;
            glCreateTextures(GL_TEXTURE_2D, 1, &tex);
            glTextureStorage2D(tex, 1, ifmt, _w, _h);
            glTexParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            GLuint slot = (GLuint)_colorMap.size();
            glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0 + slot, tex, 0);
            _colorMap[name] = tex;
        }

        void addDepthStencil(std::string const& name) {
            GLuint rbo;
            glCreateRenderbuffers(1, &rbo);
            glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, _w, _h);
            glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_STENCIL_ATTACHMENT,
                GL_RENDERBUFFER, rbo);
            _rboMap[name] = rbo;
        }

        void finalize() {
            std::vector<GLenum> draws(_colorMap.size());
            for (size_t i = 0; i < draws.size(); ++i)
                draws[i] = GL_COLOR_ATTACHMENT0 + (GLenum)i;
            glNamedFramebufferDrawBuffers(_fbo, (GLsizei)draws.size(), draws.data());

            auto status = glCheckNamedFramebufferStatus(_fbo, GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Incomplete FBO: " + std::to_string(status));
        }

        void bind()   const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); }
        void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        GLuint getColor(std::string const& name) const {
            return _colorMap.at(name);
        }

        auto getMaps() {
            std::map<std::string, GLuint> textures;

			for (auto& [name, tex] : _colorMap)
				textures[name] = tex;

			for (auto& [name, rbo] : _rboMap)
				textures[name] = rbo;

            return textures;
        }

    private:
        GLuint _fbo = 0;
        int    _w, _h;
        std::unordered_map<std::string, GLuint> _colorMap, _rboMap;
    };
}

#endif  GL_FRAMEBUFFER2