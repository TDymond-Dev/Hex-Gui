#ifndef GL_TEXTURE_HPP
#define GL_TEXTURE_HPP

#include "util/glapi.hpp"
#include <string>
#include <stdexcept>
#include <cstdint>

namespace gl {
    struct Texture2D {
        unsigned int id, bit;
        int w, h, c;
        Texture2D(int width, int height, int channel, std::vector<unsigned char> const& data) :w{ width }, h{ height }, c{ channel } {
			if (channel == 1)
                bit = GL_RED;
			else if (channel == 2) 
                bit = GL_RG;
			else if (channel == 3) 
                bit = GL_RGB;
			else if (channel == 4) 
                bit = GL_RGBA;
			else 
                throw std::runtime_error("Unsupported channel number for texture");
            
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, bit, width, height, 0, bit, GL_UNSIGNED_BYTE, data.data());
            //glGenerateMipmap(GL_TEXTURE_2D);
        }

        Texture2D(int w, int h, int c) :w{ w }, h{ h }, c{ c } {
            bit = (c == 1) ? GL_RED : (c == 2) ? GL_RG : (c == 3) ? GL_RGB : GL_RGBA;
            glGenTextures(1, &id);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT,
                NULL);
            glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        }

        ~Texture2D() { glDeleteTextures(1, &id); }

        void bind(int slot) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, id);
        }

        void update(std::vector<unsigned char> const& data, int x, int y, int w, int h) {
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
        }
    };

    struct ImageTexture {
        GLuint textureID;
        int ww, wh;
        GLenum internalFormat;
        int bindUnit = 0;

        ImageTexture(int w, int h, int bindUnit, GLenum internalFormat) :
            ww{ w }, wh{ h }, bindUnit(bindUnit), internalFormat{ internalFormat } {
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Allocate storage for the texture data
            glTextureStorage2D(textureID, 1, internalFormat, w, h);

            glBindImageTexture(bindUnit, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, internalFormat);
        }

        void clear() {
            auto [format, type] = getClearFormat(internalFormat);
            glClearTexSubImage(textureID, 0, 0, 0, 0, ww, wh, 1, format, type, nullptr);
        }

        void bind() {
            glBindTexture(GL_TEXTURE_2D, textureID);
            glBindTextureUnit(bindUnit, textureID);
        }

        ~ImageTexture() {
            glBindTexture(GL_TEXTURE_2D, 0);
            glDeleteTextures(1, &textureID);
        }

    private:
        std::pair<GLenum, GLenum> getClearFormat(GLenum internalFormat) {
            switch (internalFormat) {
            case GL_RGBA8:
                return { GL_RGBA, GL_UNSIGNED_BYTE };
            case GL_R8UI:
                return { GL_RED_INTEGER, GL_UNSIGNED_BYTE };
            default:
                // Handle other formats as needed
                return { GL_NONE, GL_NONE };
            }
        }
    };

    class SparseTextureArray {
    public:
        // Constructor parameters:
        // target         - The texture target (typically GL_TEXTURE_2D_ARRAY)
        // levels         - Number of mipmap levels (use 1 if you only need the base level)
        // internalFormat - The internal storage format (e.g., GL_RGBA8)
        // width, height  - Dimensions in texels of each layer
        // layers         - Number of layers in the texture array
        SparseTextureArray(GLenum target, GLint levels, GLenum internalFormat,
            GLsizei width, GLsizei height, GLsizei layers)
            : m_target(target),
            m_levels(levels),
            m_internalFormat(internalFormat),
            m_width(width),
            m_height(height),
            m_layers(layers)
        {
            glGenTextures(1, &m_textureID);
            if (m_textureID == 0) {
                throw std::runtime_error("Failed to generate texture object.");
            }
            glBindTexture(m_target, m_textureID);
            // Allocate immutable storage for the texture array.
            glTexStorage3D(m_target, m_levels, m_internalFormat, m_width, m_height, m_layers);
            // Setup default texture parameters.
            glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(m_target, 0);
        }

        ~SparseTextureArray() {
            if (m_textureID != 0) {
                glDeleteTextures(1, &m_textureID);
            }
        }

        // Retrieve the underlying texture object.
        GLuint getTextureID() const { return m_textureID; }

        // Commits (or decommits) a subregion (page) of the texture at a specific mip level.
        // Coordinates (xoffset,yoffset,layer) and dimensions (width,height,depth) are given in texels.
        void commitPage(int level, int xoffset, int yoffset, int layer,
            int width, int height, int depth, bool commit)
        {
            if (!glTexPageCommitmentARB) {
                throw std::runtime_error("glTexPageCommitmentARB function not loaded.");
            }
            glBindTexture(m_target, m_textureID);
            glTexPageCommitmentARB(m_target, level, xoffset, yoffset, layer,
                width, height, depth, commit ? GL_TRUE : GL_FALSE);
            glBindTexture(m_target, 0);
        }

        // Queries the virtual page size (in texels) for this texture’s internal format.
        // The results are stored in pageSizeX, pageSizeY, and pageSizeZ.
        void getVirtualPageSize(int& pageSizeX, int& pageSizeY, int& pageSizeZ)
        {
            if (!glGetInternalformativ) {
                throw std::runtime_error("glGetInternalformativ function not loaded.");
            }
            glBindTexture(m_target, m_textureID);
            glGetInternalformativ(m_target, m_internalFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &pageSizeX);
            glGetInternalformativ(m_target, m_internalFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &pageSizeY);
            glGetInternalformativ(m_target, m_internalFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &pageSizeZ);
            glBindTexture(m_target, 0);
        }

        // Binds the texture to a specified texture unit using glBindTextureUnit.
        // This is available in OpenGL 4.5 core and eliminates the need to repeatedly call glActiveTexture/glBindTexture.
        void bindToUnit(GLuint unit) const
        {
            if (unit >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
                throw std::out_of_range("Texture unit exceeds maximum combined texture image units.");
            }
            // OpenGL 4.5 provides glBindTextureUnit in core; no extension loading is needed.
            glBindTextureUnit(unit, m_textureID);
        }

    private:
        GLuint m_textureID;
        GLenum m_target;
        GLint m_levels;
        GLenum m_internalFormat;
        GLsizei m_width, m_height, m_layers;
    };
} // namespace gl::detail

