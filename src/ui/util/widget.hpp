#ifndef UI_WIDGET
#define UI_WIDGET

#include "misc.hpp"
#include "event.hpp"
#include "canvas.hpp"
#include "animation.hpp"
#include "native.hpp"

namespace ui {
	class Widget {
		size_t GenerateUUID() {
			static int currentUUID = 0;
			return currentUUID++;
		}

		size_t uuid_ = GenerateUUID();
		Widget* parent_;
		std::vector<std::unique_ptr<Widget>> children_;

		bool clipsChildren_ = false;
		bool visible_ = true;
		bool enabled_ = true;
		bool mouseOver_ = false;

		bool acceptsDragDrop_ = false;
		bool initiatesDrag_ = false;
		bool consumesDrag_ = false;

		bool clickFocus_ = false;
		bool keyFocus_ = false;
		bool dragStartFocus_ = false;
		bool dragDropFocus_ = false;

		bool wrapChildrenX_ = false;
		bool wrapChildrenY_ = false;

		Rect rect_;
		float xSize_ = 1.0, ySize_ = 1.0;
		std::optional<int> fixedX_ = std::nullopt, fixedY_ = std::nullopt;

		fAlignment xAlignment_ = fCenter, yAlignment_ = fCenter;
		fConstraint xConstraint_ = fFlex, yConstraint_ = fFlex;
	public:
		static inline std::function<void(Widget*, int, int, int, int, eNativeType type)> onMakeTopLevel;
		static inline std::function<Native*(int)> onGetWindow;
		static inline std::function<void(int)> onRemoveTopLevel;
		static inline std::function<void(Widget*)> onCloseWidget;

		Widget() : parent_(nullptr) {}
		~Widget() {}

		std::unique_ptr<Widget> extract() {
			if (parent_ == nullptr) {
				std::cout << "No parent\n";
				return nullptr;
			}

			int idx = index();
			auto temp = std::move(parent_->children_[idx]);
			parent_->children_.erase(parent_->children_.begin() + idx);

			return std::move(temp);
		}

		template<typename T>
		T* insertChild(int index, std::unique_ptr<T> widget) {
			children_.insert(children_.begin() + index, std::move(widget));
			children_[index]->parent_ = this;
			return static_cast<T*>(children_[index].get());
		}

		template<typename child_t, typename ... args_t>
		child_t* insertChildAtIndex(int index, args_t ... args) {
			auto child = std::make_unique<child_t>(args...);
			child->parent_ = this;
			child_t* ptr = child.get();
			children_.insert(children_.begin() + index, std::move(child));
			
			return ptr;
		}

		template<typename child_t, typename ... args_t>
		child_t* addChild(args_t ... args) {
			auto child = std::make_unique<child_t>(args...);
			child->parent_ = this;
			child_t* ptr = child.get();
			children_.push_back(std::move(child));
			return ptr;
		}

		void makeWindow(int x, int y, int w, int h, eNativeType type = fPersistent) {
			onMakeTopLevel(this, x, y, w, h, type);
		}

		void closeWindow() {
			onRemoveTopLevel(uuid_);
		}

		void close() {
			onCloseWidget(this);
		}

		void event(Event* e);

		void paint(Canvas* c) {
			if (!visible_)
				return;

			onPaint(c);

			if (clipsChildren_)
				c->clip(rect_.x, rect_.y, rect_.w, rect_.h);

			for (auto& ch : children_)
				ch->paint(c);

			if (clipsChildren_)
				c->unclip();
		}

		void layout(int x, int y, int w, int h) {
			int rw = xConstraint_ == fRelative ? w * xSize_ : xConstraint_ == fFlex ? w : xSize_;
			int rh = yConstraint_ == fRelative ? h * ySize_ : yConstraint_ == fFlex ? h : ySize_;

			int rx = xConstraint_ == fFlex || xAlignment_ == fLeading ? x : xAlignment_ == fTrailing ? x + (w - rw)  : x + (w - rw) / 2;
			int ry = yConstraint_ == fFlex || yAlignment_ == fLeading ? y : yAlignment_ == fTrailing ? y + (h - rh)  : y + (h - rh) / 2;

			int rxx = fixedX_.has_value() ? fixedX_.value() : rx;
			int ryy = fixedY_.has_value() ? fixedY_.value() : ry;

			rect_ = { rxx, ryy, rw, rh };
			onLayout(rxx, ryy, rw, rh);

			for (auto& child : children_) {
				if (wrapChildrenX_) {
					if (child->rect_.x < rxx) rect_.x = child->rect_.x;
					if (child->rect_.x + child->rect_.w > rxx + rw) rect_.w = (child->rect_.x + child->rect_.w) - rect_.x;
				}
				if (wrapChildrenY_) {
					if (child->rect_.y < ryy) rect_.y = child->rect_.y;
					if (child->rect_.y + child->rect_.h > ryy + rh) rect_.h = (child->rect_.y + child->rect_.h) - rect_.y;
				}
			}
		}

