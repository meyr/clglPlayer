CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME clglPlayer)
PROJECT(${PROJECTNAME})
SET(SRCS main.c opengl.c opencl.c algorithm.c shader.c utility.c decode.c)
FILE(GLOB_RECURSE HEADERS "*.h")
SET(CODEC_LIB avutil avformat avcodec swscale)
SET(OPENCL_LIB OpenCL)
if (UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
	SET(OPENGL_LIB GL glut GLU)
	add_custom_target(myCustomTarget
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/algorithm.cl .)
endif (UNIX)
if (WIN32)
	# hader file search path
	SET(VSINC "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/include/")
	include_directories(${VSINC})
	# lib file 
	SET(OPENGL_LIB freeglut glew32 glut32)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
	add_custom_target(myCustomTarget
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/algorithm.cl .
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/dll/* .
		)

endif(WIN32)
ADD_EXECUTABLE(${PROJECTNAME} ${SRCS} ${HADERS})
add_dependencies(${PROJECTNAME} myCustomTarget)
#ADD_DEFINITIONS(-DCL_SOURCE_DIR="${showImage_SOURCE_DIR}")
#message("source path: ${showImage_SOURCE_DIR}\n")
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${OPENGL_LIB} ${OPENCL_LIB} ${CODEC_LIB}) 
#ADD_LIBRARY(my SHARED ${hello_SRCS})
#ADD_LIBRARY(my ${hello_SRCS})
