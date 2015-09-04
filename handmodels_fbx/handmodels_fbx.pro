include(../common/common.prf)
include(../external/qglviewer.prf)
include(../external/surfacemesh/surfacemesh.prf)
include(../external/fbxsdk.prf)

SOURCES += main.cpp

HEADERS += \
    Tree.h \
    Fbx_IO.h \
    Fbx_IO_SurfaceMesh.h \
    Fbx_IO_Skeleton.h
