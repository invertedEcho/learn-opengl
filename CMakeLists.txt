cmake_minimum_required(VERSION 3.10)
project(LearnOpenGL)

add_subdirectory(external/glfw)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(${PROJECT_NAME}
	src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    external/glfw/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad dl)
