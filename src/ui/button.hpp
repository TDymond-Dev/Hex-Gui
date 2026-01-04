#ifndef UI_BUTTON
#define UI_BUTTON

#include "util/widget.hpp"

namespace ui {
	class TextButton : public Widget {
		std::string text_;
		uf::TextModel tm_;
		std::function<void(std::string const&)> onPress;
	public:
		TextButton(std::string const& text, std::function<void(std::string const&)> onPress) : 
			text_(text), onPress(onPress), Widget() {}

		void onPaint(Canvas* c) {
			c->solid(mouseOver() ? col.lightGray : col.darkGray);
			c->solid(col.black, 4);
			c->rect(x(), y(), w(), h());

			c->solid(col.white);
			c->text(tm_, text_, 48, x(), y(), w(), h());
		}

		void onMousePress(MouseEvent* me) {
			if (me->left() && onPress) {
				onPress(text_);
			}
		}
	};

	class IconButton : public Widget {
		std::string name;
		std::function<void(std::string const&)> onPress;
	public:
		IconButton(std::string const& icon_name, std::function<void(std::string const&)> onPress) : 
			name(icon_name), onPress(onPress), Widget() {}

		void onPaint(Canvas* c) {
			c->solid(mouseOver() ? col.lightGray : col.darkGray);
			c->solid(col.black, 4);
			c->rect(x(), y(), w(), h());

			c->mask(name, x(), y(), w(), h());
			c->rect(x(), y(), w(), h());
		}

		void onMousePress(MouseEvent* me) {
			if (me->left() && onPress) {
				onPress(name);
			}
		}
	};
}


#endif UI_BUTTON