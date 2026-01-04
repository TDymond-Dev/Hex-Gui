#ifndef UI_LIST
#define UI_LIST

#include "layout.hpp"

namespace ui {
	class List : public Widget {
		std::vector<std::string> items;
		std::function<void(std::string)> onSelect_;
		int itemHeight_ = 50;
		uf::TextModel tm_;
		int highlightIndex = -1, selectedIndex = -1;
	public:
		List(std::vector<std::string> const& items, std::function<void(std::string)> onSelect = nullptr)
			: items(items), onSelect_(onSelect), Widget() {}

		void onPaint(Canvas* c) override {
			c->solid(col.darkGray);
			c->solid(col.black, 4);
			c->rect(x(), y(), w(), h());

			for (int i = 0; i < items.size(); i++) {
				if (highlightIndex == i) {
					c->solid(col.lightGray);
					c->rect(x(), y() + i * itemHeight_, w(), itemHeight_);
				}
				if (selectedIndex == i) {
					c->solid(col.flutterBlue);
					c->rect(x(), y() + i * itemHeight_, w(), itemHeight_);
				}

				c->solid(col.white);
				c->text(tm_, items[i], 48, x(), y() + i * itemHeight_, w(), itemHeight_);
			}
		}

		void onMouseMove(MouseEvent* me) {
			bool highlighted = false;
			for (int i = 0; i < items.size(); i++) {
				if (me->y() >= this->y() + i * itemHeight_ && me->y() <= this->y() + (i + 1) * itemHeight_) {
					highlightIndex = i, highlighted = true;
				}
			}

			if (!highlighted) 
				highlightIndex = -1;
		}

		void onMousePress(MouseEvent* me) override {
			if (me->left()) {
				int index = indexAt(me->x(), me->y());
				if (index != -1 && onSelect_) {
					selectedIndex = index;
					onSelect_(items[index]);
				}
			}
		}

	private:
		int indexAt(int x, int y) {
			for (int i = 0; i < items.size(); i++) {
				if (y >= this->y() + i * itemHeight_ && y <= this->y() + (i + 1) * itemHeight_) {
					return i;
				}
			}
			return -1;
		}
	};
}



#endif UI_LIST