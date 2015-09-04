TEMPLATE = subdirs
CONFIG += ordered

# this one just show the RGB image
SUBDIRS += simple

# this one tests the hand model
SUBDIRS += handmodels

# this one uses nite and dumps tracked images into a folder
#SUBDIRS += nitedbbuilder

# this show the image + tracking with opencv
#SUBDIRS += opencvtrack

# 
#SUBDIRS += kinectrecorder

#
#SUBDIRS += handprojection

#
#SUBDIRS += boxprojection

#SUBDIRS += handmodels_fbx

#SUBDIRS += handmodels_mesh
# tracks a model from "offline" data.
# offer a way of "scrubbing" through frames
#SUBDIRS += offlinetracker
