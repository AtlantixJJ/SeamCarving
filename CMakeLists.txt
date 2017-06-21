cmake_minimum_required(VERSION 2.8)

PROJECT(SeamCarving)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( include )
ADD_SUBDIRECTORY(src)
