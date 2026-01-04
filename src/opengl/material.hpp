#ifndef GL_MATERIAL
#define GL_MATERIAL

#include "shader.hpp"

#include <array>
#include <unordered_map>
#include <variant>

namespace gl {
	//	Common Inputs:
	//		- Delta Time: The time elapsed since the last frame, used for animations and time-based effects.
	//		- Mouse Pos: The position of the mouse cursor in screen space, used for interactive materials.
	//		- Ray hit Pos: The position of the ray hit in world space, used for ray tracing effects.
	//		- Ray Hit direction: The direction of the ray hit in world space, used for ray tracing effects.
	//		- Ray Hit UV: The UV coordinates of the ray hit in texture space, used for texture sampling.
	//		- View Direction: The direction of the camera in world space, used for reflections and refractions.
	//		- View Matrix: The view matrix of the camera, used for transforming vertices from world space to view space.
	//		- u_InvViewMatrix: The inverse view matrix of the camera, used for transforming normals from view space to world space.
	//		- u_InvProjectionMatrix: The inverse projection matrix of the camera, used for transforming vertices from screen space to world space.
	//		- View Position: The position of the camera in world space, used for lighting calculations.
	//		- View Projection Matrix: The projection matrix of the camera, used for transforming vertices from world space to screen space.
	//		- Screen Resolution: The resolution of the screen, used for screen space effects.
	//		- u_PixelSize = 1.0/u_ScreenResolution
 
	// Simple PBR-like material parameter set
	struct MaterialDesc {
		/*
		Shading Channels(PBR)
			- Base Color: The base color of the Material, which is the color that the Material reflects when lit by a light source.
			- Metallic: The metallic property of the Material, which determines how much the Material reflects light like a metal.
			- Roughness: The roughness property of the Material, which determines how smooth or rough the surface of the Material is.
			- Normal: The normal map of the Material, which is used to create the illusion of surface detail by perturbing the surface normals.
			- Emissive: The emissive property of the Material, which determines how much light the Material emits.
			- Opacity: The opacity of the Material, which determines how transparent or opaque the Material is.
			- Opacity Mask: The opacity mask of the Material, which is used to create a mask for the opacity of the Material.
			- Ambient Occlusion: The ambient occlusion property of the Material, which determines how much ambient light the Material receives.
			- Possibilities: Refraction, Parallax, Reflection, Displacement, etc.
		*/
		enum eShadingChannel {
			BaseColor,
			Metallic,
			Roughness,
			Normal,
			Emissive,
			Opacity,
			OpacityMask,
			AmbientOcclusion,
		};
		std::unordered_map<eShadingChannel, Texture2D*> textures; // unit -> texture

		// Scalar/vec parameters
		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, std::array<float, 3>> vec3s;
		std::unordered_map<std::string, std::array<float, 4>> vec4s;
		std::unordered_map<std::string, std::array<float, 16>> mat4s;

		// Stencil (separate front/back)
		bool stencilTest = false;

		struct StencilState {
			GLenum func = GL_ALWAYS;
			GLint  ref = 0;
			GLuint mask = 0xFFFFFFFF;
			GLenum failOp = GL_KEEP;
			GLenum zFailOp = GL_KEEP;
			GLenum passOp = GL_KEEP;
			GLuint writeMask = 0xFFFFFFFF;
		};
		StencilState stencilFront{};
		StencilState stencilBack{};

		// Blending
		bool blendEnable = false;
		struct BlendState {
			GLenum srcColor = GL_ONE;
			GLenum dstColor = GL_ZERO;
			GLenum srcAlpha = GL_ONE;
			GLenum dstAlpha = GL_ZERO;
			GLenum eqColor = GL_FUNC_ADD;
			GLenum eqAlpha = GL_FUNC_ADD;
		};
		BlendState blend{};

		// Color Write Mask
		struct ColorMaskState {
			GLboolean r = GL_TRUE;
			GLboolean g = GL_TRUE;
			GLboolean b = GL_TRUE;
			GLboolean a = GL_TRUE;
		};
		ColorMaskState colorMask{};

		// Clear Color
		std::array<float, 4> clearColor{ 0.0f, 0.0f, 0.0f, 0.0f };

		bool double_sided = false;
		bool ccw_winding = true;

		void setup() {}
	};

