
cmake_minimum_required(VERSION 2.8)

project(GripperClient)

set ( project_sources
    main.cpp
    gripperclient.cpp
    gripperclient.h )

add_executable(${PROJECT_NAME}
    ${project_sources}
    )

target_link_libraries(${PROJECT_NAME} pthread)
