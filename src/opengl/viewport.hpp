#ifndef GL_VIEWPORT
#define GL_VIEWPORT

#include "util/gldef.h"

namespace gl {
	struct Viewport {
		// Viewport & Clip Control
		struct ViewportState {
			GLint   x = 0, y = 0;
			GLsizei w = 800, h = 600;
		};
		ViewportState viewport{};

		struct ClipControlState {
			GLenum origin = GL_LOWER_LEFT;
			GLenum depth = GL_NEGATIVE_ONE_TO_ONE;
		};
		ClipControlState clipControl{};

		struct ScissorState {
			GLint   x = 0, y = 0;
			GLsizei w = 0, h = 0; // disabled by default
		};
		ScissorState scissor{};

		void bind() const {
			// Apply viewport
			glViewport(viewport.x, viewport.y, viewport.w, viewport.h);

			// Apply clip control
			glClipControl(clipControl.origin, clipControl.depth);

			// Apply scissor state
			if (scissor.w > 0 && scissor.h > 0) {
				glEnable(GL_SCISSOR_TEST);
				glScissor(scissor.x, scissor.y, scissor.w, scissor.h);
			}
			else {
				glDisable(GL_SCISSOR_TEST);
			}
		}
	};
}



#endif GL_VIEWPORT