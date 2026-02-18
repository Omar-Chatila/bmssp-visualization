cmake_minimum_required(VERSION 3.29)
project(MyCppProject VERSION 1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O0 -g3")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#add_compile_definitions(TCP_INFO_PRINT)

#add_subdirectory(external)
include(FetchContent)
# SDL3 einbinden
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG origin/main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL3
)
FetchContent_MakeAvailable(SDL3)

# SDL3 Include-Verzeichnis hinzufügen
set(SDL3_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/SDL3/include)
message(STATUS "SDL3_INCLUDE_DIRS=${SDL3_INCLUDE_DIRS}")
include_directories(${SDL3_INCLUDE_DIRS})

FetchContent_Populate(
  imgui
  URL https://github.com/ocornut/imgui/archive/docking.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)
#FetchContent_MakeAvailable(imgui)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL 2 REQUIRED)
find_package(glfw3 REQUIRED)

# Erstelle die imgui_sdl3 Bibliothek
add_library(imgui_sdl3 STATIC
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp

  imgui/backends/imgui_impl_sdl3.cpp
  imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_link_libraries(imgui_sdl3 PUBLIC SDL3::SDL3)

target_include_directories(imgui_sdl3
PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/imgui
  ${CMAKE_CURRENT_LIST_DIR}/imgui/backends
  ${SDL3_INCLUDE_DIRS}  # SDL3 Include-Verzeichnis
)

#-------------------------------------------
set(APP_NAME "TestEmu")
set(SOURCES src/main.cpp
        src/properties.h
        src/Dijkstra.h
        src/Dijkstra.cpp
        src/BlockLinkedList.h
        src/FibHeap.h
        src/FibHeap.tpp
        src/Graph.h
        src/Graph.cpp
        src/GraphFactory.h
)
set(APP_LIBS 
SDL3::SDL3 
imgui_sdl3)

add_executable(${APP_NAME} ${SOURCES})
target_include_directories(${APP_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${APP_NAME} PRIVATE ${APP_LIBS} )


