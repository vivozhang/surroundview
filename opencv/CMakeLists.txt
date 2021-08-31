set(GOOG_CXX_FLAGS "-std=c++11 -O4 -Wint-conversion -Wno-narrowing")
set(CMAKE_BUILD_TYPE "Release")
include_directories(include)

file(GLOB srcs "src/*cpp")
file(GLOB hdrs "include/*.h")

ug_library(modules_surroundview_opencv
  SRCS 
    ${srcs}
  HDRS
    ${hrds}
)

