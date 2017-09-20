# CMake module for find_package(OpenCV)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   OPENCV_INCLUDE_DIR
#   OPENCV_LIBLSIT

find_path(OPENCV_INCLUDE_DIR
    NAMES cv.h
    PATH_SUFFIXES opencv
)

find_library(OPENCV_CORE_LIBRARY              NAMES opencv_core)
find_library(OPENCV_VIDEOSTAB_LIBRARY         NAMES opencv_videostab)
find_library(OPENCV_VIDEO_LIBRARY             NAMES opencv_video)
find_library(OPENCV_SUPERRES_LIBRARY          NAMES opencv_superres)
find_library(OPENCV_STITCHING_LIBRARY         NAMES opencv_stitching)
find_library(OPENCV_PHOTO_LIBRARY             NAMES opencv_photo)
find_library(OPENCV_OBJDETECT_LIBRARY         NAMES opencv_objdetect)
find_library(OPENCV_ML_LIBRARY                NAMES opencv_ml)
find_library(OPENCV_IMGCODECS_LIBRARY         NAMES opencv_imgcodecs)
find_library(OPENCV_IMGPROC_LIBRARY           NAMES opencv_imgproc)
find_library(OPENCV_CALIB3D_LIBRARY           NAMES opencv_calib3d)
find_library(OPENCV_FEATURES2D_LIBRARY        NAMES opencv_features2d)
find_library(OPENCV_XFEATURES2D_LIBRARY       NAMES opencv_xfeatures2d)
find_library(OPENCV_HIGHGUI_LIBRARY           NAMES opencv_highgui)
find_library(OPENCV_FLANN_LIBRARY             NAMES opencv_flann)

get_filename_component(OPENCV_ROOT_INCLUDE_DIR "${OPENCV_INCLUDE_DIR}" DIRECTORY)
