#include <iostream>
#include <Application.h>
#include <CWindow.h>

int main() {

	CWindow mainWindow(L"VulkanApp", 700, 500);
	try
	{
		VulkanApp::Application vulkanApp(mainWindow.GetHandle());
		mainWindow.MainLoopProcedure = std::bind(&VulkanApp::Application::RenderFrame, &vulkanApp);
		mainWindow.Show(true);
		mainWindow.RunMainLoop();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what();
	}

	return 0;
}