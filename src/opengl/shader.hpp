#ifndef GL_SHADER2
#define GL_SHADER2

#include "util/gldef.h"

#include <string>
#include <map>

namespace gl::detail {
	class Shader {
		GLuint   _id = 0;
		std::string _source;
	public:
		Shader(GLenum type, const std::string& source) : _source(source)
		{
			_id = glCreateShader(type);
			const char* src = _source.c_str();
			glShaderSource(_id, 1, &src, nullptr);
			glCompileShader(_id);
			GLint success;
			glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[512];
				glGetShaderInfoLog(_id, 512, nullptr, infoLog);
				std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		}

		~Shader() {
			if (_id != 0) {
				glDeleteShader(_id);
			}
		}

		GLuint id() const { return _id; }
		std::string source() const { return _source; }
	};

	class Program {
		std::map<GLenum, std::unique_ptr<Shader>> _shaders;
		GLuint _id = 0;
	public:
		Program() {}

		Program(std::map<GLenum, std::string> const& shaderSources) {
			build(shaderSources);
		}

		~Program() {
			if (_id != 0) {
				glDeleteProgram(_id);
			}
		}

		void build(std::map<GLenum, std::string> const& shaderSources) {
			_id = glCreateProgram();

			for (const auto& [type, source] : shaderSources) {
				attachShader(type, source);
			}

			link();
		}

		GLuint id() const {
			return _id;
		}

		void use() const {
			glUseProgram(id());
		}

		void SetUniform(const std::string& name, int value) const {
			glUniform1i(_getUniformLocation(name), value);
		}

		void SetUniform(const std::string& name, int v0, int v1) const {
			glUniform2i(_getUniformLocation(name), v0, v1);
		}

		void SetUniform(const std::string& name, int v0, int v1, int v2) const {
			glUniform3i(_getUniformLocation(name), v0, v1, v2);
		}

		void SetUniform(const std::string& name, float value) const {
			glUniform1f(_getUniformLocation(name), value);
		}

		void SetUniform(const std::string& name, float v0, float v1) const {
			glUniform2f(_getUniformLocation(name), v0, v1);
		}

		void SetUniform(const std::string& name, float v0, float v1, float v2) const {
			glUniform3f(_getUniformLocation(name), v0, v1, v2);
		}

		void SetUniform(const std::string& name, float v0, float v1, float v2, float v3) const {
			glUniform4f(_getUniformLocation(name), v0, v1, v2, v3);
		}

		void SetUniform(const std::string& name, const float* matrix, bool transpose = false) const {
			glUniformMatrix4fv(_getUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, matrix);
		}

		void SetUniformBuffer(const std::string& name, GLuint bindingPoint) const {
			GLuint blockIndex = glGetUniformBlockIndex(id(), name.c_str());
			glUniformBlockBinding(id(), blockIndex, bindingPoint);
		}

		void SetShaderStorageBuffer(const std::string& name, GLuint bindingPoint, GLuint bufferID) const {
			GLuint blockIndex = glGetProgramResourceIndex(id(), GL_SHADER_STORAGE_BLOCK, name.c_str());
			glShaderStorageBlockBinding(id(), blockIndex, bindingPoint);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
		}

		void SetTextureUnit(const std::string& name, int unit, GLuint textureID) const {
			glActiveTexture(GL_TEXTURE0 + unit); // activate texture unit 0
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(_getUniformLocation(name), unit);
		}

		void SetTextureArrayUnit(const std::string& name, int unit, GLuint textureID) const {
			glActiveTexture(GL_TEXTURE0 + unit); // activate texture unit 0
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
			glUniform1i(_getUniformLocation(name), unit);
		}

	private:
		GLuint _getUniformLocation(const std::string& name) const {
			int uid = glGetUniformLocation(id(), name.c_str());
			if (uid == -1)
				std::cerr << "Warning: uniform '" << name << "' not found or unused in shader program." << std::endl;
			return uid;
		}

		void attachShader(GLenum type, const std::string& source) {
			auto shader = std::make_unique<Shader>(type, source);
			glAttachShader(_id, shader->id());
			_shaders[type] = std::move(shader);
		}

		void link() {
			glLinkProgram(_id);
			GLint success;
			glGetProgramiv(_id, GL_LINK_STATUS, &success);
			if (!success) {
				char infoLog[512];
				glGetProgramInfoLog(_id, 512, nullptr, infoLog);
				std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
		}
	};
}

namespace gl {
	using Shader = detail::Shader;
	using Program = detail::Program;
};

#endif // !GL_SHADER2