include(../common/common.prf)
include(../external/openni.prf)
include(../external/qglviewer.prf)

LIBS += -framework GLUT
INCLUDEPATH += $$PWD/../handmodels

HEADERS += $$PWD/../common/KinectHelper.h \
    WorkHelper.h \
    $$PWD/../handmodels/HandModel.h \
    $$PWD/../handmodels/HandModel_Cylinders.h \
    HandProjection.h
SOURCES += $$PWD/../common/KinectHelper.cpp \
    $$PWD/../handmodels/HandModel_Cylinders.cpp

SOURCES += main.cpp
