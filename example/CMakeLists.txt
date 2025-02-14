find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_executable("rhi_example"
    Scene/Geometry.hpp Scene/Geometry.cpp
    WSI/Window.hpp WSI/Window.cpp
    main.cpp
)

target_include_directories("rhi_example" PRIVATE
    "../src"
    "../external/glfw/include"
    "../external/glm"
)
target_link_libraries("rhi_example" PRIVATE glfw glm::glm RHI)

add_custom_command(
        TARGET "rhi_example"
        POST_BUILD
        COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_LIST_DIR}/Shaders/nbl_shader_util.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Shaders"
)