		virtual void postLayout() {}

		// getters
		Widget* root() { return parent_ == nullptr ? this : parent_->root(); }
		Widget* parent() { return parent_; }
		Widget* child(int index) { return children_[index].get(); }
		std::vector<std::unique_ptr<Widget>>& children() { return children_; }
		int x() const { return rect_.x; }
		int y() const { return rect_.y; }
		int w() const { return rect_.w; }
		int h() const { return rect_.h; }
		std::pair<int, int> pos() const { return { x(), y() }; }
		std::pair<int, int> size() const { return { w(), h() }; }
		int xSize() const { return xSize_; }
		int ySize() const { return ySize_; }
		Rect rect() const { return rect_; }
		bool clipsChildren() { return clipsChildren_; }
		bool visible() const { return visible_; }
		bool enabled() const { return enabled_; }
		bool mouseOver() const { return mouseOver_; }
		bool acceptsDragDrop() const { return acceptsDragDrop_; }
		bool initiatesDrag() const { return initiatesDrag_; }
		bool consumesDrag() const { return consumesDrag_; }
		bool clickFocus() const { return clickFocus_; }
		bool keyFocus() const { return keyFocus_; }
		bool dragStartFocus() const { return dragStartFocus_; }
		bool dragDropFocus() const { return dragDropFocus_; }
		fConstraint xConstraint() const { return xConstraint_; }
		fConstraint yConstraint() const { return yConstraint_; }
		fAlignment xAlignment() const { return xAlignment_; }
		fAlignment yAlignment() const { return yAlignment_; }
		bool wrapChildrenX() const { return wrapChildrenX_; }
		bool wrapChildrenY() const { return wrapChildrenY_; }
		int id() const { return uuid_; }
		Native* window() { return onGetWindow(uuid_); }
		Native* window(int id) { return onGetWindow(id); }

		// Setters
		void setClipsChildren(bool v) { clipsChildren_ = v; }
		void setFixedW(int w) { xSize_ = w, xConstraint_ = fFixed; }
		void setFixedH(int h) { ySize_ = h, yConstraint_ = fFixed; }
		void setFixedSize(int w, int h) { xSize_ = w, ySize_ = h, xConstraint_ = fFixed, yConstraint_ = fFixed; }
		void setRelativeW(float w) { xSize_ = w, xConstraint_ = fRelative; }
		void setRelativeH(float h) { ySize_ = h, yConstraint_ = fRelative; }
		void setRelativeSize(float w, float h) { xSize_ = w, ySize_ = h, xConstraint_ = fRelative, yConstraint_ = fRelative; }
		void setFlexW(int lvl) { xConstraint_ = fFlex, xSize_ = lvl; }
		void setFlexH(int lvl) { yConstraint_ = fFlex, ySize_ = lvl; }
		void setFlexSize(int wlvl, int hlvl) { xConstraint_ = fFlex, yConstraint_ = fFlex, xSize_ = wlvl, ySize_ = hlvl; }
		void setLeft() { xAlignment_ = fLeading; }
		void setTop() { yAlignment_ = fLeading; }
		void setCenterX() { xAlignment_ = fCenter; }
		void setCenterY() { yAlignment_ = fCenter; }
		void setRight() { xAlignment_ = fTrailing; }
		void setBottom() { yAlignment_ = fTrailing; }
		void setVisible(bool v) { if (visible_ != v) { visible_ = v; v ? onShow() : onHide(); } }
		void setEnabled(bool v) { if (enabled_ != v) { enabled_ = v; v ? onEnabled() : onDisabled(); } }
		void setAcceptsDragDrop(bool v) { acceptsDragDrop_ = v; }
		void setInitiatesDrag(bool v) { initiatesDrag_ = v; }
		void setConsumesDrag(bool v) { consumesDrag_ = v; }
		void setClickFocus(bool v) { clickFocus_ = v; }
		void setKeyFocus(bool v) { keyFocus_ = v; }
		void setDragStartFocus(bool v) { dragStartFocus_ = v; }
		void setDragDropFocus(bool v) { dragDropFocus_ = v; }
		void setFixedX(int x) { fixedX_ = x; }
		void setFixedY(int y) { fixedY_ = y; }
		void setFixedPos(int x, int y) { fixedX_ = x; fixedY_ = y; }

	protected:
		// Widget Events
		virtual void onEvent(Event* e) {}
		virtual bool onFilterEvent(Event* e) { return false; }
		virtual void onGainFocus() {};
		virtual void onLostFocus() {};
		virtual void onEnter() {};
		virtual void onLeave() {};
		virtual void onHide() {};
		virtual void onShow() {};
		virtual void onDisabled() {};
		virtual void onEnabled() {};
		virtual void onPaint(Canvas* c) {};
		virtual void onClose() {};
		virtual void onLayout(int x, int y, int w, int h) {
			for (auto& ch : children_) 
				ch->layout(rect_.x, rect_.y, rect_.w, rect_.h);
		}
		virtual void onPostLayout() {};

