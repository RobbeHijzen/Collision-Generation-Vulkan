#include "vulkanbase/VulkanBase.h"

int main() {
	// DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
	//DISABLE_LAYER_NV_OPTIMUS_1 = 1
	//_putenv_s("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
	//_putenv_s("DISABLE_LAYER_NV_OPTIMUS_1", "1");
	VulkanBase vulkanApp;

	try 
	{
		vulkanApp.run();
	}
	catch (const std::exception& execption) 
	{
		std::cerr << execption.what() << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}