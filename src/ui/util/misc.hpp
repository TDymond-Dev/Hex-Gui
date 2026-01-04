#ifndef UI_MISC
#define UI_MISC

#include <Windows.h>


#include <variant>
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include <array>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>

namespace ui {
#define PI 3.14159265358979323846

	struct color {
		union {
			struct { uint8_t r, g, b, a; };
			uint8_t rgba[4];
		};

		color(int r = 0, int g = 0, int b = 0, int a = 255) : rgba{ (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a } {}
		color(uint32_t packed) { _unpack(packed); }

		auto operator [](int index) { return rgba[index]; }
		int pack() const { return (int)_pack(); }

		operator int() {
			return pack();
		}

	private:
		int32_t _pack() const {
			return (static_cast<uint8_t>(r) << 24) | (static_cast<uint8_t>(g) << 16) | (static_cast<uint8_t>(b) << 8) | static_cast<uint8_t>(a);
		}

		void _unpack(uint32_t packed) {
			r = static_cast<uint8_t>((packed >> 24) & 0xFF);
			g = static_cast<uint8_t>((packed >> 16) & 0xFF);
			b = static_cast<uint8_t>((packed >> 8) & 0xFF);
			a = static_cast<uint8_t>(packed & 0xFF);
		}
	};

	struct Color {
		inline static const color red = color(255, 0, 0);
		inline static const color green = color(0, 255, 0);
		inline static const color blue = color(0, 0, 255);
		inline static const color white = color(255, 255, 255);
		inline static const color black = color(0, 0, 0);
		inline static const color yellow = color(255, 255, 0);
		inline static const color cyan = color(0, 255, 255);
		inline static const color magenta = color(255, 0, 255);
		inline static const color transparent = color(0, 0, 0, 0);
		inline static const color gray = color(128, 128, 128);
		inline static const color darkGray = color(64, 64, 64);
		inline static const color lightGray = color(192, 192, 192);
		inline static const color orange = color(255, 200, 0);
		inline static const color pink = color(255, 175, 175);
		inline static const color brown = color(160, 100, 50);
		inline static const color flutterBlue = color(0, 122, 255);
		inline static const color rgba(int r, int g, int b, int a = 255) { return color(r, g, b, a); }
		inline static const color random() { return color(rand() % 256, rand() % 256, rand() % 256, 255); }
	} col;

	struct Rect {
		int x, y, w, h;

		Rect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}

		bool contains(int px, int py) const {
			return px >= x && px <= x + w && py >= y && py <= y + h;
		}

		bool contains(const Rect& other) const {
			return contains(other.x, other.y) && contains(other.x + other.w, other.y + other.h);
		}

		bool intersects(const Rect& other) const {
			return !(other.x > x + w || other.x + other.w < x || other.y > y + h || other.y + other.h < y);
		}

		std::pair<int, int> coord(float u = 0.5, float v = 0.5) const {
			return { static_cast<int>(x + u * w), static_cast<int>(y + v * h) };
		}

		void translate(int dx, int dy) {
			x += dx, y += dy;
		}

		void scale(float fx, float fy, int anchorX = 0, int anchorY = 0) {
			w = static_cast<int>(w * fx), h = static_cast<int>(h * fy);
			x = anchorX + (x - anchorX) * fx, y = anchorY + (y - anchorY) * fy;
		}

		Rect clipped(const Rect& clipRect) const {
			int nx = max(x, clipRect.x);
			int ny = max(y, clipRect.y);
			int nw = min(x + w, clipRect.x + clipRect.w) - nx;
			int nh = min(y + h, clipRect.y + clipRect.h) - ny;
			if (nw < 0 || nh < 0) return Rect(0, 0, 0, 0); // No intersection
			return Rect(nx, ny, nw, nh);
		}
	};

	enum fAlignment {
		fLeading,
		fCenter,
		fTrailing
	};

	enum fDirection {
		fHorizontal,
		fVertical,
		fStacked
	};

	enum fConstraint {
		fFixed,
		fRelative,
		fFlex
	};

	struct Style {
		color background = col.rgba(44, 44, 44);
		color border = col.black;
		color text = col.white;
		color highlight = col.lightGray;
		color selected = col.flutterBlue;
		color disabled = col.gray;

		int borderSize = 2;
		int fontSize = 30;
		int cornerSize = 10;
	};

	enum fSpacing {
		fSpaceNormal,
		fSpaceBetween,
		fSpaceEvenly,
		fSpaceAround
	};



}

#endif // UI_MISC