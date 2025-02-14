cmake_minimum_required(VERSION 3.28)
project(RHI)

set(CMAKE_CXX_STANDARD 23)

# ============== #
#    Options     #
# ============== #
# region Options

# Build example project
set(RHI_EXAMPLE TRUE)
# Use namespace for RHI code
set(rhi_USE_NAMESPACE FALSE)
# Customize the name of the RHI namespace.
set(rhi_NAMESPACE rhi)

if (rhi_USE_NAMESPACE)
    add_compile_definitions(rhi_USE_NAMESPACE rhi_NAMESPACE=${rhi_NAMESPACE})
endif()

# endregion

# ============== #
#  Dependencies  #
# ============== #
find_package(Vulkan REQUIRED)

if (WIN32)
    add_subdirectory(external/dx12headers)
    set(D3D12_LIBRARIES "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
    set(D3D12_INCLUDE_DIRS "external/dx12headers/include/directx")

    add_subdirectory(external/d3d12ma)
    set(D3D12MA_INCLUDE_DIRS "external/d3d12ma/include")
endif()

add_subdirectory(external/fmt)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

# ============== #
#   RHI Library  #
# ============== #
add_library(${PROJECT_NAME}
    # region RHI_Sources
    src/RHI/Definitions.hpp
    src/RHI/Definitions.cpp
    src/RHI/DynamicRHI.hpp
    src/RHI/DynamicRHI.cpp
    src/RHI/Frame.hpp
    src/RHI/Macros.hpp
    src/RHI/RHIBuffer.hpp
    src/RHI/RHICommandList.hpp
    src/RHI/RHICommandQueue.hpp
    src/RHI/RHIFramebuffer.hpp
    src/RHI/RHIPipeline.hpp
    src/RHI/RHIRenderPass.hpp
    src/RHI/RHISwapchain.hpp
    src/RHI/RHITexture.hpp
    src/RHI/RHIWindow.hpp
    src/RHI.hpp
    src/RHI.cpp
    # endregion
)

target_precompile_headers(${PROJECT_NAME} PUBLIC "src/pch.hpp")
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanRHI fmt::fmt)
target_include_directories(${PROJECT_NAME} PRIVATE "src" "external/fmt/include")

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC D3D12RHI)
endif()

# ============== #
#   Vulkan RHI   #
# ============== #
add_compile_definitions(VULKAN_RHI_ENABLED)

add_library(VulkanRHI
    # region VulkanRHI_Sources
    src/VulkanRHI/Platform.hpp
    src/VulkanRHI/VulkanBase.hpp
    src/VulkanRHI/VulkanCommandQueue.hpp    src/VulkanRHI/VulkanCommandQueue.cpp
    src/VulkanRHI/VulkanDebugContext.hpp    src/VulkanRHI/VulkanDebugContext.cpp
    src/VulkanRHI/VulkanDevice.hpp          src/VulkanRHI/VulkanDevice.cpp
    src/VulkanRHI/VulkanDeviceExtension.hpp src/VulkanRHI/VulkanDeviceExtension.cpp
    src/VulkanRHI/VulkanRHI.hpp             src/VulkanRHI/VulkanRHI.cpp
    src/VulkanRHI/VulkanSwapchain.hpp       src/VulkanRHI/VulkanSwapchain.cpp
    src/VulkanRHI/VulkanBuffer.hpp          src/VulkanRHI/VulkanBuffer.cpp
    src/VulkanRHI/VulkanAllocator.hpp       src/VulkanRHI/VulkanAllocator.cpp
    src/VulkanRHI/VulkanPipeline.hpp        src/VulkanRHI/VulkanPipeline.cpp
    src/VulkanRHI/VulkanFramebuffer.hpp     src/VulkanRHI/VulkanFramebuffer.cpp
    src/VulkanRHI/VulkanRenderPass.hpp      src/VulkanRHI/VulkanRenderPass.cpp
    src/VulkanRHI/VulkanTexture.hpp         src/VulkanRHI/VulkanTexture.cpp
    # endregion
)
target_include_directories(VulkanRHI PUBLIC ${Vulkan_INCLUDE_DIRS} "src" "external/fmt/include")
target_link_libraries(VulkanRHI PUBLIC ${Vulkan_LIBRARIES} fmt::fmt)
target_precompile_headers(VulkanRHI REUSE_FROM RHI)
target_compile_definitions(VulkanRHI
    PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    PRIVATE VULKAN_DEBUGGING_ENABLED
    # PRIVATE VULKAN_VERBOSE_LOGGING
    # PRIVATE VULKAN_API_DUMP
)

# ============== #
#   D3D12 RHI    #
# ============== #
if (WIN32)
    add_compile_definitions(D3D12_RHI_ENABLED WIN32_LEAN_AND_MEAN NOMINMAX _AMD64_=1)

    add_library(D3D12RHI
        #region D3D12RHI_Sources
        src/D3D12RHI/D3D12Core.hpp          src/D3D12RHI/D3D12Core.cpp
        src/D3D12RHI/D3D12RHI.hpp           src/D3D12RHI/D3D12RHI.cpp
        src/D3D12RHI/D3D12Device.hpp        src/D3D12RHI/D3D12Device.cpp
        src/D3D12RHI/D3D12CommandQueue.hpp  src/D3D12RHI/D3D12CommandQueue.cpp
        src/D3D12RHI/D3D12CommandList.hpp   src/D3D12RHI/D3D12CommandList.cpp
        src/D3D12RHI/D3D12Swapchain.hpp     src/D3D12RHI/D3D12Swapchain.cpp
        src/D3D12RHI/D3D12Framebuffer.hpp   src/D3D12RHI/D3D12Framebuffer.cpp
        src/D3D12RHI/D3D12RenderPass.hpp    src/D3D12RHI/D3D12RenderPass.cpp
        src/D3D12RHI/D3D12Pipeline.hpp      src/D3D12RHI/D3D12Pipeline.cpp
        src/D3D12RHI/D3D12Buffer.hpp
        src/D3D12RHI/D3D12Buffer.cpp
        src/D3D12RHI/D3D12Texture.hpp
        src/D3D12RHI/D3D12Texture.cpp
        # endregion
    )
    target_include_directories(D3D12RHI PUBLIC ${D3D12_INCLUDE_DIRS} ${D3D12MA_INCLUDE_DIRS} "src" "external/fmt/include")
    target_link_libraries(D3D12RHI PUBLIC ${D3D12_LIBRARIES} D3D12MemoryAllocator fmt::fmt)
    target_precompile_headers(D3D12RHI REUSE_FROM RHI)
    target_compile_definitions(D3D12RHI
        PRIVATE D3D12_DEBUGGING_ENABLED
        # PRIVATE D3D12_VERBOSE_LOGGING
    )
endif()

# ============== #
#  Example App.  #
# ============== #
if (RHI_EXAMPLE)
    add_subdirectory(example)
endif()