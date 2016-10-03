#include "ZVK_Application.h"
#include <vector>

ZVK_Application::ZVK_Application()
{
	// Use validation layers if this is a debug build
	std::vector<const char*> layers;
#if defined(_DEBUG)
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	app_info = vk::ApplicationInfo()
		.setPApplicationName("Vulkan C++ Program Template")
		.setApplicationVersion(1)
		.setPEngineName("LunarG SDK")
		.setEngineVersion(1)
		.setApiVersion(VK_API_VERSION_1_0);

	inst_info = vk::InstanceCreateInfo()
		.setFlags(vk::InstanceCreateFlags())
		.setPApplicationInfo(&app_info)
		.setEnabledExtensionCount(0)
		.setPpEnabledExtensionNames(NULL)
		.setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
		.setPpEnabledLayerNames(layers.data());
	
	instance = vk::createInstance(inst_info);
}


ZVK_Application::~ZVK_Application()
{
	instance.destroy();
}
