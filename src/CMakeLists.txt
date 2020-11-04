cmake_minimum_required(VERSION 3.10.2) # Ubuntu 18.04 default right now? My 20.04 is 3.16.3
project(KasteRobot)

# Using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Building as debug for now (my robworks is not though, may be a problem)
set(CMAKE_BUILD_TYPE Debug)

# wxWidgets library
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include("${wxWidgets_USE_FILE}")

# MySQL xDevAPI dependency
find_package(OpenSSL REQUIRED)

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
    ${PROJECT_SOURCE_DIR}/app.hpp
    )

# GUI layer source files GLOB
file(GLOB GUI_FILES
    ${PROJECT_SOURCE_DIR}/gui/cmain.cpp
    ${PROJECT_SOURCE_DIR}/gui/cmain.hpp

    ${PROJECT_SOURCE_DIR}/gui/cimagepanel.cpp
    ${PROJECT_SOURCE_DIR}/gui/cimagepanel.hpp

    ${PROJECT_SOURCE_DIR}/gui/cidbindings.hpp
    )

# Logic layer source files GLOB
file(GLOB LOGIC_FILES
    ${PROJECT_SOURCE_DIR}/logic/xcontroller.cpp
    ${PROJECT_SOURCE_DIR}/logic/xcontroller.hpp

    ${PROJECT_SOURCE_DIR}/logic/xbaslercam.cpp
    ${PROJECT_SOURCE_DIR}/logic/xbaslercam.hpp

    ${PROJECT_SOURCE_DIR}/logic/ximagehandler.cpp
    ${PROJECT_SOURCE_DIR}/logic/ximagehandler.hpp

    ${PROJECT_SOURCE_DIR}/logic/xurcontrol.cpp
    ${PROJECT_SOURCE_DIR}/logic/xurcontrol.hpp

    ${PROJECT_SOURCE_DIR}/logic/xgripperclient.cpp
    ${PROJECT_SOURCE_DIR}/logic/xgripperclient.hpp

    ${PROJECT_SOURCE_DIR}/logic/xcollisiondetector.hpp
    ${PROJECT_SOURCE_DIR}/logic/xcollisiondetector.cpp

    ${PROJECT_SOURCE_DIR}/logic/xexceptions.hpp

    ${PROJECT_SOURCE_DIR}/logic/xinfostruct.hpp

    ${PROJECT_SOURCE_DIR}/logic/xmath.cpp
    ${PROJECT_SOURCE_DIR}/logic/xmath.hpp
    )

# Database layer source files GLOB
file (GLOB DB_FILES
     ${PROJECT_SOURCE_DIR}/database/qdatabasehandler.cpp
     ${PROJECT_SOURCE_DIR}/database/qdatabasehandler.hpp
    )

# Creating executable, linked with source files
# CMake complains here: Files in some directories may conflict with implicit libraries (librtde.so.1.3 in /usr/lib/x86_64-linux-gnu hidden by /usr/local/lib)
# Might be a local mistake on my machine due to multiples of the same lib
add_executable(${PROJECT_NAME} ${SRC} ${GUI_FILES} ${LOGIC_FILES} ${DB_FILES})

# Some more wxWidgets compiling stuff (wx-config --cxxflags)
set(GCC_COVERAGE_COMPILE_FLAGS "-I/usr/local/lib/wx/include/gtk3-unicode-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# Linking libraries
target_link_libraries(${PROJECT_NAME} PRIVATE mysqlcppconn8 OpenSSL::SSL resolv crypto)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Pylon_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ROBWORK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES}) # Not doing enough, next line corrects that (from wx-config --libs)
target_link_libraries(${PROJECT_NAME} PRIVATE ur_rtde::rtde)
target_link_libraries(${PROJECT_NAME} PRIVATE -L/usr/local/lib -pthread -lwx_gtk3u_xrc-3.0 -lwx_gtk3u_html-3.0 -lwx_gtk3u_qa-3.0 -lwx_gtk3u_adv-3.0 -lwx_gtk3u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0)
