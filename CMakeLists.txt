cmake_minimum_required(VERSION 3.7)

project(playful-palette-igr205)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the Qt library
find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets OpenGL)

# tinygltf
include_directories(lib/tinygltf)

add_definitions(-DPROJECT_DIR="${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE SRCS src/window/*.cc src/paletteWidget/*.cpp)

message("${SRCS}")

add_executable(viewer src/viewer.cc ${SRCS})

target_link_libraries(viewer Qt5::Widgets Qt5::Core Qt5::OpenGL Qt5::Gui)
