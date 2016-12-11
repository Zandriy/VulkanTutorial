/* main.cpp
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
 
#include <iostream>

#include "ZVK_Application.h"

int main(int argc, char **argv)
try
{
	////// Start VulkanTutorial_01. //////
	ZVK_Application app;

	std::cout << "Vulkan Application was successfully created.\n";

	////// Start VulkanTutorial_02. //////
	std::vector<std::string> names = app.get_LayerPropertiesNames();
	if (names.size())
	{
		std::cout << "Available layers:\n";
		for (auto name : names)
		{
			std::cout << "    " << name << "\n";
		}
	}
	names = app.get_ExtensionPropertiesNames();
	if (names.size())
	{
		std::cout << "Available layers extensions:\n";
		for (auto name : names)
		{
			std::cout << "    " << name << "\n";
		}
	}
	std::cout << "Found " << app.get_PhysicalDevicesQty()
		<< " physical device" << (app.get_PhysicalDevicesQty() > 1 ? "s.\n" : ".\n");
	
	////// Start VulkanTutorial_03. //////
	std::cout << "Logical Device is "
		<< (app.create_LogicalDevice() ? "" : "NOT ") << "created.\n";

	////// Start VulkanTutorial_04. //////
	std::cout << "Command Buffers are "
        << (app.allocate_CommandBuffers() ? "" : "NOT ") << "allocated.\n";

    ////// Start VulkanTutorial_05. //////
    std::cout << "Swap Chain is "
        << (app.create_Swapchain() ? "" : "NOT ") << "created.\n";
    std::cout << "Image Views are "
        << (app.create_ImageViews() ? "" : "NOT ") << "created.\n";

    ////// Start VulkanTutorial_06. //////
    std::cout << "Depth Buffer is "
        << (app.create_DepthBuffer() ? "" : "NOT ") << "created.\n";

    ////// Start VulkanTutorial_07. //////
    std::cout << "Uniform Buffer is "
        << (app.create_UniformBuffer() ? "" : "NOT ") << "created.\n";

    ////// Start VulkanTutorial_08. //////
    std::cout << "Descriptor Set Layout is "
        << (app.create_DescriptorSetLayout() ? "" : "NOT ") << "created.\n";
    std::cout << "Pipeline Layout is "
        << (app.create_PipelineLayout() ? "" : "NOT ") << "created.\n";

    ////// Start VulkanTutorial_09. //////
    std::cout << "Descriptor Sets are "
        << (app.allocate_DescriptorSets() ? "" : "NOT ") << "allocated.\n";
    app.update_DescriptorSets();
    std::cout << "Descriptor Sets are updated.\n";

    ////// Start VulkanTutorial_10. //////
    std::cout << "Render Pass Layout is "
        << (app.create_RenderPass() ? "" : "NOT ") << "created.\n";

	return 0;
}
catch (std::bad_alloc&)
{
	std::cerr << "Problem with Vulkan Application creation\n";

	return 1;
}
catch (std::domain_error& err)
{
	std::cerr << err.what() << "\n";

	return 2;
}
catch (...)
{
	std::cout << "Uncaught exception discovered\n";

	return -1;
}