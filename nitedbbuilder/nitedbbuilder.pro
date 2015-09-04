include(../common/common.prf) # Do we need this here?
include(../external/opencv.prf)
include(../external/openni.prf)
include(../external/nite2.prf)

HEADERS *= $$PWD/../common/KinectHelper.h \
HEADERS += WorkHelper.h \
    NiteHelper.h
SOURCES *= $$PWD/../common/KinectHelper.cpp \
    NiteHelper.cpp
SOURCES += main.cpp

DEFINES += NOSYNC
