/* ZVK_Application.cpp
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
 
#include "ZVK_Application.h"
#include <stdexcept>

ZVK_Application::ZVK_Application()
{
	if (vk::Result::eSuccess != get_LayerProperties())
	{
		throw std::domain_error{"Problem while getting Layer Properties"};
	}
	if (vk::Result::eSuccess != get_ExtensionProperties())
	{
		throw std::domain_error{ "Problem while getting Extension Properties" };
	}

	// Use validation layers if this is a debug build
	std::vector<const char*> layers;
#if defined(_DEBUG)
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	app_info = vk::ApplicationInfo()
		.setPApplicationName("Vulkan C++ Tutorial")
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

	if (!instance)
	{
		throw std::bad_alloc{};
	}
	
	if (vk::Result::eSuccess != get_PhysicalDevices())
	{
		throw std::domain_error{"Physical Devices are not found"};
	}
}

ZVK_Application::~ZVK_Application()
{
	if (logical_device)
	{
		logical_device.destroy();
	}
	instance.destroy();
}

vk::Result ZVK_Application::get_LayerProperties()
{
	uint32_t count = 0;
	vk::Result res;
	do
	{
		res = vk::enumerateInstanceLayerProperties(&count, nullptr);
		if (vk::Result::eSuccess != res)
		{
			return res;
		}
		if (!count)
		{
			return vk::Result::eSuccess;
		}
		layer_properties.resize(count);
		res = vk::enumerateInstanceLayerProperties(&count, layer_properties.data());

		// it's possible, that while first and second call of enumerateInstanceLayerProperties
		// the number of instance layers could change. In this case the eIncomplete value
		// will be returned.
	} while (vk::Result::eIncomplete == res);

	return res;
}

vk::Result ZVK_Application::get_ExtensionProperties()
{
	uint32_t count = 0;
	vk::Result res;

	if (!layer_properties.size())
	{
		return vk::Result::eSuccess;
	}

	extention_properties.resize(layer_properties.size());

	for (size_t i = 0; i < layer_properties.size(); ++i)
	{
		do {
			res = vk::enumerateInstanceExtensionProperties(
				layer_properties[i].layerName, &count, nullptr);
			if (vk::Result::eSuccess != res)
			{
				return res;
			}
			if (!count)
			{
				break;
			}

			extention_properties[i].resize(count);
			res = vk::enumerateInstanceExtensionProperties(
				layer_properties[i].layerName, &count, extention_properties[i].data());

			// it's possible, that while first and second call of enumerateInstanceExtensionProperties
			// the number of instance properties could change. In this case the eIncomplete value
			// will be returned.
		} while (vk::Result::eIncomplete == res);
	}

	return res;
}

vk::Result ZVK_Application::get_PhysicalDevices()
{
	uint32_t count = 0;
	vk::Result res = instance.enumeratePhysicalDevices(&count, nullptr);
	if (vk::Result::eSuccess != res)
	{
		return res;
	}
	if (!count)
	{
		return vk::Result::eErrorDeviceLost;
	}

	gpus.resize(count);
	res = instance.enumeratePhysicalDevices(&count, gpus.data());

	return res;
}

std::vector<std::string> ZVK_Application::get_LayerPropertiesNames()
{
	std::vector<std::string> result;
	for (auto a : layer_properties)
	{
		result.push_back(a.layerName);
	}
	return std::move(result);
}

std::vector<std::string> ZVK_Application::get_ExtensionPropertiesNames()
{
	std::vector<std::string> result;
	std::string extentions;
	for (size_t i = 0; i < layer_properties.size(); ++i)
	{
		extentions.clear();
		for (auto e : extention_properties[i])
		{
			extentions += e.extensionName;
			extentions += ", ";
		}
		if (extentions.size())
		{
			extentions.resize(extentions.size() - 2);
			result.push_back(extentions + " for " + layer_properties[i].layerName);
		}
	}
	return std::move(result);
}

size_t ZVK_Application::get_PhysicalDevicesQty()
{
	return gpus.size();
}

void ZVK_Application::create_LogicalDevice()
{
	std::vector<vk::QueueFamilyProperties> family_properties = gpus[0].getQueueFamilyProperties();

	if (!family_properties.size())
	{
		throw std::domain_error{ "GPU has no Queue Family" };
	}
	
	vk::DeviceQueueCreateInfo device_queue_info{};

	bool found = false;
	for (unsigned int i = 0; i < family_properties.size(); ++i) {
		if (family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			device_queue_info.queueFamilyIndex = i;
			found = true;
			break;
		}
	}

	if (!found)
	{
		throw std::domain_error{ "GPU has no Graphics Queue Family" };
	}

	float queue_priorities[1] = { 0.0 };
	device_queue_info.pNext = nullptr;
	device_queue_info.queueCount = 1;
	device_queue_info.pQueuePriorities = queue_priorities;

	vk::DeviceCreateInfo device_info{};
	device_info.pNext = nullptr;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &device_queue_info;
	device_info.enabledExtensionCount = 0;
	device_info.ppEnabledExtensionNames = nullptr;
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = nullptr;
	device_info.pEnabledFeatures = nullptr;

	logical_device = gpus[0].createDevice(device_info);
}