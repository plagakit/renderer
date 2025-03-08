cmake_minimum_required(VERSION 3.13)
project("renderer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
find_package(glfw3)
if (NOT glfw3_FOUND)
    include(FetchContent)
    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
    FetchContent_Declare(glad GIT_REPOSITORY https://github.com/SasLuca/MyGlad.git GIT_TAG master)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    if (NOT EMSCRIPTEN)
        FetchContent_MakeAvailable(glfw)
    endif()
    FetchContent_MakeAvailable(glad)
endif()

# ImGui
set(IMGUI_PATH "external/imgui")
file(GLOB IMGUI_SOURCES "${IMGUI_PATH}/*.cpp") 
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_PATH})
target_link_libraries(imgui glfw)

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
#target_compile_options(${PROJECT_NAME} PUBLIC  -O2)
#STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
target_link_libraries(${PROJECT_NAME} 
    PUBLIC glfw glad imgui
    PRIVATE glm::glm-header-only
)

# Other external
target_include_directories(${PROJECT_NAME} PUBLIC "external")

# Copy resources into build
set(RESOURCES_DIR "res")
file(COPY ${RESOURCES_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# Web Configurations
if (EMSCRIPTEN)
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PRIVATE  -sUSE_GLFW=3 -sALLOW_MEMORY_GROWTH -sASSERTIONS=1 -sWASM=1 -sUSE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 --preload-file ${RESOURCES_DIR})

    # https://github.com/pongasoft/emscripten-glfw?tab=readme-ov-file
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WEB=1)
    target_compile_options(${PROJECT_NAME} PUBLIC -O3 --use-port=contrib.glfw3)
    target_link_options(${PROJECT_NAME} PUBLIC --use-port=contrib.glfw3)

endif()

# Raylib MacOS dependencies
#if (APPLE)
#    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
#    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
#    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
#endif()