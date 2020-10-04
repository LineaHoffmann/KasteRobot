cmake_minimum_required(VERSION 3.10.2) # Ubuntu 18.04 default right now? My 20.04 is 3.16.3
project(KasteRobotGUI)

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
#    ${PROJECT_SOURCE_DIR}/logic/xbaslercam.cpp
#    ${PROJECT_SOURCE_DIR}/logic/xbaslercam.h

    ${PROJECT_SOURCE_DIR}/logic/xlinker.cpp
    ${PROJECT_SOURCE_DIR}/logic/xlinker.h
    )

# Database layer source files GLOB
file (GLOB DB_FILES
    )

# Creating executable, linked with source files
add_executable(${PROJECT_NAME} ${SRC} ${GUI_FILES} ${LOGIC_FILES} ${DB_FILES})

# Some more wxWidgets compiling stuff (wx-config --cxxflags)
set(GCC_COVERAGE_COMPILE_FLAGS "-I/usr/local/lib/wx/include/gtk2-unicode-static-3.0 -I/usr/local/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -D__WXGTK__ -pthread")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# Linking libraries
target_link_libraries(${PROJECT_NAME} ${Pylon_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${ROBWORK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES}) # Not doing enough, next line corrects that (from wx-config --libs)
target_link_libraries(${PROJECT_NAME} -L/usr/local/lib -pthread   /usr/local/lib/libwx_gtk2u_xrc-3.0.a /usr/local/lib/libwx_gtk2u_qa-3.0.a /usr/local/lib/libwx_baseu_net-3.0.a /usr/local/lib/libwx_gtk2u_html-3.0.a /usr/local/lib/libwx_gtk2u_adv-3.0.a /usr/local/lib/libwx_gtk2u_core-3.0.a /usr/local/lib/libwx_baseu_xml-3.0.a /usr/local/lib/libwx_baseu-3.0.a -lgthread-2.0 -pthread -lX11 -lSM -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lfontconfig -lfreetype -lpng -ljpeg -ltiff -lexpat -lwxregexu-3.0 -lz -ldl -lm)
