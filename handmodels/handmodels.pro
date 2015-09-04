include(../common/common.prf)
include(../external/qglviewer.prf)

LIBS += -framework GLUT

SOURCES += main.cpp \
    HandModel_Cylinders.cpp

HEADERS += \
    HandModel.h \
    HandModel_Cylinders.h \
    QAutoGUI.h
