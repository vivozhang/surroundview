set(CMAKE_BUILD_TYPE "Release")

file(GLOB srcs "control.cpp")
file(GLOB hdrs "control.h")

# 加载米文相机头文件
# include_directories("/opt/miivii/include/")
# file(GLOB srcs "*.cpp")
# file(GLOB hdrs "*.h")

ug_library( modules_surroundview_control
    USE_OPENCV
    SRCS 
      ${srcs}
    HDRS 
      ${hdrs}
    DEPENDS
      modules_common_proto_surroundview_control_proto
      modules_common_proto_hmi_msg_proto
      modules_canbus_protocol_mixer408_mixer_chassis_protocol
      modules_common_adapters_adapter_gflags
      cyber
      cyber_node
)