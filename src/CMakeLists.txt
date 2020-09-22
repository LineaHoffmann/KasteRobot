cmake_minimum_required(VERSION 3.16.3)
project(ProjGUI)

set(CMAKE_CXX_STANDARD 17)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

find_package(OpenCV REQUIRED)

#file(GLOB_RECURSE src
#    "*.h"
#    "*.cpp"
#    )

file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/gui/cApp.cpp
    ${PROJECT_SOURCE_DIR}/gui/cApp.h
    ${PROJECT_SOURCE_DIR}/gui/cMain.cpp
    ${PROJECT_SOURCE_DIR}/gui/cMain.h
    ${PROJECT_SOURCE_DIR}/gui/cCamWorker.cpp
    ${PROJECT_SOURCE_DIR}/gui/cCamWorker.h
    ${PROJECT_SOURCE_DIR}/gui/cImagePanel.cpp
    ${PROJECT_SOURCE_DIR}/gui/cImagePanel.h
    )

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

#target_link_libraries(${PROJECT_NAME} pthread)