namespace gl {
        //using Texture1D = detail::Texture<GL_TEXTURE_1D>;
    //using Texture2D = detail::Texture<GL_TEXTURE_2D>;
    //using Texture3D = detail::Texture<GL_TEXTURE_3D>;
    //using TextureCube = detail::Texture<GL_TEXTURE_CUBE_MAP>;
    //using Texture2DArray = detail::Texture<GL_TEXTURE_2D_ARRAY>;
    //using TextureCubeArray = detail::Texture<GL_TEXTURE_CUBE_MAP_ARRAY>;
    //using TextureRectangle = detail::Texture<GL_TEXTURE_RECTANGLE>;
    //using TextureBuffer = detail::Texture<GL_TEXTURE_BUFFER>;
    //using Texture2DMultisample = detail::Texture<GL_TEXTURE_2D_MULTISAMPLE>;
    //using Texture2DMSArray = detail::Texture<GL_TEXTURE_2D_MULTISAMPLE_ARRAY>;
}

namespace gl {
    //// 1. Create an 8k×8k sparse texture with one mip level,
    //  //  RGBA8 format, 256×256 tiles, and a debug label:
    //SparseTexture2D<GL_TEXTURE_2D> sparseTex(
    //    8192, 8192,        // full virtual size
    //    1,                 // mip levels
    //    GL_RGBA8,          // internal format
    //    256, 256,          // tile size
    //    "MegaSparseTex"    // debug label
    //);
    //
    //// 2. Commit a 3×3 tile region around the center tile
    //int centerX = (8192 / 256) / 2;
    //int centerY = (8192 / 256) / 2;
    //sparseTex.commitRegion(0, centerX - 1, centerY - 1,
    //    centerX + 1, centerY + 1);
    //
    //// 3. Upload texels into one of those tiles (via PBO or directly)
    //std::vector<uint8_t> pixelData(256 * 256 * 4, 255); // white RGBA
    //fn_glTextureSubImage2D(
    //    sparseTex.tex.id(), 0,
    //    (centerX - 1) * 256, (centerY - 1) * 256,
    //    256, 256,
    //    GL_RGBA, GL_UNSIGNED_BYTE,
    //    pixelData.data()
    //);
    //
    //// 4. Bind and sample in your shader
    //sparseTex.bind(0);

    //template<GLenum Target>
    //struct SparseTexture2D {
    //    gl::detail::Texture<Target>     tex;          // underlying Texture object
    //    GLsizei                 width, height;
    //    GLsizei                 tileW, tileH;
    //    int                      tilesX, tilesY;
    //    std::vector<bool>        resident;     // one flag per tile

    //    // Constructor: allocates storage and initializes tracking
    //    SparseTexture2D(GLsizei w, GLsizei h, GLint levels,
    //        GLenum internalFormat, GLsizei tW, GLsizei tH,
    //        const std::string& debugLabel = "")
    //        : tex(debugLabel), width(w), height(h), tileW(tW), tileH(tH)
    //    {
    //        // Allocate immutable sparse storage
    //        tex.storage(levels, internalFormat, width, height);

    //        // Compute tile grid dimensions
    //        tilesX = (width + tileW - 1) / tileW;
    //        tilesY = (height + tileH - 1) / tileH;
    //        resident.assign(tilesX * tilesY, false);
    //    }

    //    // Commit a single tile at (tx, ty) on LOD 'level'
    //    void commitPage(GLint level, int tx, int ty) {
    //        int idx = tx + ty * tilesX;
    //        if (idx < 0 || idx >= (int)resident.size())
    //            throw std::out_of_range("tile coordinate out of range");

    //        if (!resident[idx]) {
    //            fn_glTexPageCommitmentARB(
    //                Target, level,
    //                tx * tileW, ty * tileH, 0,
    //                tileW, tileH, 1,
    //                GL_TRUE
    //            );
    //            resident[idx] = true;
    //        }
    //    }

    //    // Decommit (free) a single tile
    //    void decommitPage(GLint level, int tx, int ty) {
    //        int idx = tx + ty * tilesX;
    //        if (idx < 0 || idx >= (int)resident.size())
    //            throw std::out_of_range("tile coordinate out of range");

    //        if (resident[idx]) {
    //            fn_glTexPageCommitmentARB(
    //                Target, level,
    //                tx * tileW, ty * tileH, 0,
    //                tileW, tileH, 1,
    //                GL_FALSE
    //            );
    //            resident[idx] = false;
    //        }
    //    }

    //    // Convenience: commit a rectangular block of tiles
    //    void commitRegion(GLint level, int tX0, int tY0, int tX1, int tY1) {
    //        for (int y = tY0; y <= tY1; ++y)
    //            for (int x = tX0; x <= tX1; ++x)
    //                commitPage(level, x, y);
    //    }

    //    // Bind underlying texture to unit
    //    void bind(GLuint unit) const {
    //        tex.bindUnit(unit);
    //    }
    //};
}

#endif // GL_TEXTURE_HPP
