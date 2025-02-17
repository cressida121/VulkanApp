#include <iostream>
#include <Application.h>

int main() {

	try
	{
		VulkanApp::Application application(800u, 600u);
		application.Run();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() <<std::endl;
	}

	std::cin.get();

	return 0;
}