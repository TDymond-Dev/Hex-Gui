#ifndef UI_MENU
#define UI_MENU

#include "label.hpp"
#include "layout.hpp"

namespace ui {
	class SubMenu : public VLayout {
		int persistentWindowID;
	public:
		SubMenu(int id) : persistentWindowID(id) {}
		int getPersistentWindowID() { return persistentWindowID; }
	};

	class MenuItem : public Label {
		std::function<void(std::string)> onSelect_;
	public:
		MenuItem(const std::string& text, std::function<void(std::string)> f) : onSelect_(f), Label(text) { setFixedH(50); }

		void onMousePress(MouseEvent* me) override {
			if (onSelect_ && me->left()) onSelect_(text());

			root()->window(static_cast<SubMenu*>(root())->getPersistentWindowID())->setFocus();
		}
	};

	class MenuDivider : public Widget {
	public:
		MenuDivider() : Widget() {
			setFixedH(10);
		}

		void onPaint(Canvas* c) override {
			c->solid(col.darkGray);
			c->rect(x(), y(), w(), h());
		}
	};

	class Menu : public Label {
		

		std::unique_ptr<SubMenu> submenu;
	public:
		Menu(const std::string& text, int pID) : submenu(std::make_unique<SubMenu>(pID)), Label(text) {
			setFixedH(50); 
		};

		MenuItem* addItem(const std::string& text, std::function<void(std::string)> onSelect) {
			return submenu->addChild<MenuItem>(text, onSelect);
		}

		void addDivider() {
			auto divider = submenu->addChild<MenuDivider>();
		}

		Menu* addSubmenu(const std::string& text) {
			return submenu->addChild<Menu>(text, submenu->getPersistentWindowID());
		}

		void onMousePress(MouseEvent* me) override {
			if (submenu->window() == nullptr) {
				auto [wx, wy] = root()->window()->screenPos(x() + w(), y());
				submenu->makeWindow(wx, wy, w(), 50 * submenu->children().size(), fTransient);
				submenu->window()->show();
				submenu->window()->removeTitleBarAndButtons();
			}
		}
	};
}


#endif UI_MENU