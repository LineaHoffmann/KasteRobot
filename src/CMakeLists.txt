cmake_minimum_required(VERSION 3.10.2) # Ubuntu 18.04 default right now? My 20.04 is 3.16.3
project(KasteRobot)

#set(CMAKE_COMPILER_GTK_FLAGS "-DWITH_GTK_3_X=ON")
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CMAKE_COMPILER_GTK_FLAGS}")

# Using C++17 (used previously for inline const globals.. Probably unnecessary)
# Jonas used C++11 for the camera, 17 should be good
set(CMAKE_CXX_STANDARD 17)

# Building as debug for now (my robworks is not though, may be a problem)
set(CMAKE_BUILD_TYPE Debug)

# wxWidgets library
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include("${wxWidgets_USE_FILE}")

# OpenCV library
find_package(OpenCV REQUIRED)

# Pylon library
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Pylon)
include_directories(${Pylon_INCLUDE_DIRS})

# Robworks library
set(RW_ROOT /usr/lib/x86_64-linux-gnu/RobWork)
set(RW_DIR /${RW_ROOT}/cmake)
find_package(RobWork REQUIRED)
link_directories(${ROBWORK_LIBRARY_DIRS})
include_directories(${ROBWORK_INCLUDE_DIRS})

# UR_RTDE library
find_package(ur_rtde REQUIRED)

# USE EXPLICIT GLOBBING! please ..
file(GLOB SRC
    ${PROJECT_SOURCE_DIR}/app.cpp
    ${PROJECT_SOURCE_DIR}/app.h
    ${PROJECT_SOURCE_DIR}/includeheader.h
    )

# GUI layer source files GLOB
file(GLOB GUI_FILES
    ${PROJECT_SOURCE_DIR}/gui/cmain.cpp
    ${PROJECT_SOURCE_DIR}/gui/cmain.h

    ${PROJECT_SOURCE_DIR}/gui/cimagepanel.cpp
    ${PROJECT_SOURCE_DIR}/gui/cimagepanel.h

    ${PROJECT_SOURCE_DIR}/gui/clinker.cpp
    ${PROJECT_SOURCE_DIR}/gui/clinker.h
    )

# Logic layer source files GLOB
file(GLOB LOGIC_FILES
    ${PROJECT_SOURCE_DIR}/logic/xbaslercam.cpp
    ${PROJECT_SOURCE_DIR}/logic/xbaslercam.h

    ${PROJECT_SOURCE_DIR}/logic/ximagehandler.cpp
    ${PROJECT_SOURCE_DIR}/logic/ximagehandler.h

    ${PROJECT_SOURCE_DIR}/logic/xlinker.cpp
    ${PROJECT_SOURCE_DIR}/logic/xlinker.h

    ${PROJECT_SOURCE_DIR}/logic/xrobotexceptions.h

    ${PROJECT_SOURCE_DIR}/logic/xur_control.cpp
    ${PROJECT_SOURCE_DIR}/logic/xur_control.h

    ${PROJECT_SOURCE_DIR}/logic/xgripperclient.h
    ${PROJECT_SOURCE_DIR}/logic/xgripperclient.cpp

    ${PROJECT_SOURCE_DIR}/logic/xdetectcollision.h
    ${PROJECT_SOURCE_DIR}/logic/xdetectcollision.cpp
    )

# Database layer source files GLOB
file (GLOB DB_FILES
    )

# Creating executable, linked with source files
# CMake complains here: Files in some directories may conflict with implicit libraries (librtde.so.1.3 in /usr/lib/x86_64-linux-gnu hidden by /usr/local/lib)
# Might be a local mistake on my machine due to multiples of the same lib
add_executable(${PROJECT_NAME} ${SRC} ${GUI_FILES} ${LOGIC_FILES} ${DB_FILES})

# Some more wxWidgets compiling stuff (wx-config --cxxflags)
set(GCC_COVERAGE_COMPILE_FLAGS "-I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# Linking libraries
target_link_libraries(${PROJECT_NAME} ${Pylon_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${ROBWORK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES}) # Not doing enough, next line corrects that (from wx-config --libs)
target_link_libraries(${PROJECT_NAME} ur_rtde::rtde)
target_link_libraries(${PROJECT_NAME} -L/usr/local/lib -pthread   -lwx_gtk3u_xrc-3.0 -lwx_gtk3u_html-3.0 -lwx_gtk3u_qa-3.0 -lwx_gtk3u_adv-3.0 -lwx_gtk3u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0)
