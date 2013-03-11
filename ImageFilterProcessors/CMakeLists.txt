cmake_minimum_required(VERSION 2.8 )

include_directories(/usr/include/Qt /usr/local/include)
link_directories(/usr/local/lib)

add_executable(imagefilter main.cpp EasyImageFilter.cpp NoiseReduction.cpp)
target_link_libraries(imagefilter
		opencv_core opencv_highgui
		QtGui QtCore )
