set(CMAKE_BUILD_TYPE "Release")
# 加载米文相机头文件
# include_directories("/opt/miivii/include/")
file(GLOB srcs "*.cpp")
file(GLOB hdrs "*.h")
ug_library( modules_surroundview_camera_miivii
    USE_OPENCV
    SRCS 
      ${srcs}
    HDRS 
      ${hdrs}
    DEPENDS
      /opt/miivii/lib/libmvgmslcamera.so   
)