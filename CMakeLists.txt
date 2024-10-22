cmake_minimum_required(VERSION 3.28)
project(rendeng)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(external/dx12headers)
set(D3D12_LIBRARIES "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
set(D3D12_INCLUDE_DIRS "external/dx12headers/include/directx")

add_subdirectory(external/fmt)
add_subdirectory(external/glfw)

add_executable(rendeng main.cpp
    src/Core/Base.hpp
    src/Core/EnumFlags.hpp

    src/RHI/DynamicRHI.hpp src/RHI/DynamicRHI.cpp
    src/RHI/IWindow.hpp
    src/RHI/RHIDefinitions.hpp

    src/VulkanRHI/VulkanRHI.hpp src/VulkanRHI/VulkanRHI.cpp
    src/VulkanRHI/VulkanLayer.hpp src/VulkanRHI/VulkanLayer.cpp
    src/VulkanRHI/VulkanExtension.hpp src/VulkanRHI/VulkanExtension.cpp
    src/VulkanRHI/VulkanDevice.hpp src/VulkanRHI/VulkanDevice.cpp

    src/D3D12RHI/D3D12RHI.hpp
    src/D3D12RHI/D3D12Core.hpp src/D3D12RHI/D3D12Core.cpp
    src/VulkanRHI/VulkanCore.hpp
    src/D3D12RHI/D3D12RHI.cpp
    src/D3D12RHI/D3D12Device.hpp
    src/D3D12RHI/D3D12Device.cpp
    src/WSI/Window.hpp
    src/WSI/Window.cpp
    src/RHI/CommandQueue.hpp
    src/VulkanRHI/VulkanCommandQueue.hpp
    src/VulkanRHI/VulkanCommandQueue.cpp
    src/D3D12RHI/D3D12CommandQueue.hpp
    src/D3D12RHI/D3D12CommandQueue.cpp
    src/VulkanRHI/VulkanCommandList.hpp
    src/VulkanRHI/VulkanCommandList.cpp
    src/RHI/CommandList.hpp
    src/D3D12RHI/D3D12CommandList.hpp
    src/D3D12RHI/D3D12CommandList.cpp
)

target_compile_definitions(rendeng PRIVATE
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1

    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _AMD64_=1

    VULKAN_RHI_ENABLED
    D3D12_RHI_ENABLED

    VULKAN_DEBUGGING_ENABLED
    D3D12_DEBUGGING_ENABLED
    RHI_DEBUGGING_ENABLED
)

target_link_libraries(rendeng PRIVATE ${Vulkan_LIBRARIES} ${D3D12_LIBRARIES} fmt::fmt glfw)

target_include_directories(rendeng PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${D3D12_INCLUDE_DIRS}
    "src"
    external/fmt/include
    external/glfw/include
)
