#ifndef ZVK_Application_h
#define ZVK_Application_h

#include <vulkan/vulkan.hpp>

class ZVK_Application
{
public:
	ZVK_Application();
	~ZVK_Application();
private:
	vk::ApplicationInfo app_info;
	vk::InstanceCreateInfo inst_info;

	vk::Instance instance;
};

#endif // !ZVK_Application_h

