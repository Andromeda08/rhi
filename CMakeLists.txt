cmake_minimum_required(VERSION 3.29)
project(rendeng)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(external/dx12headers)
set(D3D12_LIBRARIES "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
set(D3D12_INCLUDE_DIRS "external/dx12headers/include/directx")

add_subdirectory(external/fmt)
add_subdirectory(external/glfw)

add_executable(rendeng main.cpp
    src/RHI/DynamicRHI.hpp
    src/RHI/RHIDefinitions.hpp
    src/Core/EnumFlags.hpp
    src/RHI/IWindow.hpp
    src/VulkanRHI/VulkanRHI.hpp
    src/D3D12RHI/D3D12RHI.hpp
    src/RHI/DynamicRHI.cpp
    src/VulkanRHI/VulkanRHI.cpp
)

target_compile_definitions(rendeng PRIVATE
    VULKAN_DEBUGGING_ENABLED
    D3D12_DEBUGGING_ENABLED
    RHI_DEBUGGING_ENABLED
)

target_link_libraries(rendeng PRIVATE ${Vulkan_LIBRARIES} fmt::fmt)

target_include_directories(rendeng PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    "src"
    external/fmt/include
)
