#include <CWindow.h>

LRESULT CWindow::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	bool wasMsgProcessed = false;
	for (auto &listener : m_eventListeners) {
		wasMsgProcessed = true;
		if (listener.first == hWnd && listener.second != nullptr) {
			switch (Msg)
			{
			case WM_CREATE:
				listener.second->OnCreate();
				break;

			case WM_PAINT:
				listener.second->OnPaint();
				break;

			case WM_SIZING:
				break;

			case WM_DESTROY:
				listener.second->OnDestroy();
				break;

			case WM_ENTERSIZEMOVE:
				listener.second->OnMoveEnter();
				break;

			case WM_EXITSIZEMOVE:
				listener.second->OnMoveExit();
				break;

			case WM_CLOSE:
				listener.second->OnClose();
				break;

			case WM_SIZE:
				switch (wParam)
				{
				case SIZE_MAXIMIZED:
					listener.second->OnSizeChanged(LOWORD(lParam), HIWORD(lParam));
					break;
				case SIZE_MINIMIZED:
					listener.second->OnSizeChanged(0u, 0u);
					break;
				case SIZE_RESTORED:
					listener.second->OnSizeChanged(LOWORD(lParam), HIWORD(lParam));
					break;
				}

				break;

			default:
				wasMsgProcessed = false;
				break;
			}
		}
	}
	if (wasMsgProcessed) {
		return 0;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

HWND CWindow::CreateSystemWindow(const std::wstring name, const uint32_t windowWidth, const uint32_t windowHeight) {

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEXW wndClassExW = { 0u };
	const wchar_t windowClassName[] = L"DefaultWindowClass";

	wndClassExW.lpszClassName = windowClassName;
	wndClassExW.lpfnWndProc = WindowProc;
	wndClassExW.hInstance = hInstance;
	wndClassExW.hCursor = NULL;
	wndClassExW.hIcon = NULL;
	wndClassExW.hbrBackground = (HBRUSH)(COLOR_BTNTEXT);
	wndClassExW.lpszMenuName = NULL;
	wndClassExW.hIconSm = NULL;
	wndClassExW.style = NULL;
	wndClassExW.cbClsExtra = NULL;
	wndClassExW.cbWndExtra = NULL;
	wndClassExW.cbSize = sizeof(wndClassExW);

	RegisterClassExW(&wndClassExW);

	RECT clientArea = { 0 };
	clientArea.top = 0u;
	clientArea.left = 0u;
	clientArea.bottom = windowHeight;
	clientArea.right = windowWidth;

	DWORD windowStyle = WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

	AdjustWindowRectEx(&clientArea, windowStyle, TRUE, WS_EX_OVERLAPPEDWINDOW);

	HWND systemWindowHandle = CreateWindowExW(
		WS_EX_OVERLAPPEDWINDOW,
		windowClassName,
		name.c_str(),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	return systemWindowHandle;
}

CWindow::CWindow(const std::wstring title, const uint32_t width, const uint32_t height) 
	: m_windowHandle(CreateSystemWindow(title, width, height))
{

}

std::vector<std::pair<HWND, CWindow::IEventListener*>> CWindow::m_eventListeners;

bool CWindow::AddEventListener(IEventListener* pListener) {
	if (m_windowHandle && pListener) {
		m_eventListeners.push_back({ m_windowHandle, pListener });
		return true;
	}
	return false;
};

bool CWindow::RemoveEventListener(IEventListener* pListener) {
	bool wasRemoved = false;
	for (auto &listener : m_eventListeners) {
		if (listener.second == pListener) {
			listener.first = nullptr;
			listener.second = nullptr;
			wasRemoved = true;
		}
	}
	return wasRemoved;
}

void CWindow::Show(bool isVisible) const {
	ShowWindow(m_windowHandle, isVisible ? SW_SHOW : SW_HIDE);
}

bool CWindow::RunMainLoop() {
	if (!MainLoopProcedure)
		return false;
	do {
		MSG msg = { 0 };
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (MainLoopProcedure());
	return true;
}
