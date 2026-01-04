#ifndef UI_BOX_LAYOUT
#define UI_BOX_LAYOUT

#include "util/widget.hpp"

namespace ui {
	class Layout : public Widget {
		fDirection direction_ = fStacked;
		fSpacing spacing_ = fSpaceNormal;
		fAlignment childAlignment_ = fCenter;
		int spacingSize_ = 0;
	public:
		Layout() = default;
		Layout(fDirection dir) : direction_(dir) {}

		void setSpacingSize(int size) { spacingSize_ = size; }
		void setChildAlignment(fAlignment a) { childAlignment_ = a; }
		void setDirection(fDirection d) { direction_ = d; }
		void setSpacing(fSpacing s) { spacing_ = s; }

		fDirection direction() const { return direction_; }
		fSpacing spacing() const { return spacing_; }
		fAlignment childAlignment() const { return childAlignment_; }
		int spacingSize() const { return spacingSize_; }

		virtual void onLayout(int x, int y, int w, int h) override {
			if (direction_ == fStacked) onLayout_Stacked({x, y, w, h});
			if (direction_ == fVertical) onLayout_Vertical({ x, y, w, h });
			if (direction_ == fHorizontal) onLayout_Horizontal({ x, y, w, h });
		}
	private:
		void onLayout_Stacked(Rect const& r) {
			for (int i = 0; i < children().size(); i++) {

				float w = child(i)->xConstraint() == fFlex ? r.w : child(i)->xConstraint() == fRelative ? child(i)->xSize() * r.w : child(i)->xSize();
				float h = child(i)->yConstraint() == fFlex ? r.h : child(i)->yConstraint() == fRelative ? child(i)->ySize() * r.h : child(i)->ySize();
				child(i)->layout( r.x, r.y, int16_t(w), int16_t(h) );
			}
		}

		void onLayout_Vertical(Rect const& r) {
			float yOffset = 0.0, temp_spacing = 0;
			auto [staticH, totalFlexH, flexCount, staticCount] = dataVertical(r.h);

			if (flexCount == 0) {
				if (childAlignment() == fLeading && spacing() == fSpaceNormal) yOffset = 0;
				if (childAlignment() == fCenter && spacing() == fSpaceNormal) yOffset = (r.h - staticH) / 2;
				if (childAlignment() == fTrailing && spacing() == fSpaceNormal) yOffset = r.h - staticH;

				int spacingHeight = (r.h - staticH) + ((children().size() - 1) * spacing());
				if (spacing() == fSpaceBetween)
					temp_spacing = spacingHeight / (children().size() - 1), yOffset = 0;
				if (spacing() == fSpaceEvenly)
					temp_spacing = spacingHeight / (children().size() + 1), yOffset = temp_spacing;
				if (spacing() == fSpaceAround)
					temp_spacing = spacingHeight / children().size(), yOffset = temp_spacing / 2;
				if (spacing() == fSpaceNormal)
					temp_spacing = spacingSize();
			}

			int childIndex = 0;
			for (auto& child : children()) {
				float childH = child->ySize();
				if (child->yConstraint() == fFlex) childH = totalFlexH * (childH / (float)flexCount);
				else if (child->yConstraint() == fRelative) childH = childH * r.h;

				child->layout( r.x, int16_t(r.y + yOffset), r.w, (int16_t)childH);
				yOffset += childH;
				yOffset += temp_spacing;
			}
		}

		void onLayout_Horizontal(Rect const& r) {
			float xOffset = 0.0, temp_spacing = 0;
			auto [staticW, totalFlexW, flexCount, staticCount] = dataHorizontal(r.w);

			if (flexCount == 0) {
				if (childAlignment() == fLeading && spacing() == fSpaceNormal) xOffset = 0;
				if (childAlignment() == fCenter && spacing() == fSpaceNormal) xOffset = (r.w - staticW) / 2;
				if (childAlignment() == fTrailing && spacing() == fSpaceNormal) xOffset = r.w - staticW;

				int spacingWidth = (r.w - staticW) + ((children().size() - 1) * spacing());
				if (spacing() == fSpaceBetween)
					temp_spacing = spacingWidth / (children().size() - 1), xOffset = 0;
				if (spacing() == fSpaceEvenly)
					temp_spacing = spacingWidth / (children().size() + 1), xOffset = temp_spacing;
				if (spacing() == fSpaceAround)
					temp_spacing = spacingWidth / children().size(), xOffset = temp_spacing / 2;
				if (spacing() == fSpaceNormal)
					temp_spacing = spacingSize();
			}

			int childIndex = 0;
			for (auto& child : children()) {

				float childW = child->xSize();
				if (child->xConstraint() == fFlex) childW = totalFlexW * (childW / (float)flexCount);
				else if (child->xConstraint() == fRelative) childW = childW * r.w;

				child->layout(int16_t(r.x + xOffset), r.y, int16_t(childW) , r.h);
				xOffset += childW;
				xOffset += temp_spacing;
			}
		}

		std::tuple<int, int, int, int> dataVertical(float rh) {
			int staticH = 0, staticCount = 0, visibleCount = 0, flexCount = 0;

			for (auto& child : children()) {
				if (child->yConstraint() == fFixed)
					staticH += child->ySize(), ++staticCount;
				if (child->yConstraint() == fRelative)
					staticH += rh * child->ySize(), ++staticCount;
				if (child->yConstraint() == fFlex)
					flexCount += child->ySize();

				++visibleCount;
			}

			staticH += (staticCount - 1) * spacing();
			float totalFlexSpace = (rh - staticH);

			return std::make_tuple(staticH, totalFlexSpace, flexCount, staticCount);
		}

		std::tuple<int, int, int, int> dataHorizontal(float rw) {
			int staticW = 0, staticCount = 0, visibleCount = 0, flexCount = 0;

			for (auto& child : children()) {
				if (child->xConstraint() == fFixed)
					staticW += child->xSize(), ++staticCount;
				if (child->xConstraint() == fRelative)
					staticW += rw * child->xSize(), ++staticCount;
				if (child->xConstraint() == fFlex)
					flexCount += child->xSize();

				++visibleCount;
			}

			staticW += (staticCount - 1) * spacing();
			float totalFlexSpace = (rw - staticW);

			return std::make_tuple(staticW, totalFlexSpace, flexCount, staticCount);
		}
	};

	struct SLayout : public Layout {
		SLayout() : Layout(fStacked) {}
	};

	struct VLayout : public Layout {
		VLayout() : Layout(fVertical) {}
	};

	struct HLayout : public Layout {
		HLayout() : Layout(fHorizontal) {}
	};
}

#endif // UI_BOX_LAYOUT