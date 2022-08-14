cmake_minimum_required(VERSION 3.12)

project(Interpolation_demo
	VERSION 1.0
	LANGUAGES CXX)

# Headers
include_directories(birb2d/include/)
include_directories(birb2d/vendor/SDL2_gfx/)

add_subdirectory(birb2d)

add_executable(interpolation_demo src/main.cpp)
target_link_libraries(interpolation_demo birb2d)

# Copy the resources into the build directory
file(COPY ./res DESTINATION ./)
