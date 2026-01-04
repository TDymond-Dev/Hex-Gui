#ifndef UI_SPLITTER
#define UI_SPLITTER

#include "layout.hpp"

#include <algorithm>

namespace ui {
	class VSplitter : public VLayout {
		int handleHeight = 80;
		float handleWeight = 0.5f;
		Widget* leading_ = nullptr;
		Widget* trailing_ = nullptr;
		float minWeight = 0.1, maxWeight = 0.9;
	public:
		VSplitter(float handleWeight = 0.5f) : handleWeight(handleWeight), VLayout() {
			setInitiatesDrag(true);
			setConsumesDrag(true);
		};

		template<typename widget_t, typename ... args_t>
		widget_t* setLeading(args_t ... args) {
			leading_ = addChild<widget_t>(args...);
			return static_cast<widget_t*>(leading_);
		}
		template<typename widget_t, typename ... args_t>
		widget_t* setTrailing(args_t ... args) {
			trailing_ = addChild<widget_t>(args...);
			return static_cast<widget_t*>(trailing_);
		};


		void onLayout(int rx, int ry, int rw, int rh) override {
			int availableHeight = rh - handleHeight;
			int leadingHeight = int(availableHeight * handleWeight);
			int trailingHeight = availableHeight - leadingHeight;

			leading_->layout(rx, ry, rw, leadingHeight);
			
			trailing_->layout(rx, ry + leadingHeight + handleHeight, rw, trailingHeight);
		}

		void onDragMove(DragEvent* de) override {
			if (de->y() < y() + leading_->h() || de->y() > y() + h() - trailing_->h())
				return;

			handleWeight = std::clamp(newWeight(de->y()), minWeight, maxWeight);
		}

	private:
		float newWeight(int y) {
			int handleStart = y - this->y() - (handleHeight / 2);
			int availableHeight = h() - handleHeight;
			return float(handleStart) / float(availableHeight);
		}
	};

	class HSplitter : public HLayout {
		int handleWidth = 80;
		float handleWeight = 0.5f;
		Widget* leading_ = nullptr;
		Widget* trailing_ = nullptr;
		float minWeight = 0.1f, maxWeight = 0.9f;

	public:
		HSplitter(float handleWeight = 0.5f)
			: handleWeight(handleWeight), HLayout()
		{
			setInitiatesDrag(true);
			setConsumesDrag(true);
		}

		template<typename widget_t, typename ... args_t>
		widget_t* setLeading(args_t ... args) {
			leading_ = addChild<widget_t>(args...);
			return static_cast<widget_t*>(leading_);
		}

		template<typename widget_t, typename ... args_t>
		widget_t* setTrailing(args_t ... args) {
			trailing_ = addChild<widget_t>(args...);
			return static_cast<widget_t*>(trailing_);
		}

		void onLayout(int rx, int ry, int rw, int rh) override {
			int availableWidth = rw - handleWidth;
			int leadingWidth = int(availableWidth * handleWeight);
			int trailingWidth = availableWidth - leadingWidth;

			leading_->layout(rx, ry, leadingWidth, rh);

			trailing_->layout(rx + leadingWidth + handleWidth, ry, trailingWidth, rh);
		}

		void onDragMove(DragEvent* de) override {
			if (de->x() < x() + leading_->w() || de->x() > x() + w() - trailing_->w())
				return;

			handleWeight = std::clamp(newWeight(de->x()), minWeight, maxWeight);
		}

	private:
		float newWeight(int xPos) {
			int handleStart = xPos - this->x() - (handleWidth / 2);
			int availableWidth = w() - handleWidth;
			return float(handleStart) / float(availableWidth);
		}
	};



	
	
}

#endif UI_SPLITTER