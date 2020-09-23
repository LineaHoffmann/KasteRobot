cmake_minimum_required(VERSION 3.5)
project(DB_conn)

#find_package(OpenCV REQUIRED)

file(GLOB_RECURSE src
	"*.h"
	"*.cpp"
	)

add_executable(${PROJECT_NAME} ${src})


target_link_libraries(${PROJECT_NAME} mysqlclient)

