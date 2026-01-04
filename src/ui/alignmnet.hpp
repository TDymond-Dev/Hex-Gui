#ifndef UI_ALIGNMENT
#define UI_ALIGNMENT

#include "util/widget.hpp"

namespace ui {
	struct Margin : public Widget {
		int margin_left_ = 0, margin_right_ = 0, margin_top_ = 0, margin_bottom_ = 0;
		Margin(int v) : margin_left_(v), margin_right_(v), margin_top_(v), margin_bottom_(v) {}

		void onLayout(int rx, int ry, int rw, int rh) override {
			for (auto& child : children()) {
				child->layout(x() + margin_left_, y() + margin_top_, w() - margin_left_ - margin_right_, h() - margin_top_ - margin_bottom_);
			}
		}
	};

	struct Anchor : public Widget {
		int x_pos = 0, y_pos = 0;
		enum eAnchor {
			TopLeft, Top, TopRight, Left, Center, Right, BottomLeft, Bottom, BottomRight
		};

		eAnchor parent_anchor_ = Center, child_anchor_ = Center;
		Anchor(eAnchor parent, eAnchor child) : parent_anchor_(parent), child_anchor_(child) {}

		void onLayout(int rx, int ry, int rw, int rh) override {
			for (auto& child : children()) {
				child->layout(x(), y(), w(), h());
			}


			switch (parent_anchor_) {
			case TopLeft: x_pos = x(); y_pos = y(); break;
			case Top: x_pos = x() + w() / 2; y_pos = y(); break;
			case TopRight: x_pos = x() + w(); y_pos = y(); break;
			case Left: x_pos = x(); y_pos = y() + h() / 2; break;
			case Center: x_pos = x() + w() / 2; y_pos = y() + h() / 2; break;
			case Right: x_pos = x() + w(); y_pos = y() + h() / 2; break;
			case BottomLeft: x_pos = x(); y_pos = y() + h(); break;
			case Bottom: x_pos = x() + w() / 2; y_pos = y() + h(); break;
			case BottomRight: x_pos = x() + w(); y_pos = y() + h(); break;
			}

			auto c = child(0);
			switch (child_anchor_) {
			case TopLeft: x_pos -= 0; y_pos -= 0; break;
			case Top: x_pos -= c->w() / 2; y_pos -= 0; break;
			case TopRight: x_pos -= c->w(); y_pos -= 0; break;
			case Left: x_pos -= 0; y_pos -= c->h() / 2; break;
			case Center: x_pos -= c->w() / 2; y_pos -= c->h() / 2; break;
			case Right: x_pos -= c->w(); y_pos -= c->h() / 2; break;
			case BottomLeft: x_pos += 0; y_pos -= c->h(); break;
			case Bottom: x_pos -= c->w() / 2; y_pos -= c->h(); break;
			case BottomRight: x_pos -= c->w(); y_pos -= c->h(); break;
			}
		}

		void onPostLayout() override {
			child(0)->setFixedPos(x_pos, y_pos);
		}
	};
}

#endif UI_ALIGNMENT