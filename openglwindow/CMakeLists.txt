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

ug_binary(openglwindow
    USES_GLOG
    USES_ABSL
    SRCS 
      ${srcs}
      ${MOCS}
    HDRS 
      ${hdrs}
    DEPENDS
      cyber
      cyber_node
      cyber_scheduler
      modules_common_time
      modules_surroundview_control
      modules_surroundview_opencv
      modules_common_proto_hmi_msg_proto
      modules_surroundview_camera_tzter
      # modules_surroundview_camera_miivii
      ${Qt5Widgets_LIBRARIES}
      GL
)