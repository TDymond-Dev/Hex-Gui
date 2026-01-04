#ifndef UI_WINDOW
#define UI_WINDOW

#include <Windows.h>
#include <string>
#include <functional>
#include <array>
#include <memory>
#include <queue>
#include <optional>

namespace ui {
	struct PixelFormatBase {
		PIXELFORMATDESCRIPTOR pfd{};

		PixelFormatBase() {
			ZeroMemory(&pfd, sizeof(pfd));
			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 24;
			pfd.cStencilBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;
		}

		explicit PixelFormatBase(HDC hdc) : PixelFormatBase() { set(hdc); }

		void set(HDC hdc) {
			int format = ChoosePixelFormat(hdc, &pfd);
			if (format == 0 || !SetPixelFormat(hdc, format, &pfd)) {
				MessageBox(nullptr, L"Failed to set base pixel format", L"Error", MB_ICONEXCLAMATION | MB_OK);
			}
		}

		void erase(HWND hwnd) {
			if (HGLRC hglrc = wglGetCurrentContext()) {
				wglMakeCurrent(nullptr, nullptr);
				wglDeleteContext(hglrc);
			}
			if (HDC hdc = GetDC(hwnd)) {
				ReleaseDC(hwnd, hdc);
			}
		}
	};

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	enum eNativeType {
		fPersistent,
		fTransient
	};

	class Native {
		int id_;
		HDC hdc_;
		HWND hwnd_;
		WNDCLASSEX class_;
		eNativeType type_ = fPersistent;
		static inline std::optional<PixelFormatBase> pfd;
	public:
		Native(int id, int x, int y, int w, int h, eNativeType nType) : type_(nType), id_(id) {
			create_window(x, y, w, h);

			if (!pfd.has_value()) {
				pfd = PixelFormatBase();
			}

			// Set the pixel format
			hdc_ = GetDC(hwnd_);
			pfd->set(hdc_);
		}

		~Native() {
			ReleaseDC(hwnd_, hdc_);
			DestroyWindow(hwnd_);
		}

		void removeTitleBarAndButtons() {
			LONG_PTR style = GetWindowLongPtr(hwnd_, GWL_STYLE);
			style &= ~(WS_CAPTION | WS_SYSMENU);

			SetWindowLongPtr(hwnd_, GWL_STYLE, style);
			SetWindowPos(hwnd_, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}

		void swapBuffers() { SwapBuffers(hdc_); }
		void show() { ShowWindow(hwnd_, SW_SHOW); }
		void hide() { ShowWindow(hwnd_, SW_HIDE); }
		void minimize() { ShowWindow(hwnd_, SW_MINIMIZE); }
		void maximize() { ShowWindow(hwnd_, SW_MAXIMIZE); }
		void restore() { ShowWindow(hwnd_, SW_RESTORE); }

		// SW_SHOW, SW_HIDE, SW_MINIMIZE, SW_MAXIMIZE, SW_RESTORE
		void setState(int state) { ShowWindow(hwnd_, state); }
		void setTitle(std::string const& title) { SetWindowText(hwnd_, std::wstring{ title.begin(), title.end() }.c_str()); }
		void setX(int x) { SetWindowPos(hwnd_, NULL, x, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
		void setY(int y) { SetWindowPos(hwnd_, NULL, 0, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
		void setPos(int x, int y) { SetWindowPos(hwnd_, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
		void setWidth(int w) { SetWindowPos(hwnd_, NULL, 0, 0, w, 0, SWP_NOMOVE | SWP_NOZORDER); }
		void setHeight(int h) { SetWindowPos(hwnd_, NULL, 0, 0, 0, h, SWP_NOMOVE | SWP_NOZORDER); }
		void setSize(int w, int h) { SetWindowPos(hwnd_, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }
		void setEnabled(bool enabled) { EnableWindow(hwnd_, enabled); }
		void setFocus() {
			SetForegroundWindow(hwnd_);
		}
		void setWindowType(eNativeType type) { type_ = type; }

		// Getters
		std::array<int, 2> mousePos() const { POINT point; GetCursorPos(&point); return { (uint16_t)point.x, (uint16_t)point.y }; }
		std::array<int, 2> windowPos() const { RECT rect; GetClientRect(hwnd_, &rect); return { (uint16_t)rect.left, (uint16_t)rect.top }; }
		std::array<int, 2> windowSize() const { RECT rect; GetClientRect(hwnd_, &rect); return { uint16_t(rect.right - rect.left), uint16_t(rect.bottom - rect.top) }; }
		std::array<int, 2> screenPos(int x, int y) const { POINT point = { x, y }; ClientToScreen(hwnd_, &point); return { (uint16_t)point.x, (uint16_t)point.y }; }
		int x() const { RECT rect; GetClientRect(hwnd_, &rect); return rect.left; }
		int y() const { RECT rect; GetClientRect(hwnd_, &rect); return rect.top; }
		int width() const { RECT rect; GetClientRect(hwnd_, &rect); return rect.right - rect.left; }
		int height() const { RECT rect; GetClientRect(hwnd_, &rect); return rect.bottom - rect.top; }
		std::array<int, 4>rect() const { RECT rect; GetClientRect(hwnd_, &rect); return { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top }; }
		int id() const { return id_; }
		eNativeType type() const { return type_; }
		
		HWND hwnd() const { return hwnd_; }
		HDC hdc() const { return hdc_; }
	private:
		void create_window(int x, int y, int w, int h) {
			std::string className = "EngineGameWindow_" + std::to_string(id_);
			std::wstring wide_className{ className.begin(), className.end() };

			auto DoesWindowClassExist = [](LPCTSTR className, HINSTANCE hInstance) -> bool {
				WNDCLASSEX wc; return GetClassInfoEx(hInstance, className, &wc) != 0;
				};

			if (!DoesWindowClassExist(wide_className.c_str(), GetModuleHandle(NULL))) {
				class_ = {};
				class_.cbSize = sizeof(WNDCLASSEX); //the size of the structure
				class_.style = CS_DBLCLKS; //Style of the class
				class_.lpfnWndProc = WndProc; // function pointer to the callback function
				class_.cbClsExtra = 0;
				class_.cbWndExtra = 0;
				class_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
				class_.lpszClassName = wide_className.c_str();
				class_.lpszMenuName = NULL;
				class_.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
				class_.hInstance = GetModuleHandle(NULL);

				if (!RegisterClassEx(&class_)) {
					MessageBox(NULL, L"Sorry could not register window class", L"Window Registration Error", MB_ICONEXCLAMATION | MB_OK);
				}
			}

			DWORD style = WS_OVERLAPPEDWINDOW | CS_OWNDC;
			hwnd_ = CreateWindowEx((DWORD)0, wide_className.c_str(), L"Title", style, (int)x, (int)y, (int)w, (int)h, NULL, NULL, class_.hInstance, nullptr);

			if (hwnd_ == NULL) {
				MessageBox(hwnd_, L"Window not created", L"Window Creation Error", MB_ICONEXCLAMATION | MB_OK);
			}

			SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

			hdc_ = GetDC(hwnd_);
		}
	};

	static void wMsg() {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	struct WindowMessage {
		int wID;
		HWND hwnd;
		UINT msg;
		WPARAM wp;
		LPARAM lp;
	};

	std::optional<std::function<void(WindowMessage const&)>> onMessage;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		Native* window = reinterpret_cast<Native*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (window != nullptr) {
			if (onMessage) 
				onMessage.value()({ window->id(), hwnd, msg, wp, lp });
		}

		return DefWindowProc(hwnd, msg, wp, lp);
	}
}

#endif // UI_WINDOW
