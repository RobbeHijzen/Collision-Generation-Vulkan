#include "VulkanBase/VulkanBase.h"

int main() 
{
    VulkanBase vulkanApp{};
    
    try 
    {
    	vulkanApp.Run();
    }
    catch (const std::exception& execption) 
    {
    	std::cerr << execption.what() << "\n";
    	return EXIT_FAILURE;
    }

    return 0;
}



