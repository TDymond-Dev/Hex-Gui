#ifndef UI_TAB
#define UI_TAB

#include "util/widget.hpp"
#include "layout.hpp"
#include "label.hpp"

namespace ui {
	enum ePosition {
		Left,
		Right,
		Top,
		Bottom,
		Middle
	};

	class Tab : public VLayout {
		class Header : public Label {
			Tab* tab_;
		public:
			Header(std::string const& s, Tab* t) : tab_(t), Label(s) {}

			void onMousePress(MouseEvent* me) {
				if (me->left()) tab_->changeTab_(tab_->indexAt(me->x(), me->y()));
				if (me->right()) tab_->removeTab_(tab_->indexAt(me->x(), me->y()));
			}
		};

		HLayout* headerRow_ = nullptr;
		SLayout* content_ = nullptr;

		size_t currentIndex_ = 0;

		size_t dragStartIndex;
	public:
		Tab() : VLayout() {
			headerRow_ = addChild<HLayout>();
			headerRow_->setFixedH(50);
			content_ = addChild<SLayout>();

			setInitiatesDrag(true);
			setConsumesDrag(true);
			setAcceptsDragDrop(true);
		}

		template<typename widget_t, typename ... args_t>
		widget_t* addTab(std::string const& title, args_t ... args) {
			auto header = headerRow_->addChild<Header>(title, this);
			auto content = content_->addChild<widget_t>(args...);
			
			changeTab_(int(header->children().size()));

			return static_cast<widget_t*>(content);
		}

		void changeTab(size_t index) {
			changeTab_(int(index));
		}

		void removeTab(size_t index) {
			removeTab_(int(index));
		}

		void swapTabs(size_t index1, size_t index2) {
			if (index1 < 0 || index1 >= headerRow_->children().size())
				return;
			if (index2 < 0 || index2 >= headerRow_->children().size())
				return;
			std::swap(headerRow_->children()[index1], headerRow_->children()[index2]);
			std::swap(content_->children()[index1], content_->children()[index2]);
			
			if (currentIndex_ == index1)
				changeTab_(index2);
			else if (currentIndex_ == index2)
				changeTab_(index1);
		}

		std::optional<std::function<void(size_t)>> onTabChanged;

		void onDragStart(DragEvent* de) override {
			dragStartIndex = indexAt(de->x(), de->y());
		}

		void onDragDrop(DragEvent* de) override {
			size_t dropIndex = indexAt(de->x(), de->y());
			if (dropIndex != -1 && dragStartIndex != -1 && dropIndex != dragStartIndex) {
				swapTabs(dragStartIndex, dropIndex);
			}
		}

		void onMousePress(MouseEvent* me) override {
			if (me->left()) 
				changeTab_(indexAt(me->x(), me->y()));
		}

	private:
		void removeTab_(int index) {
			if (index < 0 || index >= headerRow_->children().size())
				return;

			headerRow_->child(index)->close();
			content_->child(index)->close();
			
			if (index == currentIndex_) {
				if (index == 0) changeTab_(index + 1);
				else if (index == headerRow_->children().size() - 1) changeTab_(index - 1);
				else changeTab_(index);
			}
		}

		void changeTab_(int index) {
			if (index < 0 || index >= headerRow_->children().size())
				return;

			for (int i = 0; i < headerRow_->children().size(); i++) {
				content_->child(i)->setVisible(i == index ? true : false);
				content_->child(i)->setEnabled(i == index ? true : false);
			}
			currentIndex_ = index;

			if (onTabChanged.has_value())
				onTabChanged.value()(index);
		}

		int indexAt(int x, int y) {
			for (int i = 0; i < headerRow_->children().size(); i++)
				if (headerRow_->child(i)->rect().contains(x, y))
					return i;

			return -1;
		}
	};
}

#endif // UI_TAB