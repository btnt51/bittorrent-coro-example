cmake_minimum_required(VERSION 3.13)

project(bittorrent-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)
find_package(Boost REQUIRED COMPONENTS system)
set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
add_executable(bittorrent ${SOURCE_FILES})
target_link_libraries(bittorrent PRIVATE Boost::system)
