/* ZVK_Application.cpp
 * VulkanTutorial project
 *
 * Created by Andriy Zhabura on 02-Oct-2016.
 * Last modified on 08-Oct-2016.
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
#include <limits>

ZVK_Application::ZVK_Application()
    : window(L"Vulkan Tutorial", 800, 600)
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
	
	std::vector<const char*> extentions;
	extentions.push_back("VK_KHR_surface");
	extentions.push_back("VK_KHR_win32_surface");

	app_info = vk::ApplicationInfo()
		.setPApplicationName("Vulkan C++ Tutorial")
		.setApplicationVersion(1)
		.setPEngineName("LunarG SDK")
		.setEngineVersion(1)
		.setApiVersion(VK_API_VERSION_1_0);

	inst_info = vk::InstanceCreateInfo()
		.setFlags(vk::InstanceCreateFlags())
		.setPApplicationInfo(&app_info)
		.setEnabledExtensionCount(static_cast<uint32_t>(extentions.size()))
		.setPpEnabledExtensionNames(extentions.data())
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
    if (image_views.size())
    {
        for (auto iv : image_views)
        {
            logical_device.destroyImageView(iv);
        }
    }
    if (swap_chain)
    {
        logical_device.destroySwapchainKHR(swap_chain);
    }
    if (surface)
    {
        instance.destroySurfaceKHR(surface);
    }
	if (command_buffers.size())
	{
		logical_device.freeCommandBuffers(cmd_pool, command_buffers);
	}
	if (cmd_pool)
	{
		logical_device.destroyCommandPool(cmd_pool);
	}
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

bool ZVK_Application::create_LogicalDevice()
{
    create_Surface();
    fill_device_queue_info();
	
	std::vector<const char*> extentions;
	extentions.push_back("VK_KHR_swapchain");

	float queue_priorities[1] = { 0.0 };
	device_queue_info.pNext = nullptr;
	device_queue_info.queueCount = 1;
	device_queue_info.pQueuePriorities = queue_priorities;

	device_info.pNext = nullptr;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &device_queue_info;
	device_info.enabledExtensionCount = static_cast<uint32_t>(extentions.size());
	device_info.ppEnabledExtensionNames = extentions.data();
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = nullptr;
	device_info.pEnabledFeatures = nullptr;

	logical_device = gpus[0].createDevice(device_info);

	return (logical_device ? true : false);
}

bool ZVK_Application::allocate_CommandBuffers()
{
	vk::CommandPoolCreateInfo cmd_pool_info{};
	cmd_pool_info.pNext = nullptr;
	cmd_pool_info.queueFamilyIndex = device_info.pQueueCreateInfos->queueFamilyIndex;

	cmd_pool = logical_device.createCommandPool(cmd_pool_info);

	if (!cmd_pool)
	{
		throw std::domain_error{ "CommandPool was not created" };
	}

	vk::CommandBufferAllocateInfo cmd{};
	cmd.pNext = nullptr;
	cmd.commandPool = cmd_pool;
	cmd.level = vk::CommandBufferLevel::ePrimary;
	cmd.commandBufferCount = 1;

	command_buffers = logical_device.allocateCommandBuffers(cmd);
	if (!command_buffers.size())
	{
		throw std::domain_error{ "Command Buffers were not created" };
	}

	return true;
}

void ZVK_Application::create_Surface()
{
    vk::Win32SurfaceCreateInfoKHR info{};
    info.pNext = nullptr;
    info.hinstance = window.module();
    info.hwnd = window.handle();

    surface = instance.createWin32SurfaceKHR(info);

    if (!surface)
    {
        throw std::domain_error{ "Vulkan surface was not created" };
    }
}

void ZVK_Application::fill_device_queue_info()
{
    std::vector<vk::QueueFamilyProperties> family_properties = gpus[0].getQueueFamilyProperties();

    if (!family_properties.size())
    {
        throw std::domain_error{ "GPU has no Queue Family" };
    }

    // Iterate over each queue to learn whether it supports presenting
    std::vector<vk::Bool32> support_present(family_properties.size());
    for (uint32_t i = 0; i < family_properties.size(); ++i) {
        gpus[0].getSurfaceSupportKHR(i, surface, &support_present[i]);
    }

    // Search for a queue family supported present
    present_family_index = UINT32_MAX;
    for (uint32_t i = 0; i < family_properties.size(); ++i)
    {
        if (support_present[i] == VK_TRUE) {
            present_family_index = i;
            break;
        }
    }

    if (UINT32_MAX == present_family_index)
    {
        throw std::domain_error{ "GPU has no Present Queue Family" };
    }

    // Search for a queue family supported both graphics and present
    device_queue_info.queueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < family_properties.size(); ++i) {
        if (family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            device_queue_info.queueFamilyIndex = i;
            if (support_present[i] == VK_TRUE) {
                present_family_index = i;
                break;
            }
        }
    }

    if (UINT32_MAX == device_queue_info.queueFamilyIndex)
    {
        throw std::domain_error{ "GPU has no Graphics Queue Family" };
    }
}

bool ZVK_Application::create_Swapchain()
{
    check_SurfaceFormat();

    vk::SurfaceCapabilitiesKHR surface_capabilities{};
    if (vk::Result::eSuccess != gpus[0].getSurfaceCapabilitiesKHR(surface, &surface_capabilities))
    {
        throw std::domain_error{ "No Surface Capabilities found" };
    }

    std::vector<vk::PresentModeKHR> present_modes = gpus[0].getSurfacePresentModesKHR(surface);
    if (!present_modes.size())
    {
        throw std::domain_error{ "No Surface Present Modes found" };
    }

    vk::Extent2D swapchain_extent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchain_extent.width = window.width();
        swapchain_extent.height = window.height();
        if (swapchain_extent.width < surface_capabilities.minImageExtent.width) {
            swapchain_extent.width = surface_capabilities.minImageExtent.width;
        }
        else if (swapchain_extent.width >
            surface_capabilities.maxImageExtent.width) {
            swapchain_extent.width = surface_capabilities.maxImageExtent.width;
        }

        if (swapchain_extent.height < surface_capabilities.minImageExtent.height) {
            swapchain_extent.height = surface_capabilities.minImageExtent.height;
        }
        else if (swapchain_extent.height >
            surface_capabilities.maxImageExtent.height) {
            swapchain_extent.height = surface_capabilities.maxImageExtent.height;
        }
    }
    else {
        // If the surface size is defined, the swap chain size must match
        swapchain_extent = surface_capabilities.currentExtent;
    }

    // If mailbox mode is available, use it, as is the lowest-latency non-
    // tearing mode.  If not, try IMMEDIATE which will usually be available,
    // and is fastest (though it tears).  If not, fall back to FIFO which is
    // always available.
    vk::PresentModeKHR swap_present_mode = vk::PresentModeKHR::eFifo;
    for (size_t i = 0; i < present_modes.size(); ++i) {
        if (present_modes[i] == vk::PresentModeKHR::eMailbox) {
            swap_present_mode = vk::PresentModeKHR::eMailbox;
            break;
        }
        if ((swap_present_mode != vk::PresentModeKHR::eMailbox) &&
            (present_modes[i] == vk::PresentModeKHR::eImmediate)) {
            swap_present_mode = vk::PresentModeKHR::eImmediate;
        }
    }

    vk::SwapchainCreateInfoKHR swapchain_ci{};
    swapchain_ci.pNext = nullptr;
    swapchain_ci.surface = surface;
    swapchain_ci.minImageCount = surface_capabilities.minImageCount;
    swapchain_ci.imageFormat = surface_format.format;
    swapchain_ci.imageExtent.width = swapchain_extent.width;
    swapchain_ci.imageExtent.height = swapchain_extent.height;
    swapchain_ci.preTransform = surface_capabilities.currentTransform;
    swapchain_ci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swap_present_mode;
    swapchain_ci.oldSwapchain = nullptr;
    swapchain_ci.clipped = true;
    swapchain_ci.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapchain_ci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchain_ci.imageSharingMode = vk::SharingMode::eExclusive;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = nullptr;
    std::vector<uint32_t> queueFamilyIndices{device_queue_info.queueFamilyIndex, present_family_index };
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between
        // the queues, or we have to create the swap chain with imageSharingMode
        // as eConcurrent
        swapchain_ci.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchain_ci.queueFamilyIndexCount = 2;
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    swap_chain = logical_device.createSwapchainKHR(swapchain_ci);

    return (swap_chain ? true : false);
}

void ZVK_Application::check_SurfaceFormat()
{
    std::vector<vk::SurfaceFormatKHR> formats = gpus[0].getSurfaceFormatsKHR(surface);

    if (!formats.size())
    {
        throw std::domain_error{ "No Surface Formats found" };
    }

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    bool no_preferred = (formats.size() == 1 && formats[0].format == vk::Format::eUndefined);
    
    surface_format.setFormat(no_preferred ? vk::Format::eB8G8R8A8Unorm : formats[0].format);
}

bool ZVK_Application::create_ImageViews()
{    
    std::vector<vk::Image> swap_images = logical_device.getSwapchainImagesKHR(swap_chain);
    if (!swap_images.size())
    {
        throw std::domain_error{ "No Swap Chain Images found" };
    }

    for (uint32_t i = 0; i < swap_images.size(); ++i) {
        vk::ImageViewCreateInfo color_image_view{};
        color_image_view.pNext = nullptr;
        color_image_view.image = swap_images[i];
        color_image_view.viewType = vk::ImageViewType::e2D;
        color_image_view.format = surface_format.format;
        color_image_view.components.r = vk::ComponentSwizzle::eR;
        color_image_view.components.g = vk::ComponentSwizzle::eG;
        color_image_view.components.b = vk::ComponentSwizzle::eB;
        color_image_view.components.a = vk::ComponentSwizzle::eA;
        color_image_view.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;

        image_views.push_back(logical_device.createImageView(color_image_view));
        if (!image_views[i])
        {
            throw std::domain_error{ "No Image View created" };
        }
    }

    return image_views.size() == swap_images.size();
}