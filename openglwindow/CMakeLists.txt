cmake_minimum_required(VERSION 3.10)
project(surroundview)
set(CMAKE_BUILD_TYPE "Release")
set(GOOG_CXX_FLAGS "-std=c++11 -O4 -Wint-conversion -Wno-narrowing")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
if(Qt5Widgets_FOUND)
  message(STATUS ${Qt5Widgets_INCLUDE_DIRS})
  message(STATUS ${Qt5Core_INCLUDE_DIRS})
endif()


file(GLOB srcs "*.cpp")
file(GLOB hdrs "*.h" "*.hpp")

# include_directroies(${CMAKE_CURRENT_BINARY_DIR} ${hdrs} ${APOLLO_DIR})
# add_executable(${PROJECT_NAME} ${srcs} )

# # #为target添加需要链接的共享库
# target_link_libraries(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES})
# ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
#宏转化
qt5_wrap_cpp(MOCS openglwindow.h)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${srcs} ${MOCS} )
target_link_libraries(
                                           ${PROJECT_NAME} 
                                           ${Qt5Widgets_LIBRARIES}
                                           /usr/lib/x86_64-linux-gnu/libGL.so.1
                                           )