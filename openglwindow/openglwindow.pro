include(openglwindow.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH +=  /usr/include/opencv4/    \
                                      /opt/miivii/include/          \
                                      ../../../apollo-qmake/genfiles/modules/surroundview/proto/  \
                                      ../../../apollo-qmake/genfiles/     \
                                      ../../../     \
                                     ../../../apollo-qmake/third_party/include/            \
                                     ../control/      \
                                     ../../hmi/        \
                                    
SOURCES += \
    InitMosaic.cpp \
    esTransform.cpp \
    main.cpp \
    mtlloader.cpp \
    objloader.cpp \
    render.cpp \
    tinystr.cpp \
    tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp \
    xmltest.cpp               \
   #../control/camera4.1.cpp \
   #../control/camera3.2.cpp \
    #../control/control.cpp        \
   #/home/sany/workspace/syautodrive/apollo-qmake/genfiles/modules/surroundview/proto/surroundview_control.pb.cc \
   #/home/sany/workspace/syautodrive/apollo-qmake/genfiles/modules/surroundview/proto/surroundview_err.pb.cc \


#LIBS +=/surrdata/avm/rgba_0616/triangle1/opencv/lib/libopencv_local_lib.so
LIBS+=-L$$PWD/../opencv/lib/ -lopencv_local_lib

# opencv 3.2 version
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_core.so.3.2
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_highgui.so.3.2
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_imgcodecs.so.3.2
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_imgproc.so.3.2
#LIBS +=-L$$PWD../../../apollo-qmake/third_party/lib/aarch64/ -lopencv_core  -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc

# opencv 4.1 version
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_core.so.4.1.1
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_highgui.so.4.1.1
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_imgcodecs.so.4.1.1
#LIBS +=/usr/lib/aarch64-linux-gnu/libopencv_imgproc.so.4.1.1

#LIBS +=/opt/miivii/lib/libmvgmslcamera.so

#LIBS +=-Wl,-rpath=/apollo/modules/drivers/gnssins -L/apollo/modules/drivers/gnssins -lgnssins_data_component
#LIBS +=-Wl,-rpath=/apollo/modules/common -L/apollo/modules/common -lmodules_common
#LIBS +=-Wl,-rpath=/apollo/modules -L/apollo/modules -lmodules_proto
#LIBS +=-Wl,-rpath=/apollo -L/apollo -lcyber

#LIBS += -lprotobuf
#LIBS += -labsl
#LIBS += -lglog
#LIBS += -lgflags
#LIBS += -lfastrtps
#LIBS += -lfastcdr
#LIBS += -lsqlite3
#LIBS += -losqp
#LIBS += -lqpOASES
#LIBS += -L/opt/aarch64/