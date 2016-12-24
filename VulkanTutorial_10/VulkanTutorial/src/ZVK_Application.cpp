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
    if (render_pass)
    {
        logical_device.destroyRenderPass(render_pass);
    }
    if (image_acquired_semaphore)
    {
        logical_device.destroySemaphore(image_acquired_semaphore);
    }
    if (desc_pool)
    {
        logical_device.destroyDescriptorPool(desc_pool);
    }
    if (pipeline_layout)
    {
        logical_device.destroyPipelineLayout(pipeline_layout);
    }
    if (descriptor_layouts.size())
    {
        for (auto dl : descriptor_layouts)
        {
            if (dl)
            {
                logical_device.destroyDescriptorSetLayout(dl);
            }
        }
    }
    if (uniform_buffer_memory)
    {
        logical_device.freeMemory(uniform_buffer_memory);
    }
    if (uniform_buffer)
    {
        logical_device.destroyBuffer(uniform_buffer);
    }
    if (depth_image_view)
    {
        logical_device.destroyImageView(depth_image_view);
    }
    if (depth_memory)
    {
        logical_device.freeMemory(depth_memory);
    }
    if (depth_image)
    {
        logical_device.destroyImage(depth_image);
    }
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

bool ZVK_Application::create_DepthBuffer()
{
    create_DepthImage();
    allocate_DepthMemory();
    create_DepthImageView();

    return depth_image_view ? true : false;
}

void ZVK_Application::create_DepthImage()
{
    vk::ImageCreateInfo image_info = {};
    depth_format = vk::Format::eD16Unorm;
    vk::FormatProperties props = gpus[0].getFormatProperties(depth_format);
    if (props.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
        image_info.tiling = vk::ImageTiling::eLinear;
    }
    else if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
        image_info.tiling = vk::ImageTiling::eOptimal;
    }
    else {
        /* Try other depth formats? */
        throw std::domain_error{ "vk::Format::eD16Unorm is unsupported" };
    }

    image_info.pNext = nullptr;
    image_info.imageType = vk::ImageType::e2D;
    image_info.format = depth_format;
    image_info.extent.width = window.width();
    image_info.extent.height = window.height();
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = num_samples;
    image_info.initialLayout = vk::ImageLayout::eUndefined;
    image_info.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    image_info.sharingMode = vk::SharingMode::eExclusive;

    depth_image = logical_device.createImage(image_info);
    if (!depth_image)
    {
        throw std::domain_error{ "Depth Image is not created" };
    }
}

void ZVK_Application::allocate_DepthMemory()
{
    depth_memory = logical_device.allocateMemory(get_AllocateInfo(logical_device.getImageMemoryRequirements(depth_image)));
    if (!depth_memory)
    {
        throw std::domain_error{ "Depth Memory cannot be allocated" };
    }
}

