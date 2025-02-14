# Render Hardware Interface

A *(minimal)* Render Hardware Interface (RHI) for Vulkan and DirectX 12.

The RHI is compromised of 3 projects:
- [The core RHI interface](src/RHI)
- [Vulkan Backend](src/VulkanRHI)
- [DirectX 12 Backend](src/D3D12RHI)

The Vulkan backend is always available while the D3D12 backend is enabled only when building for Windows.

## Usage
You can use CMake to include the library in your project. 
```cmake
add_subdirectory(<path_to_rhi>)
target_link_libraries(<target> ... RHI)
target_include_directories(<target> ... <path_to_rhi>/src/include)
```
The window used for rendering is passed to the RHI on creation and should implement the [`RHIWindow`](src/RHI/RHIWindow.hpp) interface.

You can create and use an instance using the desired backend:
```c++
#include <RHI.hpp>
// ...
const auto RHI = createRHI({
    .apiType = RHIInterfaceType::D3D12,
    .pWindow = window,
});

// Resource creation using the RHI instance:
const auto vertexBuffer = RHI->createBuffer({
    .bufferSize = geometry->vertexCount() * sizeof(Vertex),
    .bufferType = RHIBufferType::Vertex,
    .debugName  = "VertexBuffer",
});
// ...
```

Namespacing of the RHI interface (and customizing its name) can be configured near the top of the [top-level CMake](CMakeLists.txt) file. (Disabled by default.)

**Note:** The project is primarily tested and developed on Windows using MSVC, however the Vulkan backend should support macOS via [MoltenVK](https://github.com/KhronosGroup/MoltenVK).

## Example
A minimal example rendering a cube can be found under [`example`](example).
Building the example requires Python3 to be installed as shaders are compiled by a [python script](example/Shaders/nbl_shader_util.py).

(The example target can be toggled via the `RHI_EXAMPLE` CMake variable.)

## License
The code is licensed [MIT License](https://opensource.org/licenses/MIT). Please see [the license file](LICENSE) for more information.