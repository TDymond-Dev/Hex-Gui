#ifndef UI_EVENT
#define UI_EVENT

#include "misc.hpp"

#include <memory>

namespace ui {
	enum eCategory {
		fMouse,
		fKeyboard,
		fWindow,
		fDrag,
	};

	enum eAction {
		fPress,
		fRelease,
		fDoublePress,
		fMove,
		fWheelUp,
		fWheelDown,

		fClose,
		fResize,
		fGainedFocus,
		fLostFocus,
		fMinimize,
		fMaximize,
		fRestore,
		fHover,
		fEnter,
		fLeave,
		fStart,
		fEnd,
		fInactive,
		fActivated,
		fClickActivate,
		fHide,
		fShow,
	};

	enum eButton {
		fNoButton,
		fLeft,
		fRight,
		fMiddle,
		fKey
	};

	class Event {
		eCategory catergory_;
		eAction action_;
		eButton button_;
		std::array<int, 2> mPos;
		bool ignored_ = false;
		std::string desc_;
		int windowID_ = -1;
	protected:
		Event(std::string desc, eCategory category, eAction action, eButton button)
			: desc_(desc), catergory_(category), action_(action), button_(button) {}

	public:
		// setters
		void setPos(int x, int y) {
			mPos = { x, y };
		}

		void setIgnored(bool b) {
			ignored_ = b;
		}

		void setWindowID(int id) {
			windowID_ = id;
		}

		// getters
		eCategory category() const { return catergory_; }
		eAction action() const { return action_; }
		eButton button() const { return button_; }
		int x() const { return mPos[0]; }
		int y() const { return mPos[1]; }
		bool ignored() { return ignored_; }
		std::string description() const { return desc_ + " WID: " + std::to_string(windowID_) + "at " + std::to_string(x()) + ", " + std::to_string(y()); }
		int windowID() const { return windowID_; }

		bool alt() const { return (GetKeyState(VK_MENU) & 0x8000) != 0; }
		bool ctrl() const { return (GetKeyState(VK_CONTROL) & 0x8000) != 0; }
		bool shift() const { return (GetKeyState(VK_SHIFT) & 0x8000) != 0; }
	};

	class MouseEvent : public Event {
	public:
		MouseEvent(eButton btn, eAction act) : Event("Mouse Event", fMouse, act, btn) {}

		bool left() const { return button() == fLeft; }
		bool right() const { return button() == fRight; }
		bool middle() const { return button() == fMiddle; }

		bool press() const { return action() == fPress; }
		bool release() const { return action() == fRelease; }
		bool move() const { return action() == fMove; }
		bool doublePress() const { return action() == fDoublePress; }
		bool wheelUp() const { return action() == fWheelUp; }
		bool wheelDown() const { return action() == fWheelDown; }
	};

	class DragEvent : public Event {
	public:
		DragEvent(eButton b, eAction a) : Event("Drag Event", fDrag, a, b) {}

		bool start() const { return action() == fStart; }
		bool end() const { return action() == fEnd; }
		bool move() const { return action() == fMove; }
		
		bool left() const { return button() == fLeft; }
		bool right() const { return button() == fRight; }
		bool middle() const { return button() == fMiddle; }
	};

	class KeyEvent : public Event {
		char key;
	public:
		KeyEvent(eAction a, char c) : key(c), Event("Key Event", fKeyboard, a, fKey) {}

		bool press() const { return action() == fPress; }
		bool release() const { return action() == fRelease; }
		char getKey() const { return key; }
	};

	

	class WindowEvent : public Event {
	public:
		WindowEvent(eAction a) : Event("Window Event", fWindow, a, fKey) {}

		bool close() const { return action() == fClose; }
		bool resize() const { return action() == fResize; }
		bool gainedFocus() const { return action() == fGainedFocus; }
		bool lostFocus() const { return action() == fLostFocus; }
		bool minimize() const { return action() == fMinimize; }
		bool maximize() const { return action() == fMaximize; }
		bool restore() const { return action() == fRestore; }
		bool hover() const { return action() == fHover; }
		bool enter() const { return action() == fEnter; }
		bool leave() const { return action() == fLeave; }
		bool hide() const { return action() == fHide; }
		bool show() const { return action() == fShow; }
	};

	std::optional<std::function<void(Event* e)>> onEvent;

