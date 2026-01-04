#ifndef UFONT_ATLAS
#define UFONT_ATLAS


#include "bitmap.hpp"

namespace uf::detail {
	struct FontAtlas {
		std::vector<unsigned char> data;
		int w = 0, h = 0;

		struct FontIndex {
			int x, y, w, h;
		};

		std::unordered_map<char, FontIndex> index;
	};

	FontAtlas load_atlas(FontMetric const& fm, std::unordered_map<char, FontCharacter> const& characters) {
		FontAtlas atlas{};


		for (auto const& [c, ch] : characters) {
			atlas.w += ch.width, atlas.h = ch.height > atlas.h ? ch.height : atlas.h;
		}

		atlas.data = std::vector<unsigned char>(atlas.w * atlas.h, unsigned char{ 0 });

		int xStep = 0;
		for (auto [c, ch] : characters) {
			auto [data, bw, bc] = load_bitmap(ch);

			int i = 0;
			for (int x = xStep; x < xStep + ch.width; x++) {
				for (int y = 0; y < ch.height; y++) {
					atlas.data[y * atlas.w + x] = data[y * ch.width + i];
				}
				i++;
			}
			atlas.index.try_emplace(c, FontAtlas::FontIndex{ xStep, 0, ch.width, ch.height });
			xStep += ch.width;
		}
		
		return std::move(atlas);
	}

	FontAtlas load_signed_atlas(FontMetric const& fm, std::unordered_map<char, FontCharacter> const& characters) {
		FontAtlas atlas{};


		for (auto const& [c, ch] : characters) {
			atlas.w += ch.width, atlas.h = ch.height > atlas.h ? ch.height : atlas.h;
		}

		atlas.data = std::vector<unsigned char>(atlas.w * atlas.h, unsigned char{ 0 });

		int xStep = 0;
		for (auto [c, ch] : characters) {
			auto [data, w, h] = load_signed_bitmap(ch, 16);
			int i = 0;
			for (int x = xStep; x < xStep + ch.width; x++) {
				for (int y = 0; y < ch.height; y++) {
					atlas.data[y * atlas.w + x] = data[y * ch.width + i];
				}
				i++;
			}
			atlas.index.try_emplace(c, FontAtlas::FontIndex{ xStep, 0, ch.width, ch.height });
			xStep += ch.width;
		}

		return std::move(atlas);
	}

	FontAtlas load_signed_atlas(FontMetric const& fm, std::unordered_map<char, FontCharacter> const& characters, int padding) {
		FontAtlas atlas{};


		for (auto const& [c, ch] : characters) {
			atlas.w += ch.width + (padding * 2), atlas.h = ch.height > atlas.h ? ch.height : atlas.h;
		}
		atlas.h += padding * 2;


		atlas.data = std::vector<unsigned char>(atlas.w * atlas.h, unsigned char{ 0 });

		int xStep = 0 + padding;
		for (auto [c, ch] : characters) {
			auto [data, w, h] = load_signed_bitmap(ch, 16);
			int i = 0;
			for (int x = xStep; x < xStep + ch.width; x++) {
				for (int y = 0; y < ch.height; y++) {
					atlas.data[(y + padding) * atlas.w + x] = data[y * ch.width + i];
				}
				i++;
			}
			atlas.index.try_emplace(c, FontAtlas::FontIndex{ xStep, 0, ch.width, ch.height });
			xStep += ch.width;
		}

		return std::move(atlas);
	}
}


#endif UFONT_ATLAS