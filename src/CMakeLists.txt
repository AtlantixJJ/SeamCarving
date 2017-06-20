SET(TOOLS tools)
find_package( OpenCV REQUIRED )

AUX_SOURCE_DIRECTORY(carver ENGINE_SRCS)
ADD_LIBRARY(engine ${ENGINE_SRCS})

ADD_EXECUTABLE(SeamCarver ${TOOLS}/SeamCarver.cpp)
TARGET_LINK_LIBRARIES(SeamCarver ${OpenCV_LIBS} engine)

ADD_EXECUTABLE(testor ${TOOLS}/test.cpp)
TARGET_LINK_LIBRARIES(testor ${OpenCV_LIBS})

