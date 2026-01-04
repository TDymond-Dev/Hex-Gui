#ifndef UI_SCROLL
#define UI_SCROLL

#include "util/widget.hpp"

namespace ui {
	class VScroll : public Widget {
		bool scrollbarVisible_ = false;
		int scrollOffset_ = 0;
		int scrollBarWidth = 20;
	public:
		VScroll() : Widget() {
			setClipsChildren(true);
			setScrollbarVisible(true);
		}

		void onLayout(int rx, int ry, int rw, int rh) {
			for (auto& child : children()) {
				child->layout(rx, ry + scrollOffset_, rw, rh);
			}
		}

		void onMouseWheel(MouseEvent* me) override {
			scrollOffset_ += me->wheelUp() ? -20 : 20;

			me->setIgnored(true);
		}

		void onPaint(Canvas* c) override {
			if (scrollbarVisible_) {
				int scrollPos = (int)((float)(-scrollOffset_) / (float)totalChildHeight() * (float)h());
				float scrollHeight = (h() / totalChildHeight()) * h();

				c->solid(col.darkGray);
				c->rect(x() + w() - scrollBarWidth, y() - scrollPos, scrollBarWidth, scrollHeight);
			}
		}

		void setScrollOffset(int offset) {
			scrollOffset_ = offset;
		}

		int scrollOffset() const {
			return scrollOffset_;
		}

		void setScrollbarVisible(bool v) {
			scrollbarVisible_ = v;
		}

		bool scrollbarVisible() const {
			return scrollbarVisible_;
		}

	private:
		int totalChildHeight() {
			return child(children().size() - 1)->y() + child(children().size() - 1)->h();
		}
	};
}




#endif UI_SCROLL