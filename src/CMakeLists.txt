cmake_minimum_required(VERSION 3.5)
project(ProjGUI)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE src
    "*.h"
    "*.cpp"
    )

add_executable(${PROJECT_NAME} ${src})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
