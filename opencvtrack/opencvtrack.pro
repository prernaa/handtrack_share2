include(../common/common.prf) # Do we need this here?
include(../external/opencv.prf)
include(../external/openni.prf)

HEADERS *= $$PWD/../common/KinectHelper.h \
HEADERS += WorkHelper.h
SOURCES *= $$PWD/../common/KinectHelper.cpp

HEADERS += detector.h
SOURCES += main.cpp \
    detector.cpp
