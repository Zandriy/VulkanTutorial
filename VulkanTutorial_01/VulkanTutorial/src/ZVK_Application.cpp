/* ZVK_Application.cpp
 * VulkanTutorial project
 *
 * Created by Andriy Zhabura on 02-Oct-2016.
 * Last modified on 03-Oct-2016.
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
#include <vector>

ZVK_Application::ZVK_Application()
{
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
}


ZVK_Application::~ZVK_Application()
{
	instance.destroy();
}
