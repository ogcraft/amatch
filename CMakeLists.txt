cmake_minimum_required (VERSION 2.6) 
project (amatch)

message( STATUS "amatch dir: " ${PROJECT_SOURCE_DIR} )

if(WIN32)
    set(CMAKE_CXX_LINKER "${VCINSTALLDIR}/VC/bin/link.exe")

#SET(CMAKE_CXX_CREATE_SHARED_LIBRARY
#  "\"${CMAKE_CXX_LINKER}\" ${CMAKE_CL_NOLOGO} ${CMAKE_START_TEMP_FILE}
# /out:<TARGET> /PDB:<TARGET_PDB> /dll
# /version:<TARGET_VERSION_MAJOR>.<TARGET_VERSION_MINOR> <LINK_FLAGS>
# <OBJECTS> <LINK_LIBRARIES> ${CMAKE_END_TEMP_FILE}")

#SET(CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_LIBRARY}")
#SET(CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY}")
#SET(CMAKE_C_CREATE_SHARED_MODULE "${CMAKE_C_CREATE_SHARED_LIBRARY}")

	include_directories ( ${AMATCH_DIR}/include )

endif(WIN32)


set(AMATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

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
