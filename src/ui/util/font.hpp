#ifndef COM_PARSER_FONT
#define COM_PARSER_FONT

#include "parsing/ufont/ufont.hpp"

namespace uf {
	struct UFont {
		uf::Metric metric;
		std::unordered_map<char, uf::Character> characters;
		uf::Atlas atlas;
	};

	void load_font(UFont& font, std::string const& name, int size) {
		font.metric = uf::load_metric(name);
		font.characters = uf::load_characters(font.metric, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}\\|;:'\",.<>/?`~ ");
		font.atlas = uf::load_atlas(font.metric, font.characters, size);
	}
}

#endif // #endif COM_PARSER_FONT