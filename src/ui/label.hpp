#ifndef UI_LABEL
#define UI_LABEL

#include "util/widget.hpp"

namespace ui {
	struct Label : public Widget {
		std::string text_;
		uf::TextModel textModel_;

		Label(std::string const& text) : Widget(), text_(text) {
			textModel_ = uf::TextModel();
		}

		void setText(std::string const& text) {
			text_ = text;
		}
		std::string const& text() const {
			return text_;
		}
		virtual void onPaint(Canvas* c) override {
			c->solid(col.white);
			c->text(textModel_, text_, 24, x(), y(), w(), h());
		}
	};
}


#endif UI_LABEL