cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES "world/*.cpp" "world/*.c")

add_library(soccerLib SHARED ${SOURCES})
target_compile_options (soccerLib PRIVATE -DBT_USE_DOUBLE_PRECISION=1)
target_link_libraries(soccerLib glfw3 GL X11 Xrandr Xi dl pthread BulletInverseDynamics BulletSoftBody BulletDynamics BulletCollision LinearMath)

add_executable(viewTesting worldTester.cpp)
target_compile_options (viewTesting PRIVATE -DBT_USE_DOUBLE_PRECISION=1)
target_link_libraries(viewTesting soccerLib)
