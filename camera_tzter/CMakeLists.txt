set(CMAKE_BUILD_TYPE "Release")
file(GLOB srcs "*.c")
file(GLOB hdrs "*.h")

#加载天准相机
ug_library(modules_surroundview_camera_tzter
    SRCS 
      ${srcs}
    HDRS 
      ${hdrs}
    DEPENDS
      # /lib/aarch64-linux-gnu/libpthread.so.0
)