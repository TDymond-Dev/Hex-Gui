#ifndef RENDER_DRAW_OBJECT
#define RENDER_DRAW_OBJECT

#include "opengl/buffer.hpp"
#include "engine/common/eml/vector.hpp"

namespace engine::render {
	struct Mesh {
		struct Vertex {
			eml::vec2f position;
		};

		std::vector<Vertex>       vertices;
		std::vector<uint32_t>     indices;
	};

	std::vector<Mesh> gMeshes;

	struct DrawElementsIndirectCommand
	{
		uint32_t count;         // index count
		uint32_t instanceCount; // usually 1 (or more if true instancing)
		uint32_t firstIndex;    // starting index within index buffer (in indices)
		int32_t  baseVertex;    // added to vertex index
		uint32_t baseInstance;  // becomes gl_BaseInstance
	};

	template<typename data_t>
	class DrawObject {
		gl::vbuffer<data_t> vbuffer_;
		gl::ebuffer ebuffer_;

		std::vector<DrawElementsIndirectCommand> draw_commands_;

		uint32_t index_offset_ = 0;
		uint32_t vertex_offset_ = 0;
	public:
		void add_mesh(const std::vector<data_t>& vertices, const std::vector<uint32_t>& indices, uint32_t instance_count = 1) {
			// Upload vertex data
			vbuffer_.append(vertices);

			// Upload index data
			ebuffer_.append(indices);

			DrawElementsIndirectCommand cmd{};
			cmd.count = static_cast<uint32_t>(indices.size());
			cmd.instanceCount = instance_count;
			cmd.firstIndex = index_offset_;
			cmd.baseVertex = static_cast<int32_t>(vertex_offset_);
			cmd.baseInstance = 0;

			draw_commands_.push_back(cmd);

			index_offset_ += static_cast<uint32_t>(indices.size());
			vertex_offset_ += static_cast<uint32_t>(vertices.size());
		}

		void draw(GLenum mode) {
			vbuffer_.bind();
			ebuffer_.bind();
			glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, draw_commands_.data(), draw_commands_.size(), 0);
		}

		void reserve(size_t vertex_count, size_t index_count) {
			vbuffer_.reserve(vertex_count);
			ebuffer_.reserve(index_count);
		}

		void clear() {
			vbuffer_.clear();
			ebuffer_.clear();
			draw_commands_.clear();
			index_offset_ = 0;
			vertex_offset_ = 0;
		}
	};
}

#endif // ! RENDER_DRAW_OBJECT