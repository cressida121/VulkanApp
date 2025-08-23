#ifndef C_WINDOW_H_
#define C_WINDOW_H_

#include <Windows.h>

#include <vector>
#include <string>
#include <functional>

class CWindow {
public:
	std::function<bool()> MainLoopProcedure;

	class IEventListener {
		friend class CWindow;
	private:
		virtual void OnClose() {};
		virtual void OnCreate() {};
		virtual void OnSizeChanged(const uint32_t width, const uint32_t height) {};
		virtual void OnDestroy() {};
		virtual void OnMoveEnter() {};
		virtual void OnMoveExit() {};
		virtual void OnPaint() {};
	};

	CWindow(const std::wstring title, const uint32_t width, const uint32_t height);
	bool AddEventListener(IEventListener* pListener);
	bool RemoveEventListener(IEventListener* pListener);
	HWND GetHandle() const { return m_windowHandle; };
	void Show(bool isVisible) const;
	bool RunMainLoop();

private:
	static LRESULT WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	HWND CreateSystemWindow(const std::wstring name, const uint32_t windowWidth, const uint32_t windowHeight);

	HWND m_windowHandle = NULL;
	static std::vector<std::pair<HWND, IEventListener*>> m_eventListeners;

};

#endif // !C_WINDOW_H_
