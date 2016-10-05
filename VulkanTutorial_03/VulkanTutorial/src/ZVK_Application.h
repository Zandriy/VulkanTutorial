/* ZVK_Application.h
 * VulkanTutorial project
 *
 * Created by Andriy Zhabura on 02-Oct-2016.
 * Last modified on 05-Oct-2016.
 */

/*
 * Copyright © 2016 Andriy Zhabura
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */
 
#ifndef ZVK_Application_h
#define ZVK_Application_h

#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>

class ZVK_Application
{
public:
	ZVK_Application();
	~ZVK_Application();

	std::vector<std::string> get_LayerPropertiesNames();
	std::vector<std::string> get_ExtensionPropertiesNames();
	size_t get_PhysicalDevicesQty();

	bool create_LogicalDevice();
private:
	vk::ApplicationInfo app_info{};
	vk::InstanceCreateInfo inst_info{};

	vk::Instance instance{};

	std::vector<vk::LayerProperties> layer_properties;
	std::vector<std::vector<vk::ExtensionProperties>> extention_properties;
	std::vector<vk::PhysicalDevice> gpus;
	
	vk::Result get_LayerProperties();
	vk::Result get_ExtensionProperties();
	vk::Result get_PhysicalDevices();

	vk::DeviceQueueCreateInfo device_queue_info{};
	vk::DeviceCreateInfo device_info{};
	vk::Device logical_device{};
};

#endif // !ZVK_Application_h

