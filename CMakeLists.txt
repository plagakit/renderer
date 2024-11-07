cmake_minimum_required(VERSION 3.13)
project("renderer")

# GLFW
find_package(glfw3)
if (NOT glfw3_FOUND)
    include(FetchContent)
    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
    FetchContent_Declare(glad GIT_REPOSITORY https://github.com/SasLuca/MyGlad.git GIT_TAG master)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
    FetchContent_MakeAvailable(glad)
endif()

# raylib
#[[
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION})
if (NOT raylib_FOUND) # If not found, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
	set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# raylib-cpp
find_package(raylib_cpp)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG "master"
		SYSTEM # suppress compiler warnings
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

# raygui
add_library(raygui INTERFACE)
target_include_directories(raygui SYSTEM INTERFACE external)
]]

# ImGui
set(IMGUI_PATH "external/imgui")
file(GLOB IMGUI_SOURCES "${IMGUI_PATH}/*.cpp") 
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_PATH})
target_link_libraries(imgui glfw)

# ImGui raylib backend
#[[
set(IMGUI_RAYLIB_DIR "external/rlImGui")
add_library(imgui_raylib STATIC "${IMGUI_RAYLIB_DIR}/rlImGui.cpp")
target_compile_definitions(imgui_raylib PUBLIC NO_FONT_AWESOME)
target_include_directories(imgui_raylib SYSTEM PUBLIC ${IMGUI_RAYLIB_DIR})
target_link_libraries(imgui_raylib imgui raylib)
]]

# GLM
find_package(glm)
if (NOT glm_FOUND)
    include(FetchContent)
    FetchContent_Declare(
	    glm
	    GIT_REPOSITORY	https://github.com/g-truc/glm.git
	    GIT_TAGS master
    )
    FetchContent_MakeAvailable(glm)
endif()

# MAIN FILES
add_executable(${PROJECT_NAME})
add_subdirectory("src")
target_link_libraries(${PROJECT_NAME} 
    PUBLIC glfw glad imgui
    PRIVATE glm::glm-header-only
)

# Copy resources into build
set(RESOURCES_DIR "res")
file(COPY ${RESOURCES_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# Web Configurations
if (EMSCRIPTEN)
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

	# Required linker flags for using Raylib with Emscripten
    target_link_options(${PROJECT_NAME} PRIVATE -sEXPORTED_FUNCTIONS=['_main','_malloc'] -sEXPORTED_RUNTIME_METHODS=ccall -sUSE_GLFW=3 -sALLOW_MEMORY_GROWTH --preload-file ${RESOURCES_DIR})

    # https://github.com/pongasoft/emscripten-glfw?tab=readme-ov-file
    target_compile_options(${PROJECT_NAME} PUBLIC "--use-port=contrib.glfw3")
    target_link_options(${PROJECT_NAME} PUBLIC "--use-port=contrib.glfw3")

endif()

# Raylib MacOS dependencies
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()