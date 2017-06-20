cmake_minimum_required(VERSION 3.5)

PROJECT(SeamCarving)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( include )
ADD_SUBDIRECTORY(src)