	static void RawMsgToEvent(int windowID, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		std::unique_ptr<Event> e;

		static eButton button = fNoButton;
		static std::array<int, 2> mousePos = {};
		static std::array<int, 2> mouseDownPos;
		static bool dragStarted = false;

		switch (msg) {
		case WM_KEYDOWN:
			e = std::make_unique<KeyEvent>(fPress, static_cast<char>(wp));
			break;
		case WM_KEYUP:
			e = std::make_unique<KeyEvent>(fRelease, static_cast<char>(wp));
			break;
		case WM_LBUTTONDOWN:
			e = std::make_unique<MouseEvent>(fLeft, fPress);
			mouseDownPos = { LOWORD(lp), HIWORD(lp) }, button = fLeft;
			break;
		case WM_MBUTTONDOWN:
			e = std::make_unique<MouseEvent>(fMiddle, fPress);
			mouseDownPos = { LOWORD(lp), HIWORD(lp) }, button = fMiddle;
			break;
		case WM_RBUTTONDOWN:
			e = std::make_unique<MouseEvent>(fRight, fPress);
			mouseDownPos = { LOWORD(lp), HIWORD(lp) }, button = fRight;
			break;
		case WM_LBUTTONUP:
			e = std::make_unique<MouseEvent>(fLeft, fRelease);
			button = fNoButton, dragStarted = false;
			break;
		case WM_MBUTTONUP:
			e = std::make_unique<MouseEvent>(fMiddle, fRelease);
			button = fNoButton, dragStarted = false;
			break;
		case WM_RBUTTONUP:
			e = std::make_unique<MouseEvent>(fRight, fRelease);
			button = fNoButton, dragStarted = false;
			break;
		case WM_MOUSEWHEEL:
			e = std::make_unique<MouseEvent>(fNoButton, GET_WHEEL_DELTA_WPARAM(wp) > 0 ? fWheelUp : fWheelDown);
			break;
		case WM_MOUSEMOVE:
			if (button != fNoButton && !dragStarted) {
				dragStarted = true;
				e = std::make_unique<DragEvent>(button, fStart);
			}
			else if (button != fNoButton && dragStarted)
				e = std::make_unique<DragEvent>(button, fMove);
			else
				e = std::make_unique<MouseEvent>(button, fMove);
			mousePos = { LOWORD(lp), HIWORD(lp) };
			break;
		case WM_LBUTTONDBLCLK:
			e = std::make_unique<MouseEvent>(fLeft, fDoublePress);
			mouseDownPos = { LOWORD(lp), HIWORD(lp) };
			break;
		case WM_RBUTTONDBLCLK:
			e = std::make_unique<MouseEvent>(fRight, fDoublePress);
			mouseDownPos = { LOWORD(lp), HIWORD(lp) };
			break;
		case WM_MBUTTONDBLCLK:
			e = std::make_unique<MouseEvent>(fMiddle, fDoublePress);
			mouseDownPos = { LOWORD(lp), HIWORD(lp) };
			break;
		case WM_MOUSEHOVER:
			e = std::make_unique<WindowEvent>(fHover);
			break;
		case WM_MOUSELEAVE:
			e = std::make_unique<WindowEvent>(fLeave);
			break;
		case WM_CLOSE:
			e = std::make_unique<WindowEvent>(fClose);
			break;
		case WM_SIZE:
			if (wp == SIZE_MAXIMIZED) e = std::make_unique<WindowEvent>(fMaximize);
			if (wp == SIZE_MINIMIZED) e = std::make_unique<WindowEvent>(fMinimize);
			if (wp == SIZE_RESTORED) e = std::make_unique<WindowEvent>(fRestore);
			break;
		case WM_SETFOCUS:
			e = std::make_unique<WindowEvent>(fGainedFocus);
			break;
		case WM_KILLFOCUS:
			e = std::make_unique<WindowEvent>(fLostFocus);
			break;
		case WM_ACTIVATE:
			if (wp == WA_INACTIVE) e = std::make_unique<WindowEvent>(fInactive);
			if (wp == WA_ACTIVE) e = std::make_unique<WindowEvent>(fActivated);
			if (wp == WA_CLICKACTIVE) e = std::make_unique<WindowEvent>(fClickActivate);
			break;
		case WM_SHOWWINDOW:
			if (wp == TRUE) e = std::make_unique<WindowEvent>(fShow);
			if (wp == FALSE) e = std::make_unique<WindowEvent>(fHide);
			break;
		case WM_MOVING:
			e = std::make_unique<WindowEvent>(fMove);
			break;
		case WM_SIZING:
			e = std::make_unique<WindowEvent>(fResize);
			break;
		case WM_NCHITTEST:
			break;
		case WM_NCCALCSIZE:
			if (lp) {
				NCCALCSIZE_PARAMS* sz = reinterpret_cast<NCCALCSIZE_PARAMS*>(lp);
				//
				//// Add 8 pixel to the top border when maximized so the app isn't cut off
				//
				//	// on windows 10, if set to 0, there's a white line at the top
				//	// of the app and I've yet to find a way to remove that.
				sz->rgrc[0].top -= 200;
				sz->rgrc[0].right -= 200;
				sz->rgrc[0].bottom -= 200;
				sz->rgrc[0].left += 200;

				// Previously (WVR_HREDRAW | WVR_VREDRAW), but returning 0 or 1 doesn't
				// actually break anything so I've set it to 0. Unless someone pointed a
				// problem in the future.
				//return (WVR_HREDRAW | WVR_VREDRAW);
				UpdateWindow(hwnd);
				//return DefWindowProc(hwnd, msg, wp, lp);
			}
			break;
		case WM_NCACTIVATE:
			break;
		default:
			break;
		}

		if (e && onEvent.has_value()) {
			e->setPos(mousePos[0], mousePos[1]);
			e->setWindowID(windowID);
			onEvent.value()(e.get());	
		}
	}
}


#endif UI_EVENT