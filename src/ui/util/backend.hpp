#ifndef UI_BACKEND
#define UI_BACKEND

#include "native.hpp"
#include "animation.hpp"
#include "canvas.hpp"
#include "event.hpp"
#include "widget.hpp"

#include <set>

namespace ui {
	/*
		Modal
		Dragging
		Focus
		Events
	*/
	class Backend {
		std::unique_ptr<ui::Canvas> canvas_;

		std::optional<int> FocusWidgetID;
		std::optional<int> DragStartWidgetID;
		std::optional<int> ModalWidgetID;

		std::queue<WindowMessage> messages;

		std::map<size_t, std::unique_ptr<Native>> windows;
		std::map<size_t, Widget*> widgets;
		std::map<size_t, std::unique_ptr<Canvas>> canvases;

		std::set<int> windowsToErase;
		std::set<Widget*> widgetsToErase;
	public:
		Backend(std::string const& asset_str, std::string font_name, int fSize) {
			canvas_ = std::make_unique<ui::Canvas>(asset_str, font_name, fSize);

			onMessage = [&](WindowMessage const& wm) {
				messages.emplace(wm);
				
				if (wm.msg == WM_SETFOCUS && windows[wm.wID]->type() == fPersistent) {
					for (auto& [id, widget] : widgets) {
						if (windows[id]->type() == fTransient) {
							windowsToErase.insert(id);
						}
					}
				}
			};

			onEvent = [&](Event* e) {
				std::cout << e->description() << std::endl;
				for (auto& [id, widget] : widgets) 
					if (e->windowID() == id) widget->event(e);
			};

			Widget::onMakeTopLevel = [&, a = asset_str, fn = font_name, fs = fSize](Widget* w, int x, int y, int w_, int h_, eNativeType type) {
				windows[w->id()] = std::make_unique<Native>(w->id(), x, y, w_, h_, type);
				widgets[w->id()] = w;
				canvases[w->id()] = std::make_unique<Canvas>(a, fn, fs);
				setMouseTracking(true, windows[w->id()]->hwnd());
			};

			Widget::onRemoveTopLevel = [&](int id) {
				windowsToErase.insert(id);
			};

			Widget::onGetWindow = [&](int id) -> Native* {
				return windows[id].get();
				};

			Widget::onCloseWidget = [&](Widget* w) {
				widgetsToErase.insert(w);
			};
		}

		bool update(float dt) {
			// Event Handling
			while (!messages.empty()) {
				auto& wm = messages.front();
				RawMsgToEvent(wm.wID, wm.hwnd, wm.msg, wm.wp, wm.lp);
				messages.pop();
			}
			
			// Event Polling
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// Animations
			Animate::Update(dt);

			// Layout
			for (auto& [id, widget] : widgets) {
				widget->layout(0, 0, windows[id]->width(), windows[id]->height());
			}

			for (auto& [id, widget] : widgets) {
				widget->postLayout();
			}
			
			// Extracting Paint Data
			for (auto& [id, widget] : widgets) {
				widget->paint(canvases[id].get());
			}
			

			// Deleteing Windows
			for (auto id : windowsToErase) {
				windows.erase(id);
				widgets.erase(id);
				canvases.erase(id);
			}
			windowsToErase.clear();

			// Deleting Widgets
			for (auto w : widgetsToErase) {
				deleteWidget(w);
			}
			widgetsToErase.clear();

			return true;
		}

		ui::Canvas* canvas() {
			return canvas_.get();
		}

		void setMouseTracking(bool enable, HWND hwnd, UINT hoverTimeMs = HOVER_DEFAULT) {
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };

			if (enable) {
				tme.dwFlags = TME_HOVER | TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = hoverTimeMs; // <-- THIS is the real hover time control
			}
			else {
				tme.dwFlags = TME_CANCEL;
				tme.hwndTrack = hwnd;
			}

			TrackMouseEvent(&tme);
		}

		void setMouseDoubleClickTime(UINT milliseconds) {
			// Win32 clamps internally if needed
			SetDoubleClickTime(milliseconds);
		}

		void setProcessDpiAware() {
			// Try modern API first
			SetProcessDPIAware();	
		}

		std::vector<std::tuple<Canvas*, int, int, HDC>> extractCanvases() {
			std::vector<std::tuple<Canvas*, int, int, HDC>> result;
			for (auto& [id, canvas] : canvases) {
				result.emplace_back(canvas.get(), windows[id]->width(), windows[id]->height(), windows[id]->hdc());
			}
			return result;
		}

	private:
		void deleteWidget(Widget* w) {
			if (w->parent() != nullptr) {
				auto& siblings = w->parent()->children();
				siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
					[&](const std::unique_ptr<Widget>& child) { return child.get() == w; }), siblings.end());
			}
		}
	};
}


#endif UI_BACKEND