cmake_minimum_required(VERSION 3.10)

# *IMPORTANT* run this before compiling / running app
# `source ~/VulkanSDK/1.3.275.0/setup-env.sh`

# ~~ external libraries ~~
# glm
find_library(glm_LIB
  NAMES glm
  HINTS /opt/homebrew/lib)

if(glm_LIB)
  message(STATUS "glm found!")
else()
  message(FATAL_ERROR "no glm :(")
endif()

# glfw3
find_library(glfw3_LIB
  NAMES glfw.3.3
  HINTS /opt/homebrew/lib)

if(glfw3_LIB)
  message(STATUS "glfw3 found!")
else()
  message(FATAL_ERROR "no glfw3 :(")
endif()

# vulkan & glsl stuff
find_package(Vulkan REQUIRED)

if (${Vulkan_FOUND})
  message(STATUS "vulkan found!")
else()
  message(FATAL_ERROR "no vulkan found :(")
endif()

if (${Vulkan_glslangValidator_FOUND})
  message(STATUS "glslangValidator found!")
else()
  message(FATAL_ERROR "no glslangValidator found :(")
endif()

if (${Vulkan_glslc_FOUND})
  message(STATUS "glslc found!")
else()
  message(FATAL_ERROR "no glslc found :(")
endif()

set(LIBS ${glm_LIB} ${glfw3_LIB} ${Vulkan_LIBRARY} ${Vulkan_glslang_LIBRARY})

set(INCLUDES /opt/homebrew/include ${Vulkan_INCLUDE_DIR})


# ~~ our project ~~
project(TinyVulkan)

# vars
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# glsl compilation
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.frag"
    "shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
    )

# add_custom_command(TARGET YourMainTarget POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:TinyVulkan>/shaders/"
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "${PROJECT_BINARY_DIR}/shaders"
#         "$<TARGET_FILE_DIR:TinyVulkan>/shaders"
#         )


# ~~ adding deps and stuff ~~
# local
# execs and local libs
add_library(Tv_Window tv_window.cpp)
add_library(Tv_Pipeline tv_pipeline.cpp)
add_library(Tv_Device tv_device.cpp)
add_library(First_App first_app.cpp)
add_executable(TinyVulkan main.cpp)
add_dependencies(TinyVulkan Shaders)

# ~~ include ~~
target_include_directories(Tv_Window PUBLIC ${INCLUDES})
target_include_directories(Tv_Device PUBLIC ${INCLUDES})
target_include_directories(Tv_Pipeline PUBLIC ${INCLUDES})
target_include_directories(First_App PUBLIC ${INCLUDES})

# ~~ linking ~~
target_link_libraries(TinyVulkan
  PUBLIC Tv_Window
  PUBLIC Tv_Device
  PUBLIC Tv_Pipeline
  PUBLIC First_App)

# TODO separate this out into subdirs
target_link_libraries(Tv_Window ${LIBS})
target_link_libraries(Tv_Device ${LIBS})
target_link_libraries(Tv_Pipeline PUBLIC ${LIBS})
target_link_libraries(First_App ${LIBS})
