CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(IScroll)

ADD_EXECUTABLE(iscroll iscroll.cpp image.cpp image.h detection.h detection.cpp Gaze_tracker.h Gaze_tracker.cpp camshift_tracker.h camshift_tracker.cpp)
TARGET_LINK_LIBRARIES(iscroll ${OpenCV_LIBS})

