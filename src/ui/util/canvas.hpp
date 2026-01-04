#ifndef UI_CANVAS
#define UI_CANVAS

#include "misc.hpp"
#include "font.hpp"
#include "image.hpp"

namespace ui {
	struct RenderObject {
		int x, y, w, h;
		int type, brushIndex = -1, penIndex = -1, clipIndex = -1;
		int maskIndex = -1, boundsIndex = -1, r1, r2;
	};

	struct Canvas {
		ImageAtlas maskAtlas, imageAtlas;
		//uf::UFont font;

		enum eType { 
			eText, eRect, eRRect, eCircle, eEllipse, eLine
		};

		enum eStroke {
			eSolid, eLinear, eRadial, eConical, eImage, eRender 
		};

		Canvas(std::string const& src, std::string const& font_name, int fSize) {
			maskAtlas = ImageAtlas(src + "/icons", ImageAtlas::ALPHA);
			imageAtlas = ImageAtlas(src + "/images", ImageAtlas::RGB);

			static bool fontLoaded = false;

			if (!fontLoaded) {
				uf::LoadGlobal(font_name, fSize);
				fontLoaded = true;
			}
		}

		void text(uf::TextModel tm, std::string const& s, int size, float x, float y, float w, float h) {
			tm.cache(x, y, w, h, s, size);
			bounds(x, y, w, h);

			int j = 0;
			for (auto& c : tm.characters()) {

				int fIndex = mData.size();
				auto& region = uf::gAtlas.position(s[j++]);
				mData.insert(mData.end(), { region[0], region[1], region[2], region[3] });
				auto back = createRO(c.x, c.y, c.w, c.h, eText, false);
				if (back) back->r1 = fIndex;
			}
			post();
		}

		void rect(float x, float y, float w, float h) {
			createRO(x, y, w, h, eRect);
		}

		void rrect(int x, int y, int w, int h, int cr) { 
			rrect(x, y, w, h, cr, cr, cr, cr); 
		}

		void rrect(int x, int y, int w, int h, int tl, int tr, int br, int bl) {
			auto obj = createRO(x, y, w, h, eRRect);
			if (obj) obj->r1 = mData.size();
			mData.insert(mData.end(), { tl, tr, br, bl });
		}

		void circle(int x, int y, int r) {
			createRO(x - r, y - r, r * 2, r * 2, eCircle);
		}

		void ellipse(int x, int y, int w, int h) {
			createRO(x, y, w, h, eEllipse);
		}

		void line(int x1, int y1, int x2, int y2, int thickness) {
			auto obj = createRO(x1, y1, x2, y2, eLine);
			if (obj) obj->r1 = thickness;
		}

		void solid(color const& c, int w = 0) {
			stroke(w == 0 ? brushIndex : penIndex, { eSolid, w, c.pack() });
		}

		void linear(color const& c1, color const& c2, int x1, int y1, int x2, int y2, int w = 0) {
			stroke(w == 0 ? brushIndex : penIndex, { eLinear, w, c1.pack(), c2.pack(), x1, y1, x2, y2 });
		}

		void radial(color const& c1, color const& c2, int x, int y, int r, int w = 0) {
			stroke(w == 0 ? brushIndex : penIndex, { eRadial, w, c1.pack(), c2.pack(), x, y, r });
		}

		void conical(color const& c1, color const& c2, int x, int y, int r, int w = 0) {
			stroke(w == 0 ? brushIndex : penIndex, { eConical, w, c1.pack(), c2.pack(), x, y, r });
		}

		void image(std::string const& s, int w = 0) {
			auto& region = imageAtlas[s].region;
			stroke(w == 0 ? brushIndex : penIndex, { eImage, 0, region[0], region[1], region[2], region[3] });
		}

		void render() {
			stroke(brushIndex, { eRender, 0 });
		}

		void mask(std::string const& s, int w = 0) {
			maskIndex = mData.size();
			auto& region = maskAtlas[s].region;
			int index;
			stroke(index, { region[0], region[1], region[2], region[3] });
		}

		void clip(int x, int y, int w, int h) {
			cX = x, cY = y, cW = w, cH = h;
			clipIndex = mData.size();
			mData.insert(mData.end(), { x, y, w, h });
		}

		void unclip() {
			clipIndex = -1;
		}

		void clear() {
			mObjects.clear();
			mOverlayObjects.clear();
			mData.clear();
			post();
		}

		void bounds(int x, int y, int w, int h) {
			boundsIndex = mData.size();
			mData.insert(mData.end(), { x, y, w, h });
		}

		auto data() {
			if (!mOverlayObjects.empty())
				mObjects.insert(mObjects.end(), mOverlayObjects.begin(), mOverlayObjects.end());
			return std::make_tuple(mObjects, mData);
		}


		void setOverlay(bool b) {
			overlay = b;
		}
	private:
		void stroke(int& index, std::vector<int> const& data) {
			index = mData.size();
			mData.insert(mData.end(), data.begin(), data.end());
		}

		bool intersects(int x, int y, int w, int h) {
			return x < cX + cW && x + w > cX && y < cY + cH && y + h > cY;
		}

		RenderObject* createRO(float x, float y, float w, float h, int type, bool post = true) {
			if (clipIndex != -1 && !intersects(x, y, w, h))
				return nullptr;

			if (overlay) {
				auto& ro = mOverlayObjects.emplace_back();
				ro.x = x, ro.y = y, ro.w = w, ro.h = h, ro.type = type;
				ro.brushIndex = brushIndex, ro.penIndex = penIndex, ro.clipIndex = clipIndex, ro.maskIndex = maskIndex, ro.boundsIndex = boundsIndex;
				if (post)
					this->post();
				return &mOverlayObjects.back();
			}
			else {
				auto& ro = mObjects.emplace_back();
				ro.x = x, ro.y = y, ro.w = w, ro.h = h, ro.type = type;
				ro.brushIndex = brushIndex, ro.penIndex = penIndex, ro.clipIndex = clipIndex, ro.maskIndex = maskIndex, ro.boundsIndex = boundsIndex;
				if (post)
					this->post();
				return &mObjects.back();
			}
		}

		void post() {
			brushIndex = -1, penIndex = -1, boundsIndex = -1, maskIndex = -1;
		}

		int brushIndex = -1, penIndex = -1, clipIndex = -1, maskIndex = -1, boundsIndex = -1;
		int cX, cY, cW, cH;
		bool overlay = false;
		std::vector<RenderObject> mObjects;
		std::vector<RenderObject> mOverlayObjects;
		std::vector<int> mData;
	};
}

#endif // UI_CANVAS