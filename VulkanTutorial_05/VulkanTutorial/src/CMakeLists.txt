## CMakeLists.txt
## VulkanTutorial project
##
## Created by Andriy Zhabura on 02-Oct-2016.
## Last modified on  11-Oct-2016.
##

##
 # Copyright © 2016 Andriy Zhabura
 #
 # Permission to use, copy, modify, distribute, and sell this software and its
 # documentation for any purpose is hereby granted without fee, provided that
 # the above copyright notice appear in all copies and that both that copyright
 # notice and this permission notice appear in supporting documentation, and
 # that the name of the copyright holders not be used in advertising or
 # publicity pertaining to distribution of the software without specific,
 # written prior permission.  The copyright holders make no representations
 # about the suitability of this software for any purpose.  It is provided "as
 # is" without express or implied warranty.
 #
 # THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 # INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 # EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 # CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 # DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 # TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 # OF THIS SOFTWARE.
 ##

# Use the Win64 generator, for example:
# cmake -G "Visual Studio 12 2013 Win64" ../src

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(Vulkan_project VulkanTutorial)
set(Vulkan_prg Vulkan_project)
project(${Vulkan_project} CXX)

#use UNICODE for sources
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

#find Vulkan library
include_directories(${PROJECT_SOURCE_DIR}/../ThirdParty/include)
link_directories(${PROJECT_SOURCE_DIR}/../ThirdParty/bin)

#OS spesific adjustments
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(EXTRA_LIBS "${EXTRA_LIBS}" "vulkan-1")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

#create the Vulkan executable
file(GLOB Vulkan_SRC *.cpp)
file(GLOB Vulkan_HDR *.h)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_executable(${Vulkan_prg} ${Vulkan_SRC} ${Vulkan_HDR})
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

target_link_libraries(${Vulkan_prg} ${EXTRA_LIBS})
