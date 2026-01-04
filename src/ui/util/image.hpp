#ifndef COM_PARSER_IMG
#define COM_PARSER_IMG

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "parsing/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "parsing/stb_image.h"

#include <filesystem>
#include <unordered_map>
#include <array>

namespace ui {
	bool save_image(std::string const& path, std::vector<unsigned char> const& data, int w, int h, int c) {
		int result = 0;
		if (path.substr(path.find_last_of(".") + 1) == "png") {
			result = stbi_write_png(path.c_str(), w, h, c, data.data(), w * c);
		}
		else if (path.substr(path.find_last_of(".") + 1) == "bmp") {
			result = stbi_write_bmp(path.c_str(), w, h, c, data.data());
		}
		else if (path.substr(path.find_last_of(".") + 1) == "tga") {
			result = stbi_write_tga(path.c_str(), w, h, c, data.data());
		}
		else if (path.substr(path.find_last_of(".") + 1) == "jpg") {
			result = stbi_write_jpg(path.c_str(), w, h, c, data.data(), 100); // Quality set to 100
		}
		else {
			return false; // Unsupported format
		}
		return result != 0;
	}

	bool load_image(std::string const& path, std::vector<unsigned char>& data, int& w, int& h, int& c) {
		unsigned char* img = stbi_load(path.c_str(), &w, &h, &c, 0);
		if (img == nullptr) {
			return false;
		}
		data.assign(img, img + (w * h * c));
		stbi_image_free(img);
		return true;
	}

	struct ImageAtlas {
		struct Image {
			int w, h, c;
			std::vector<unsigned char> data;
			std::array<int, 4> region;
			int index;
		};

		enum eFormat {
			RED, GREEN, BLUE, ALPHA, RGB, RGBA
		};

		int w_ = 0, h_ = 0, c_ = 0;
		std::vector<unsigned char> data_;
		std::unordered_map<size_t, Image> images_;
		eFormat format_;

		ImageAtlas() = default;
		ImageAtlas(std::string const& folder, eFormat format) : format_(format) {
			for (const auto& entry : std::filesystem::directory_iterator(folder)) {
				Image image;
				if (load_image(entry.path().string(), image.data, image.w, image.h, image.c)) {
					images_[std::hash<std::string>{}(entry.path().stem().string())] = image;
				}
			}

			float totalW = 0.0, totalH = 0.0;
			switch (format_) {
			case RED: build_(images_, 1); break;
			case BLUE: build_(images_, 1); break;
			case GREEN: build_(images_, 1); break;
			case ALPHA: build_(images_, 1); break;
			case RGB: build_(images_, 3); break;
			case RGBA: build_(images_, 4); break;
			}
		}

		void save_atlas(std::string const& path) const {
			save_image(path, data_, w_, h_, c_);
		}

		Image const& operator[](std::string const& key) const {
			return images_.at(std::hash<std::string>{}(key));
		}

		Image& operator[](std::string const& key) {
			return images_.at(std::hash<std::string>{}(key));
		}

		Image& getRegion(std::string const& key) {
			return images_.at(std::hash<std::string>{}(key));
		}

		std::vector<int> get_regions() const {
			std::vector<int> regions(images_.size() * 4);
			int i = 0;
			for (auto const& [key, p] : images_) {
				regions[i + 0] = p.region[0], regions[i + 1] = p.region[1], regions[i + 2] = p.region[2], regions[i + 3] = p.region[3];
				i += 4;
			}
			return regions;
		}
	private:
		void build_(std::unordered_map<size_t, Image>& images, int c) {

			float totalW = 0.0, totalH = 0.0;
			for (auto const& [key, p] : images)
				totalW += p.w, totalH = (std::max)((int)totalH, p.h);

			w_ = totalW, h_ = totalH, c_ = c;
			data_ = std::vector<unsigned char>(w_ * h_ * c_, 0);

			int x = 0.0, l = 0;

			if (format_ == RED) {
				for (auto& [key, p] : images) {
					for (int i = 0; i < p.h; ++i) {
						for (int j = 0; j < p.w; ++j) {
							data_[(i * w_ + x + j) * c_ + 0] = p.data[(i * p.w + j) * p.c + 0];
						}
					}
					p.index = l++;
					p.region = { x, 0, p.w, p.h };
					x += p.w;
				}
			}

			if (format_ == GREEN) {
				for (auto& [key, p] : images) {
					for (int i = 0; i < p.h; ++i) {
						for (int j = 0; j < p.w; ++j) {
							data_[(i * w_ + x + j) * c_ + 0] = p.data[(i * p.w + j) * p.c + 1];
						}
					}
					p.index = l++;
					p.region = { x, 0, p.w, p.h };
					x += p.w;
				}
			}

			if (format_ == BLUE) {
				for (auto& [key, p] : images) {
					for (int i = 0; i < p.h; ++i) {
						for (int j = 0; j < p.w; ++j) {
							data_[(i * w_ + x + j) * c_ + 0] = p.data[(i * p.w + j) * p.c + 2];
						}
					}
					p.index = l++;
					p.region = { x, 0, p.w, p.h };
					x += p.w;
				}
			}

			if (format_ == ALPHA) {
				for (auto& [key, p] : images) {
					for (int i = 0; i < p.h; ++i) {
						for (int j = 0; j < p.w; ++j) {
							data_[(i * w_ + x + j) * c_ + 0] = p.data[(i * p.w + j) * p.c + 3];
						}
					}
					p.index = l++;
					p.region = { x, 0, p.w, p.h };
					x += p.w;
				}
			}

			if (format_ == RGB) {
				for (auto& [key, p] : images) {
					for (int i = 0; i < p.h; ++i) {
						for (int j = 0; j < p.w; ++j) {
							data_[(i * w_ + x + j) * c_ + 0] = p.data[(i * p.w + j) * p.c + 0];
							data_[(i * w_ + x + j) * c_ + 1] = p.data[(i * p.w + j) * p.c + 1];
							data_[(i * w_ + x + j) * c_ + 2] = p.data[(i * p.w + j) * p.c + 2];
						}
					}
					p.index = l++;
					p.region = { x, 0, p.w, p.h };
					x += p.w;
				}
			}

			if (format_ == RGBA) {

				for (auto& [key, p] : images) {
					for (int i = 0; i < p.h; ++i) {
						for (int j = 0; j < p.w; ++j) {
							for (int k = 0; k < p.c; ++k) {
								data_[(i * w_ + x + j) * c_ + 0] = p.data[(i * p.w + j) * p.c + 0];
								data_[(i * w_ + x + j) * c_ + 1] = p.data[(i * p.w + j) * p.c + 1];
								data_[(i * w_ + x + j) * c_ + 2] = p.data[(i * p.w + j) * p.c + 2];
								data_[(i * w_ + x + j) * c_ + 3] = p.data[(i * p.w + j) * p.c + 3];
							}
						}
					}
					p.index = l++;
					p.region = { x, 0, p.w, p.h };
					x += p.w;
				}
			}
		}
	};
}


#endif // !COM_PARSER_IMG