cmake_minimum_required(VERSION 3.28)
project(RHI)

set(CMAKE_CXX_STANDARD 23)

# ============== #
#  Dependencies  #
# ============== #
find_package(Vulkan REQUIRED)

add_subdirectory(external/dx12headers)
set(D3D12_LIBRARIES "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
set(D3D12_INCLUDE_DIRS "external/dx12headers/include/directx")

add_subdirectory(external/fmt)
add_subdirectory(external/glfw)

# ============== #
#   Vulkan RHI   #
# ============== #
add_compile_definitions(VULKAN_RHI_ENABLED)

add_library(VulkanRHI
    src/VulkanRHI/VulkanBase.hpp
    src/VulkanRHI/VulkanCommandQueue.hpp src/VulkanRHI/VulkanCommandQueue.cpp
    src/VulkanRHI/VulkanDebugContext.hpp src/VulkanRHI/VulkanDebugContext.cpp
    src/VulkanRHI/VulkanDevice.hpp src/VulkanRHI/VulkanDevice.cpp
    src/VulkanRHI/VulkanDeviceExtension.hpp src/VulkanRHI/VulkanDeviceExtension.cpp
    src/VulkanRHI/VulkanRHI.hpp src/VulkanRHI/VulkanRHI.cpp
    src/VulkanRHI/VulkanSwapchain.hpp src/VulkanRHI/VulkanSwapchain.cpp
        src/VulkanRHI/VulkanBuffer.hpp
        src/VulkanRHI/VulkanBuffer.cpp
        src/VulkanRHI/VulkanAllocator.hpp
    src/VulkanRHI/VulkanAllocator.cpp
    src/VulkanRHI/VulkanPipeline.hpp
    src/VulkanRHI/VulkanPipeline.cpp
        src/VulkanRHI/VulkanRenderpass.hpp
    src/VulkanRHI/VulkanFramebuffer.hpp
    src/VulkanRHI/Platform.hpp
)
target_include_directories(VulkanRHI PUBLIC ${Vulkan_INCLUDE_DIRS} "src" "external/fmt/include")
target_link_libraries(VulkanRHI PUBLIC ${Vulkan_LIBRARIES} fmt::fmt)
target_compile_definitions(VulkanRHI
    PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    PRIVATE VULKAN_DEBUGGING_ENABLED
    PRIVATE VULKAN_VERBOSE_LOGGING
    # PRIVATE VULKAN_API_DUMP
)

# ============== #
#   D3D12 RHI    #
# ============== #
if (__APPLE__)
    add_compile_definitions(D3D12_RHI_ENABLED WIN32_LEAN_AND_MEAN NOMINMAX _AMD64_=1)

    add_library(D3D12RHI
        src/D3D12RHI/D3D12Core.hpp src/D3D12RHI/D3D12Core.cpp
        src/D3D12RHI/D3D12RHI.hpp src/D3D12RHI/D3D12RHI.cpp
        src/D3D12RHI/D3D12Device.hpp src/D3D12RHI/D3D12Device.cpp
        src/D3D12RHI/D3D12CommandQueue.hpp src/D3D12RHI/D3D12CommandQueue.cpp
        src/D3D12RHI/D3D12CommandList.hpp src/D3D12RHI/D3D12CommandList.cpp
        src/D3D12RHI/D3D12Swapchain.hpp src/D3D12RHI/D3D12Swapchain.cpp
    )
    target_include_directories(D3D12RHI PUBLIC ${D3D12_INCLUDE_DIRS} "src" "external/fmt/include")
    target_link_libraries(D3D12RHI PUBLIC ${D3D12_LIBRARIES} fmt::fmt)
    target_compile_definitions(D3D12RHI
        PRIVATE D3D12_DEBUGGING_ENABLED
        PRIVATE D3D12_VERBOSE_LOGGING
    )
endif()

# ============== #
#   RHI Library  #
# ============== #
add_executable(${PROJECT_NAME}
    main.cpp
    src/Core/Base.hpp
    src/Core/EnumFlags.hpp
    src/WSI/Window.hpp src/WSI/Window.cpp
    src/RHI/DynamicRHI.hpp src/RHI/DynamicRHI.cpp
        src/RHI/RHIWindow.hpp
        src/RHI/Definitions.hpp
        src/RHI/RHICommandQueue.hpp
        src/RHI/RHICommandList.hpp
    src/RHI/RHISwapchain.hpp
    src/RHI/Macros.hpp
        src/RHI/RHIBuffer.hpp
        src/RHI/Definitions.cpp
        src/RHI/RHIPipeline.hpp
    src/RHI/Frame.hpp
    src/RHI/RHIFramebuffer.hpp
        src/RHI/RHIRenderPass.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE VulkanRHI fmt::fmt glfw)
target_compile_definitions(${PROJECT_NAME} PRIVATE RHI_DEBUGGING_ENABLED)
target_include_directories(${PROJECT_NAME} PRIVATE
    "src"
    external/fmt/include
    external/glfw/include
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE D3D12RHI)
endif()