		// Drag Events
		virtual void onDragEnter(DragEvent* e) {};
		virtual void onDragLeave(DragEvent* e) {};
		virtual void onDragStart(DragEvent* e) {};
		virtual void onDragDrop(DragEvent* e) {};
		virtual void onDragMove(DragEvent* e) {};

		// Mouse Events
		virtual void onMouseWheel(MouseEvent* e) {};
		virtual void onMousePress(MouseEvent* e) {};
		virtual void onMouseDoublePress(MouseEvent* e) {};
		virtual void onMouseRelease(MouseEvent* e) {};
		virtual void onMouseMove(MouseEvent* e) {};

		// Key Events
		virtual void onKeyPress(KeyEvent* e) {};
		virtual void onKeyRelease(KeyEvent* e) {};

		// WindowEvents
		virtual void onWindowGainedFocus(WindowEvent* e) {};
		virtual void onWindowLostFocus(WindowEvent* e) {};
		virtual void onWindowClose(WindowEvent* e) {};
		virtual void onWindowEnter(WindowEvent* e) {};
		virtual void onWindowLeave(WindowEvent* e) {};
		virtual void onWindowShow(WindowEvent* e) {};
		virtual void onWindowHide(WindowEvent* e) {};
		virtual void onWindowResize(WindowEvent* e) {};
		virtual void onWindowMove(WindowEvent* e) {};
		virtual void onWindowMinimize(WindowEvent* e) {};
		virtual void onWindowMaximize(WindowEvent* e) {};
		virtual void onWindowRestore(WindowEvent* e) {};
		virtual void onWindowHover(WindowEvent* e) {};

		int index() {
			if (parent_ == nullptr)
				return -1;
			for (int i = 0; i < parent_->children_.size(); i++) {
				if (parent_->children_[i].get() == this)
					return i;
			}
			return -1;
		}
	};

	void Widget::event(Event* e) {
		if (e->ignored() || !enabled())
			return;

		for (auto& child : children())
			child->event(e);

		if (onFilterEvent(e) || e->ignored() || !enabled())
			return;

		onEvent(e);

		bool eventInView = rect().contains(e->x(), e->y());


		if (e->category() == fMouse && eventInView) {
			std::cout << "Mouse EVent" << e->windowID() << std::endl;
			auto me = static_cast<MouseEvent*>(e);
			if (me->action() == fPress) onMousePress(me);
			if (me->action() == fDoublePress) onMouseDoublePress(me);
			if (me->action() == fRelease) onMouseRelease(me);
			if (me->action() == fWheelDown || me->action() == fWheelUp)onMouseWheel(me);
			if (me->action() == fMove) onMouseMove(me);
		}
		if (e->category() == fKey) {
			auto ke = static_cast<KeyEvent*>(e);
			if (ke->action() == fPress) onKeyPress(ke);
			if (ke->action() == fRelease) onKeyRelease(ke);
		}
		if (e->category() == fDrag && eventInView) {
			auto de = static_cast<DragEvent*>(e);
			if (de->action() == fStart) onDragStart(de);
			if (de->action() == fEnd) onDragDrop(de);
			if (de->action() == fMove) onDragMove(de);
		}
		if (e->category() == fWindow) {
			WindowEvent* we = static_cast<WindowEvent*>(e);
			if (we->action() == fGainedFocus) onWindowGainedFocus(we);
			if (we->action() == fLostFocus) onWindowLostFocus(we);
			if (we->action() == fClose) onWindowClose(we);
			if (we->action() == fEnter) onWindowEnter(we);
			if (we->action() == fLeave) onWindowLeave(we);
			if (we->action() == fShow) onWindowShow(we);
			if (we->action() == fHide) onWindowHide(we);
			if (we->action() == fResize) onWindowResize(we);
			if (we->action() == fMove) onWindowMove(we);
			if (we->action() == fMinimize) onWindowMinimize(we);
			if (we->action() == fMaximize) onWindowMaximize(we);
			if (we->action() == fRestore) onWindowRestore(we);
			if (we->action() == fHover) onWindowHover(we);
		}

		if (e->category() == fMouse && !mouseOver_ && eventInView) mouseOver_ = true, onEnter();
		if (e->category() == fMouse && mouseOver_ && !eventInView) mouseOver_ = false, onLeave();
		if (e->category() == fDrag && !mouseOver_ && eventInView) mouseOver_ = true, onDragEnter(static_cast<DragEvent*>(e));
		if (e->category() == fDrag && mouseOver_ && !eventInView) mouseOver_ = false, onDragLeave(static_cast<DragEvent*>(e));
	}

	struct BoxWidget : public Widget {
		BoxWidget() : Widget() {}
		BoxWidget(int w, int h) : Widget() {
			setFixedSize(w, h);
		}

		void onPaint(Canvas* c) {
			c->solid(col.black, 2);
			c->solid(col.flutterBlue);
			c->rect(x(), y(), w(), h());
		}
	};
}

#endif UI_WIDGET