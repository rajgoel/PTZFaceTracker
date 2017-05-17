cmake_minimum_required(VERSION 2.8)
project( FT )
find_package( OpenCV REQUIRED )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

add_executable( PTZFaceTracker PTZFaceTracker.cpp FaceTracker.cpp PTZController.cpp http.cpp )
target_link_libraries( PTZFaceTracker ${OpenCV_LIBS} )


