cmake_minimum_required (VERSION 2.6) 
project (amatch)

message( STATUS "${PROJECT_SOURCE_DIR}: " ${PROJECT_SOURCE_DIR} )

set(AMATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

if(WIN32)
	include_directories ( ${AMATCH_DIR}/include )
endif(WIN32)

link_directories    ( ${AMATCH_DIR}/lib )

add_library(amatch
	src/utils.h
	src/utils.cpp
	src/sigproc.h
	src/sigproc.cpp
	src/filters.h
	src/filters.cpp
)

#add_executable (asearch 
#	asearch.cpp 
#	utils.cpp
#	sigproc.cpp
#	filters.cpp
#    )

#add_executable (amatch 
#	amatch.cpp 
#	)

#if(WIN32)
#	target_link_libraries (generate_pfkeys 
#		libfftw3-3
#		libsndfile-1.lib 
#		#samplerate 
#		#armadillo 
#		#boost_iostreams 
#		#boost_system 
#		)
#else(WIN32)
#	target_link_libraries ( generate_pfkeys
#		fftw3
#		sndfile 
#		#samplerate 
#		#armadillo 
#		#boost_iostreams 
#		#boost_system 
#		)   
#endif(WIN32)