	/*
	Blend Mode - Describes how the material will blend source color with what is already in the frame buffer
		BLEND_Opaque: Final color = Source color. This means that the Material will draw on top of the background. This blend mode is compatible with lighting.
		BLEND_Masked: Final color = Source color if OpacityMask > OpacityMaskClipValue, otherwise the pixel is discarded. This blend mode is compatible with lighting.
		BLEND_Translucent: Final color = Source color Opacity + Dest color (1 - Opacity). This blend mode is NOT compatible with dynamic lighting.
		BLEND_Additive: Final color = Source color + Dest color. This blend mode is NOT compatible with dynamic lighting.
		BLEND_Modulate: Final color = Source color x Dest color. This blend mode is NOT compatible with dynamic lighting, or fog, unless this is a decal material.
		AlphaComposite: Used for Materials with textures that have premultiplied alpha. That is, the color channel has already been multiplied by the alpha so when blending with the frame buffer, the GPU can skip the (SrcAlpha * SrcColor) arithmetic that is typically used with alpha blending. Can improve the visual appearance of additive blending.
		AlphaHoldout: Lets you "hold out" the alpha of a Material, punching a hole through Translucent objects directly behind it in view space.
	*/
	enum eBlendMode {
		BLEND_Opaque,
		BLEND_Masked,
		BLEND_Translucent,
		BLEND_Additive,
		BLEND_Modulate,
		AlphaComposite,
		AlphaHoldout
	};

	/*
	Shading Model - Shading Models determine how Material inputs (e.g. Emissive, Diffuse, Specular, Normal) are combined to make the final color.
		- Unlit : The Material is defined by the Emissive and Opacity inputs only. It does not respond to light.
		- Default Lit : The Material is defined by the Base Color, Metallic, Specular, Roughness, Normal, Emissive, Opacity and Opacity Mask inputs. It responds to light.
		- Subsurface : The Material is defined by the Base Color, Metallic, Specular, Roughness, Normal, Emissive, Opacity and Opacity Mask inputs. It responds to light and has subsurface scattering.
		- Clear Coat : Used for Materials that have a translucent coating on top like clear coat car paint or lacquer. Activates the Clear Coat and Clear Coat Roughness inputs.
		- Thin Translucent: Used for Materials that create physically based glass, such as colored or tinted glass. It properly handles white specular from light sources and tinted background objects
			Index-of-Refraction (ior) • Controls Snell’s law for refraction direction. • Also drives Fresnel (reflection vs. transmission balance).
			Thickness (t) • Either a uniform “sheet thickness” or a screen-space thickness buffer (back-face depth minus front-face depth). • This is the per-pixel distance light travels through your glass.
			Absorption/Tint (?<sub>a</sub>) • A 3-component “absorption coefficient” (aka extinction coefficient) in units of 1/length. • Beer–Lambert: transmittance = exp(–?<sub>a</sub> · t). Use this to tint transmitted light.
			Roughness (and Normal Map) • Microfacet roughness for glossy highlights and blurred refractions. • A normal map to perturb the surface and distort both reflection and refraction.
			Environment / Background Buffer • For real-time SSR/refraction: sample the scene color/depth behind your glass, offset by your refracted ray. • Or for ray-traced/back-buffered approaches: sample your skybox or IBL via the refracted vector.
			(Optional) Scattering / Haze (?<sub>s</sub>) • If you want milky or subsurface–scattering effects inside the volume. • You’d add a simple single–bounce scatter or a more complex Henyey–Greenstein term
			(Optional) Dispersion • Small per-channel IOR offsets to create rainbow “prism” effects
		- Anisotropic: Used for Materials that have anisotropic reflections, such as brushed metal or hair. It activates the Anisotropy and Anisotropy Rotation inputs.
		- Iridescent: Used for Materials that have iridescent effects, such as oil slicks or soap bubbles. It activates the Ior, Thickness, and Absorption inputs.
		- Volumetric: Used for Materials that have volumetric effects, such as fog or smoke. It activates the Scattering and Extinction inputs.
		- Hair: Used for Materials that simulate hair. It activates the Hair Color, Hair Roughness, and Hair Anisotropy inputs.
	*/
	enum eShadingModel {
		Unlit,
		DefaultLit,
		Subsurface,
		ClearCoat,
		ThinTranslucent,
		Anisotropic,
		Iridescent,
		Volumetric,
		Hair
	};

	class Material {
		std::unique_ptr<gl::Program> program_;
		MaterialDesc desc_;
		eBlendMode blendMode_ = BLEND_Opaque;
	public:
		Material() = default;

		Material(std::map<GLenum, std::string> const& shaderSources) {
			program_ = std::make_unique<gl::Program>(shaderSources);
		}

		void bind() {
			if (renderer_ == nullptr)
				return;

			desc_.setup();

			renderer_->use();

			// Bind Blendmode

			// Bind textures

			// Bind uniforms

			// Bind Buffers
		}
	}
}



#endif GL_MATERIAL