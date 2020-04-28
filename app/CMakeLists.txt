cmake_minimum_required(VERSION 2.6)

project(app)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) # Put binaries into root of build tree

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -std=c++17")

# find_package(snap REQUIRED)
find_package(metal_fs REQUIRED)

add_executable(app
    app.cpp
)

target_link_libraries(app
    metal-pipeline
    metal-filesystem-pipeline
)
