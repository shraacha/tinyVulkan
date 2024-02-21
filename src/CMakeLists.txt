cmake_minimum_required(VERSION 3.10)



# external libraries and ENV vars
# TODO make platform agnostic
set(VK_LOCATION /Users/anish/VulkanSDK/1.3.275.0)

# env vars
set(VK_ICD_FILENAMES ${VK_LOCATION}/macOS/share/vulkan/icd.d/MoltenVK_icd.json)
set(VK_LAYER_PATH ${VK_LOCATION}/macOS/share/vulkan/explicit_layer.d)

# *IMPORTANT* run this before the app
configure_file(EnvironmentVars.sh.in EnvironmentVars.sh)

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

# vulkan lib base (1)
find_library(vulkan_LIB_BASE
  NAMES vulkan.1
  HINTS ${VK_LOCATION}/macOS/lib)

if(vulkan_LIB_BASE)
  message(STATUS "vulkan base ver found!")
else()
  message(FATAL_ERROR "no vulkan base ver :(")
endif()

# vulkan lib versioned
find_library(vulkan_LIB_VERSIONED
  NAMES vulkan.1.3.275
  HINTS ${VK_LOCATION}/macOS/lib)

if(vulkan_LIB_VERSIONED)
  message(STATUS "vulkan versioned found!")
else()
  message(FATAL_ERROR "no vulkan versioned :(")
endif()

set(LIBS ${glm_LIB} ${glfw3_LIB} ${vulkan_LIB_BASE} ${vulkan_LIB_VERSIONED})

set(INCLUDES /opt/homebrew/include ${VK_LOCATION}/macOS/include)



# our project

project(TinyVulkan)

# vars
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# local
# execs and local libs
add_library(Tv_Window tv_window.cpp)
add_library(First_App first_app.cpp)
add_executable(TinyVulkan main.cpp)

# include
target_include_directories(Tv_Window PUBLIC ${INCLUDES})
target_include_directories(First_App PUBLIC ${INCLUDES})

# linking
target_link_libraries(TinyVulkan
  PUBLIC Tv_Window
  PUBLIC First_App)

# TODO separate this out into subdirs
target_link_libraries(Tv_Window ${LIBS})
target_link_libraries(First_App ${LIBS})
