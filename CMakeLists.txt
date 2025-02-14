﻿# CMakeList.txt : CMake project for nameless, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("nameless")

# Add source to this project's executable.
add_executable (nameless "src/main.cpp" "src/vulkanSetup/nameless_window.cpp" "src/app.cpp" "src/vulkanSetup/nameless_pipeline.h" "src/vulkanSetup/nameless_pipeline.cpp" "src/vulkanSetup/nameless_device.h" "src/vulkanSetup/nameless_device.cpp" "src/vulkanSetup/nameless_swapchain.h" "src/vulkanSetup/nameless_swapchain.cpp" "src/vulkanSetup/nameless_model.h" "src/vulkanSetup/nameless_model.cpp" "src/nameless_game_object.h" "src/nameless_renderer.h" "src/nameless_renderer.cpp" "src/systems/base_render_system.h" "src/systems/base_render_system.cpp" "src/nameless_camera.h" "src/nameless_camera.cpp" "src/movement_controller.h" "src/movement_controller.cpp" "src/nameless_utils.h" "src/nameless_game_object.cpp" "src/vulkanSetup/nameless_buffer.h" "src/vulkanSetup/nameless_buffer.cpp" "src/nameless_frame_info.h" "src/vulkanSetup/nameless_descriptors.h" "src/vulkanSetup/nameless_descriptors.cpp" "src/systems/point_light_system.cpp" "src/systems/point_light_system.h"   "src/vulkanSetup/nameless_texture.h" "src/vulkanSetup/nameless_texture.cpp")

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)


find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} glm::glm)


find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} tinyobjloader::tinyobjloader)


include_directories(${CMAKE_SOURCE_DIR}/vcpkg/pkgs/stb_x64-windows)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nameless PROPERTY CXX_STANDARD 20)
endif()

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/src/shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E echo "Running glslc on ${GLSL} -> ${SPIRV}"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )
add_dependencies(nameless Shaders)



# Define source directory and destination directory
set(MODELS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/models")
set(MODELS_DEST_DIR "${CMAKE_BINARY_DIR}/models")

# Copy models directory to the build directory
file(COPY ${MODELS_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR})

#Copy textures directory to build
set(TEXTURES_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/textures")
set(TEXTURES_DEST_DIR "${CMAKE_BINARY_DIR}/src/textures")

file(COPY ${TEXTURES_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# TODO: Add tests and install targets if needed.
