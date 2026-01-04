#ifndef UI_SLIDER
#define UI_SLIDER

#include "util/widget.hpp"

namespace ui {
	class Slider : public Widget {
		float weight_ = 0.5;
		std::optional<float> min, max;

		uf::TextModel model_;

		std::function<void(float)> onChange;
	public:
		Slider(std::function<void(float)> onChange) : onChange(onChange)
		{
			setInitiatesDrag(true);
			setConsumesDrag(true);
		}

		void onDragMove(DragEvent* e) {
			weight_ = float((e->x() - x())) / float(w());
		}

		void onPaint(Canvas* c) {
			c->solid(col.darkGray);
			c->solid(col.black, 4);
			c->rect(x(), y(), w(), h());

			c->solid(col.flutterBlue);
			c->rect(x(), y(), w() * weight_, h());

			c->solid(col.white);
			c->text(model_, std::to_string(weight_), 18, x(), y(), w(), h());
		}
	};
}



#endif UI_SLIDER