vk::MemoryAllocateInfo ZVK_Application::get_AllocateInfo(vk::MemoryRequirements& mem_reqs, vk::MemoryPropertyFlags requirements_mask)
{
    vk::MemoryAllocateInfo mem_alloc{};
    mem_alloc.pNext = nullptr;
    mem_alloc.allocationSize = mem_reqs.size;
    mem_alloc.memoryTypeIndex = 0;

    vk::PhysicalDeviceMemoryProperties mem_props = gpus[0].getMemoryProperties();

    // Search memtypes to find first index with those properties
    bool pass = false;
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
        if ((mem_reqs.memoryTypeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((mem_props.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                mem_alloc.memoryTypeIndex = i;
                pass = true;
                break;
            }
        }
        mem_reqs.memoryTypeBits >>= 1;
    }

    if (!pass)
    {
        throw std::domain_error{ "Depth Memory cannot be allocated" };
    }

    return mem_alloc;
}

void ZVK_Application::create_DepthImageView()
{
    logical_device.bindImageMemory(depth_image, depth_memory, 0);

    vk::ImageViewCreateInfo view_info{};
    view_info.pNext = nullptr;
    view_info.image = depth_image;
    view_info.format = depth_format;
    view_info.components.r = vk::ComponentSwizzle::eR;
    view_info.components.g = vk::ComponentSwizzle::eG;
    view_info.components.b = vk::ComponentSwizzle::eB;
    view_info.components.a = vk::ComponentSwizzle::eA;
    view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = vk::ImageViewType::e2D;

    depth_image_view = logical_device.createImageView(view_info);
    if (!depth_image_view)
    {
        throw std::domain_error{ "Depth Image cannot be created" };
    }
}

bool ZVK_Application::create_UniformBuffer()
{
    set_view();
    allocate_UniformMemory();
    fill_UniformMemory();

    return true;
}

void ZVK_Application::set_view()
{
    auto projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    auto view = glm::lookAt(
        glm::vec3(0, 3, 10), // Camera is at (0,3,10), in World Space
        glm::vec3(0, 0, 0),  // and looks at the origin
        glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
    auto model = glm::mat4(1.0f);
    // Vulkan clip space has inverted Y and half Z.
    auto clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);

    MVP = clip * projection * view * model;
}

void ZVK_Application::allocate_UniformMemory()
{
    vk::BufferCreateInfo buf_info{};
    buf_info.pNext = nullptr;
    buf_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    buf_info.size = sizeof(MVP);
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = nullptr;
    buf_info.sharingMode = vk::SharingMode::eExclusive;

    uniform_buffer = logical_device.createBuffer(buf_info);
    if (!uniform_buffer)
    {
        throw std::domain_error{ "Uniform Buffer cannot be created" };
    }

    vk::MemoryRequirements mem_reqs = logical_device.getBufferMemoryRequirements(uniform_buffer);
    uniform_buffer_memory_size = mem_reqs.size;

    vk::MemoryPropertyFlags requirements_mask;
    uniform_buffer_memory = logical_device.allocateMemory(get_AllocateInfo(mem_reqs, requirements_mask | vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
	if (!uniform_buffer_memory)
    {
        throw std::domain_error{ "Uniform Buffer Memory cannot be allocated" };
    }
}

void ZVK_Application::fill_UniformMemory()
{
    uint8_t *pData = (uint8_t*)(logical_device.mapMemory(uniform_buffer_memory, 0, uniform_buffer_memory_size));
    if (!pData)
    {
        throw std::domain_error{ "Uniform Buffer Memory cannot be mapped" };
    }

    memcpy(pData, &MVP, sizeof(MVP));

    logical_device.unmapMemory(uniform_buffer_memory);

    logical_device.bindBufferMemory(uniform_buffer, uniform_buffer_memory, 0);
}

bool ZVK_Application::create_DescriptorSetLayout()
{
    /* Note that when we start using textures, this is where our sampler will
    * need to be specified
    */
    vk::DescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = 0;
    layout_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = vk::ShaderStageFlagBits::eVertex;
    layout_binding.pImmutableSamplers = nullptr;

    /* Next take layout bindings and use them to create a descriptor set layout
    */
    vk::DescriptorSetLayoutCreateInfo descriptor_layout_info{};
    descriptor_layout_info.setSType(vk::StructureType::eDescriptorSetLayoutCreateInfo);
    descriptor_layout_info.pNext = nullptr;
    descriptor_layout_info.bindingCount = 1;
    descriptor_layout_info.pBindings = &layout_binding;

    descriptor_layouts.push_back(logical_device.createDescriptorSetLayout(descriptor_layout_info));
    return descriptor_layouts[descriptor_layouts.size()-1] ? true : false;
}

bool ZVK_Application::create_PipelineLayout()
{
    /* Now use the descriptor layout to create a pipeline layout */
    vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.pNext = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = NULL;
    pipeline_layout_info.setLayoutCount = uint32_t( descriptor_layouts.size() );
    pipeline_layout_info.pSetLayouts = descriptor_layouts.data();

    pipeline_layout = logical_device.createPipelineLayout(pipeline_layout_info);

    return pipeline_layout ? true : false;
}

bool ZVK_Application::allocate_DescriptorSets()
{
    vk::DescriptorPoolSize type_count[1];
    type_count[0].type = vk::DescriptorType::eUniformBuffer;
    type_count[0].descriptorCount = 1;

    vk::DescriptorPoolCreateInfo desc_pool_info{};
    desc_pool_info.setSType(vk::StructureType::eDescriptorPoolCreateInfo);
    desc_pool_info.pNext = nullptr;
    desc_pool_info.maxSets = 1;
    desc_pool_info.poolSizeCount = 1;
    desc_pool_info.pPoolSizes = type_count;

    desc_pool = logical_device.createDescriptorPool(desc_pool_info);

    if (!desc_pool)
    {
        throw std::domain_error{ "DescriptorPool was not created" };
    }

    vk::DescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].pNext = nullptr;
    alloc_info[0].descriptorPool = desc_pool;
    alloc_info[0].descriptorSetCount = uint32_t( descriptor_layouts.size() );
    alloc_info[0].pSetLayouts = descriptor_layouts.data();

    descriptor_sets = logical_device.allocateDescriptorSets(alloc_info[0]);

    if (!descriptor_sets.size())
    {
        throw std::domain_error{ "Descriptor Sets was not allocated" };
    }

    return true;
}

void ZVK_Application::update_DescriptorSets()
{
    vk::DescriptorBufferInfo buffer_info{ uniform_buffer, 0, sizeof(MVP) };

    vk::WriteDescriptorSet writes[1];
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptor_sets[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &buffer_info;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    logical_device.updateDescriptorSets(1, writes, 0, nullptr);
}

bool ZVK_Application::create_Semaphore()
{
    vk::SemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;

    image_acquired_semaphore = logical_device.createSemaphore(imageAcquiredSemaphoreCreateInfo);

    return image_acquired_semaphore ? true : false;
}

bool ZVK_Application::create_RenderPass()
{
	// Start recording command buffer.
    vk::CommandBufferBeginInfo cmd_buf_info{};
    command_buffers[currect_command_buffer].begin(cmd_buf_info);

    if (!create_Semaphore())
    {
        return false;
    }

	// Set the depth buffer layout.
	set_image_layout(depth_image, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // Acquire the swapchain image in order to set its layout.
    vk::Fence fence{};
    uint32_t current_buffer;
    if (vk::Result::eSuccess != logical_device.acquireNextImageKHR(swap_chain, UINT64_MAX, image_acquired_semaphore, fence, &current_buffer))
    {
        return false;
    }
    std::vector<vk::Image> swap_images = logical_device.getSwapchainImagesKHR(swap_chain);

    // Set the layout for the color buffer, transitioning it from
    // undefined to an optimal color attachment to make it usable in
    // a render pass.
    set_image_layout(swap_images[current_buffer], vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

    // Stop recording the command buffer here since this sample will not
    // actually put the render pass in the command buffer (via vk::CommandBuffer::beginRenderPass).
    // An actual application might leave the command buffer in recording mode
    // and insert vk::CommandBuffer::beginRenderPass() after the image layout transition
    // memory barrier commands.
    // This sample simply creates and defines the render pass.
    command_buffers[currect_command_buffer].end();

    /* Need attachments for render target and depth buffer */
    vk::AttachmentDescription attachments[2]{};
    attachments[0].format = surface_format.format;
    attachments[0].samples = num_samples;
    attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
    attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
    attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[0].initialLayout = vk::ImageLayout::eUndefined;
    attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

    attachments[1].format = depth_format;
    attachments[1].samples = num_samples;
    attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
    attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
    attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].initialLayout = vk::ImageLayout::eUndefined;
    attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference color_reference{};
    color_reference.attachment = 0;
    color_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depth_reference{};
    depth_reference.attachment = 1;
    depth_reference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pDepthStencilAttachment = &depth_reference;

    vk::RenderPassCreateInfo rp_info{};
    rp_info.attachmentCount = 2;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;

    render_pass = logical_device.createRenderPass(rp_info);

    return render_pass ? true : false;
}

void ZVK_Application::set_image_layout(vk::Image& image, vk::ImageAspectFlags aspectMask, vk::ImageLayout old_image_layout, vk::ImageLayout new_image_layout)
{
    /* DEPENDS on vk::CommandBuffer and vk::DeviceQueueCreateInfo are initialized */
	if (!command_buffers.size() || UINT32_MAX == device_queue_info.queueFamilyIndex)
    {
        throw std::domain_error{ "Command Buffers were not created or Device Queue is not initialized" };
    }

    vk::Queue graphics_queue = logical_device.getQueue(device_info.pQueueCreateInfos->queueFamilyIndex, 0);
    std::vector<vk::Image> swap_images = logical_device.getSwapchainImagesKHR(swap_chain);

    vk::ImageMemoryBarrier image_memory_barrier {};
    image_memory_barrier.oldLayout = old_image_layout;
    image_memory_barrier.newLayout = new_image_layout;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = aspectMask;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

	switch (old_image_layout)
	{
	case vk::ImageLayout::eColorAttachmentOptimal:
		image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
		image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		break;
	case vk::ImageLayout::ePreinitialized:
		image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		break;
	default:
		// do not set srcAccessMask
		break;
	}

	switch (new_image_layout)
	{
	case vk::ImageLayout::eTransferDstOptimal:
		image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
		break;
	case vk::ImageLayout::eTransferSrcOptimal:
		image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
		break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		break;
	case vk::ImageLayout::eColorAttachmentOptimal:
		image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		break;
	default:
		// do not set dstAccessMask
		break;
	}

    vk::PipelineStageFlagBits src_stages = vk::PipelineStageFlagBits::eTopOfPipe;
    vk::PipelineStageFlagBits dest_stages = vk::PipelineStageFlagBits::eTopOfPipe;
    
    command_buffers[currect_command_buffer].pipelineBarrier(src_stages, dest_stages, vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
}