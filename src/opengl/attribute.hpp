#ifndef GL_ATTRIBUTE
#define GL_ATTRIBUTE

#include "util/gldef.h"

#include <vector>

namespace gl {
	enum glFlag {
		Int1, Int2, Int3, Int4,
		Float1, Float2, Float3, Float4
	};

	template<typename T>
	void set_attributes(std::vector<gl::glFlag> const& flags) {
		float offset = 0;
		auto get_offset = [](gl::glFlag f) {
			if (f == gl::Float1) return std::make_tuple(1, sizeof(float) * 1, GL_FLOAT);
			if (f == gl::Float2) return std::make_tuple(2, sizeof(float) * 2, GL_FLOAT);
			if (f == gl::Float3) return std::make_tuple(3, sizeof(float) * 3, GL_FLOAT);
			if (f == gl::Float4) return std::make_tuple(4, sizeof(float) * 4, GL_FLOAT);
			if (f == gl::Int1) return std::make_tuple(1, sizeof(int) * 1, GL_INT);
			if (f == gl::Int2) return std::make_tuple(2, sizeof(int) * 2, GL_INT);
			if (f == gl::Int3) return std::make_tuple(3, sizeof(int) * 3, GL_INT);
			if (f == gl::Int4) return std::make_tuple(4, sizeof(int) * 4, GL_INT);
			};

		int total_offset = 0, index = 0;
		for (auto f : flags) {
			auto [count, offset, type] = get_offset(f);
			glVertexAttribPointer(index, count, type, GL_FALSE, sizeof(T), (void*)(total_offset));
			glEnableVertexAttribArray(index);
			index++, total_offset += offset;
		}
	}

    class VertexArray {
        GLuint vao_ = 0;
        GLsizei stride_ = 0;

        static constexpr GLsizei size_of(glFlag flag) {
            switch (flag) {
            case glFlag::Int1:   return sizeof(int) * 1;
            case glFlag::Int2:   return sizeof(int) * 2;
            case glFlag::Int3:   return sizeof(int) * 3;
            case glFlag::Int4:   return sizeof(int) * 4;
            case glFlag::Float1: return sizeof(float) * 1;
            case glFlag::Float2: return sizeof(float) * 2;
            case glFlag::Float3: return sizeof(float) * 3;
            case glFlag::Float4: return sizeof(float) * 4;
            }
            return 0;
        }

        static constexpr GLint component_count(glFlag flag) {
            switch (flag) {
            case glFlag::Int1:
            case glFlag::Float1: return 1;
            case glFlag::Int2:
            case glFlag::Float2: return 2;
            case glFlag::Int3:
            case glFlag::Float3: return 3;
            case glFlag::Int4:
            case glFlag::Float4: return 4;
            }
            return 0;
        }

        static constexpr GLenum component_type(glFlag flag) {
            switch (flag) {
            case glFlag::Int1:
            case glFlag::Int2:
            case glFlag::Int3:
            case glFlag::Int4:
                return GL_INT;

            case glFlag::Float1:
            case glFlag::Float2:
            case glFlag::Float3:
            case glFlag::Float4:
                return GL_FLOAT;
            }
            return GL_FLOAT;
        }

        static constexpr bool is_integer(glFlag flag) {
            return component_type(flag) == GL_INT;
        }

    public:
        VertexArray() {
            glCreateVertexArrays(1, &vao_);
        }

        ~VertexArray() {
            glDeleteVertexArrays(1, &vao_);
        }

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        // ------------------------------------------------------------
        // Layout setup
        // ------------------------------------------------------------

        void set_layout(const std::vector<glFlag>& layout) {
            stride_ = 0;
            for (auto flag : layout)
                stride_ += size_of(flag);

            GLsizei offset = 0;

            for (GLuint index = 0; index < layout.size(); ++index) {
                const auto flag = layout[index];

                glEnableVertexArrayAttrib(vao_, index);

                if (is_integer(flag)) {
                    glVertexArrayAttribIFormat( vao_,index,component_count(flag), component_type(flag),offset);
                }
                else {
                    glVertexArrayAttribFormat(vao_,index,component_count(flag),component_type(flag),GL_FALSE,offset );
                }

                glVertexArrayAttribBinding(vao_, index, 0);
                offset += size_of(flag);
            }
        }

        // ------------------------------------------------------------
        // Buffer binding
        // ------------------------------------------------------------

        void bind_vertex_buffer(GLuint buffer, GLintptr offset = 0) const {
            glVertexArrayVertexBuffer( vao_,0,buffer,offset,stride_);
        }

        void bind() const {
            glBindVertexArray(vao_);
        }

        GLuint id() const { return vao_; }
    };
}

#endif GL_ATTRIBUTE