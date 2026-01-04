#ifndef UI_CHECKBOX
#define UI_CHECKBOX

#include "layout.hpp"

namespace ui {
	class Checkbox : public Widget {
		std::string text_;
		bool selected_ = false;
		std::function<void(bool)> onChange_;
		uf::TextModel tm_;
	public:
		Checkbox(const std::string& text, std::function<void(bool)> onChange = nullptr)
			: text_(text), onChange_(onChange), Widget() {}

		void onMousePress(MouseEvent* me) override {
			if (me->left()) {
				setSelected(!selected_);
			}
		}

		void setSelected(bool s) { 
			if (selected_ != s && onChange_)
				onChange_(s);

			selected_ = s; 
		}

		void onPaint(Canvas* c) override {
			c->solid(col.darkGray);
			c->solid(col.black, 4);
			c->rect(x(), y(), w(), h());

			if (selected_) {
				c->solid(col.flutterBlue);
				c->rect(x(), y(), w(), h());
			}

			if (mouseOver()) {
				c->solid(col.lightGray);
				c->rect(x(), y(), w(), h());
			}

			c->solid(col.white);
			c->text(tm_, text_, 48, x(), y(), w(), h());
		}

		bool selected() const { return selected_; }
	};

	class CheckboxGroup : public VLayout {
		bool multiSelect_ = false;

	public:
		CheckboxGroup(std::vector<std::string> const& options, std::function<void(bool)> onChange) : VLayout() {
			for (auto& option : options) {
				auto cb = addChild<Checkbox>(option, onChange);
			}
		}

		void setMultiSelect(bool ms) { multiSelect_ = ms; }

		void onMousePress(MouseEvent* me) {
			if (me->left()) {
				int index = indexAt(me->x(), me->y());
				if (index < 0 || index > children().size())
					return;

				for (int i = 0; i < children().size(); i++) {
					auto cb = static_cast<Checkbox*>(child(i));
					if (i != index && !multiSelect_) {
						cb->setSelected(false);
					}
					
				}
			}
		}

	private:
		int indexAt(int x, int y) {
			for (int i = 0; i < children().size(); i++) {
				if (child(i)->rect().contains(x, y))
					return i;
			}
			return -1;
		}
	};
}


#endif UI_CHECKBOX