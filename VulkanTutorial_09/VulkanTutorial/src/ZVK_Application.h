/* ZVK_Application.h
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
 
#ifndef ZVK_Application_h
#define ZVK_Application_h

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif // _WIN32

#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Z_Window.h"

class ZVK_Application
{
public:
	ZVK_Application();
	~ZVK_Application();

	std::vector<std::string> get_LayerPropertiesNames();
	std::vector<std::string> get_ExtensionPropertiesNames();
	size_t get_PhysicalDevicesQty();

	bool create_LogicalDevice();
    bool allocate_CommandBuffers();
    bool create_Swapchain();
    bool create_ImageViews();
    bool create_DepthBuffer();
    bool create_UniformBuffer();
    bool create_DescriptorSetLayout();
    bool create_PipelineLayout();
    bool allocate_DescriptorSets();
    void update_DescriptorSets();
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
	
	vk::CommandPool cmd_pool{};
	std::vector<vk::CommandBuffer> command_buffers;

    Z_Window window;
    vk::SurfaceKHR surface{};
    uint32_t present_family_index{};
    vk::SurfaceFormatKHR surface_format{};
    vk::SwapchainKHR swap_chain{};
    std::vector<vk::ImageView> image_views;

    void create_Surface();
    void fill_device_queue_info();
    void check_SurfaceFormat();

    vk::Format depth_format{};
    vk::Image depth_image{};
    vk::DeviceMemory depth_memory{};
    vk::ImageView depth_image_view{};

    void create_DepthImage();
    void allocate_DepthMemory();
    void create_DepthImageView();
    vk::MemoryAllocateInfo get_AllocateInfo(vk::MemoryRequirements& mem_reqs,
        vk::MemoryPropertyFlags requirements_mask = vk::MemoryPropertyFlags{});

    glm::mat4 MVP{};
    vk::Buffer uniform_buffer{};
    vk::DeviceSize uniform_buffer_memory_size{};
    vk::DeviceMemory uniform_buffer_memory{};

    void set_view();
    void allocate_UniformMemory();
    void fill_UniformMemory();

    std::vector<vk::DescriptorSetLayout> descriptor_layouts;
    vk::PipelineLayout pipeline_layout{};

    vk::DescriptorPool desc_pool{};
    std::vector<vk::DescriptorSet> descriptor_sets;
};

#endif // !ZVK_Application_h

