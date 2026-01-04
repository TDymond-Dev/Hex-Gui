#ifndef UI_COMBOBOX
#define UI_COMBOBOX

#include "util/widget.hpp"
#include "layout.hpp"
#include "label.hpp"

namespace ui {
	class ComboBox : public Label {
		std::vector<std::string> items_;
		size_t selectedIndex_ = 0;

		struct ComboBoxDropDown : public VLayout {
			ComboBox* cb_;

			ComboBoxDropDown(ComboBox* cb) : cb_(cb), VLayout() {}

			void onWindowLostFocus(WindowEvent* we) override {
				closeWindow();
			}

			void onMousePress(MouseEvent* e) {
				if (e->left()) {
					cb_->setSelectedIndex(cb_->indexAt(e->x(), e->y()));
					closeWindow();
				}
			}
		};

		std::unique_ptr<ComboBoxDropDown> dropdown_ = nullptr;
	public:
		ComboBox(std::vector<std::string> const& items) : Label(items[0]), items_(items) {
			dropdown_ = std::make_unique<ComboBoxDropDown>(this);

			for (auto& item : items_) {
				auto it = dropdown_->addChild<Label>(item);
			}
		}

		void setSelectedIndex(size_t index) {
			if (index < items_.size()) {
				selectedIndex_ = index;
				setText(items_[index]);
			}
		}

		void onMousePress(MouseEvent* e) {
			if (e->left() && dropdown_->window() == nullptr) {
				auto [wx, wy] = root()->window()->screenPos(x(), y() + h());
				dropdown_->makeWindow(wx, wy, w(), int(items_.size()) * 80);
				dropdown_->window()->show();
				dropdown_->window()->removeTitleBarAndButtons();
			}
		}
	private:
		int indexAt(int x, int y) {
			for (int i = 0; i < dropdown_->children().size(); i++) {
				if (dropdown_->child(i)->rect().contains(x, y))
					return i;
			}

			return -1;
		}
	};
}

#endif UI_COMBOBOX