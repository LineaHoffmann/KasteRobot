cmake_minimum_required(VERSION 3.5)
project(RobotControl)

#find_package(OpenCV REQUIRED)
find_package(ur_rtde REQUIRED)

file(GLOB_RECURSE src
    "*.h"
    "*.cpp"
    )

add_executable(${PROJECT_NAME} ${src})

#target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ur_rtde::rtde)
