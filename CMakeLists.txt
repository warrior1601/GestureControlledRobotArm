cmake_minimum_required (VERSION 3.1)
cmake_policy(SET CMP0012 NEW)
PROJECT( project )

#Requires OpenCV
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")


add_library( Addons	source/LinkerLanguage.cpp;
			source/trackBars.h; source/drawings.h;)

ADD_EXECUTABLE(sample source/main.cpp)


#include_directories( include )
include_directories(${OpenCV_INCLUDE_DIRS})

#link_directories(${OpenCV_LIB_DIR})

target_link_libraries(sample PRIVATE ${OpenCV_LIBS} 
					Addons)
