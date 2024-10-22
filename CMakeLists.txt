cmake_minimum_required(VERSION 3.28)
project(rendeng)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(external/dx12headers)
set(D3D12_LIBRARIES "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
set(D3D12_INCLUDE_DIRS "external/dx12headers/include/directx")

add_subdirectory(external/fmt)
add_subdirectory(external/glfw)

add_library(VulkanRHI
    src/VulkanRHI/VulkanCore.hpp
    src/VulkanRHI/VulkanRHI.hpp src/VulkanRHI/VulkanRHI.cpp
    src/VulkanRHI/VulkanLayer.hpp src/VulkanRHI/VulkanLayer.cpp
    src/VulkanRHI/VulkanExtension.hpp src/VulkanRHI/VulkanExtension.cpp
    src/VulkanRHI/VulkanDevice.hpp src/VulkanRHI/VulkanDevice.cpp
    src/VulkanRHI/VulkanCommandQueue.hpp src/VulkanRHI/VulkanCommandQueue.cpp
    src/VulkanRHI/VulkanCommandList.hpp src/VulkanRHI/VulkanCommandList.cpp
)
target_compile_definitions(VulkanRHI PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_DEBUGGING_ENABLED)
target_include_directories(VulkanRHI PUBLIC ${Vulkan_INCLUDE_DIRS} "src" "external/fmt/include")
target_link_libraries(VulkanRHI PUBLIC ${Vulkan_LIBRARIES} fmt::fmt)

if (WIN32)
add_library(D3D12RHI
    src/D3D12RHI/D3D12Core.hpp src/D3D12RHI/D3D12Core.cpp
    src/D3D12RHI/D3D12RHI.hpp src/D3D12RHI/D3D12RHI.cpp
    src/D3D12RHI/D3D12Device.hpp src/D3D12RHI/D3D12Device.cpp
    src/D3D12RHI/D3D12CommandQueue.hpp src/D3D12RHI/D3D12CommandQueue.cpp
    src/D3D12RHI/D3D12CommandList.hpp src/D3D12RHI/D3D12CommandList.cpp
)
target_compile_definitions(D3D12RHI PRIVATE D3D12_DEBUGGING_ENABLED)
target_include_directories(D3D12RHI PUBLIC ${D3D12_INCLUDE_DIRS} "src" "external/fmt/include")
target_link_libraries(D3D12RHI PUBLIC ${D3D12_LIBRARIES} fmt::fmt)
endif()

add_executable(rendeng main.cpp
    src/Core/Base.hpp
    src/Core/EnumFlags.hpp
    src/WSI/Window.hpp src/WSI/Window.cpp
    src/RHI/DynamicRHI.hpp src/RHI/DynamicRHI.cpp
    src/RHI/IWindow.hpp
    src/RHI/RHIDefinitions.hpp
    src/RHI/CommandQueue.hpp
    src/RHI/CommandList.hpp
)

target_compile_definitions(rendeng PRIVATE
    VULKAN_RHI_ENABLED
    RHI_DEBUGGING_ENABLED
)
target_link_libraries(rendeng PRIVATE VulkanRHI fmt::fmt glfw)
target_include_directories(rendeng PRIVATE
    "src"
    external/fmt/include
    external/glfw/include
)

if (WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX _AMD64_=1)
    target_compile_definitions(rendeng PRIVATE D3D12_RHI_ENABLED)
    target_link_libraries(rendeng PRIVATE D3D12RHI)
endif()
