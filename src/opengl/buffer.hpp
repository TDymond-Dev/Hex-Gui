#ifndef GL_BBUFFER
#define GL_BBUFFER

#include "util/gldef.h"
#include <vector>

namespace gl::detail {
	template<typename T, GLenum type>
	struct Buffer {
		Buffer() = default;
		Buffer(std::vector<T> const& data, GLenum flags = GL_DYNAMIC_DRAW) {
			glGenBuffers(1, &mID);
			bind();
			glNamedBufferData(mID, sizeof(T) * data.size(), data.data(), flags);
			mSize = (unsigned int)data.size();
		}

		~Buffer() {
			glDeleteBuffers(1, &mID);
		}

		void bind() {
			glBindBuffer(type, mID);
		}

		void unbind() {
			glBindBuffer(type, 0);
		}

		void bind_base(int index = 0) {
			if (type == GL_SHADER_STORAGE_BUFFER) {
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mID);
			}
			else {
				std::cerr << "Error: bind_base called on non-SSBO buffer" << std::endl;
			}
		}

		void map_to(std::vector<T> const& data, GLbitfield flags = GL_WRITE_ONLY) {
			void* ptr = glMapNamedBuffer(mID, flags);

			if (ptr == nullptr) {
				throw std::runtime_error("Failed to map buffer");
			}
			else {
				// Copy the data
				memcpy(ptr, data.data(), data.size() * sizeof(T));
				glUnmapNamedBuffer(mID);
			}
		}

		void map_from(std::vector<T>& output, GLbitfield flags = GL_MAP_READ_BIT) {
			T* ptr = (T*)glMapNamedBufferRange(mID, 0, mSize * sizeof(T), flags);

			memcpy(&output[0], ptr, output.size() * sizeof(T));
			glUnmapNamedBuffer(mID);
		}

		void update(std::vector<T> const& data, GLbitfield flags = GL_WRITE_ONLY) {
			glNamedBufferData(mID, sizeof(T) * data.size(), data.data(), flags);
		}

		void update_sub(std::vector<T> const& data, int offset = 0) {
			glNamedBufferSubData(mID, offset, sizeof(T) * data.size(), data.data());
		}

		GLuint size() const {
			return mSize;
		}
		GLuint id() const {
			return mID;
		}

	private:
		GLuint mID, mSize = 0;
	};
}

namespace gl {
	template<typename T> using vbuffer = detail::Buffer<T, GL_ARRAY_BUFFER>;
	template<typename T> using ebuffer = detail::Buffer<T, GL_ELEMENT_ARRAY_BUFFER>;
	template<typename T> using ubuffer = detail::Buffer<T, GL_UNIFORM_BUFFER>;
	template<typename T> using sbuffer = detail::Buffer<T, GL_SHADER_STORAGE_BUFFER>;
	template<typename T> using tbuffer = detail::Buffer<T, GL_TEXTURE_BUFFER>;
	template<typename T> using pbuffer = detail::Buffer<T, GL_PIXEL_PACK_BUFFER>;
	template<typename T> using rbuffer = detail::Buffer<T, GL_COPY_READ_BUFFER>;
	template<typename T> using wbuffer = detail::Buffer<T, GL_COPY_WRITE_BUFFER>;
	template<typename T> using dbuffer = detail::Buffer<T, GL_DISPATCH_INDIRECT_BUFFER>;
	template<typename T> using abuffer = detail::Buffer<T, GL_ATOMIC_COUNTER_BUFFER>;
}

#endif GL_BBUFFER