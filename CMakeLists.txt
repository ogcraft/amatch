cmake_minimum_required (VERSION 2.6) 
project (amatch)

set (CMAKE_BUILD_TYPE Debug)
#set (CMAKE_BUILD_TYPE Release)

set(AMATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

if (MSVC)
        add_definitions (/wd4996 /D "_CRT_SECURE_NO_WARNINGS")
endif (MSVC)


#find_package(Boost)
# If Boost is not found then Boost_INCLUDE_DIRS will be set
# to "NOTFOUND" which will make CMake complain on Windows.
# if (BOOST_FOUND)
#   set (BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
#endif (BOOST_FOUND)

message( STATUS "amatch dir: " ${AMATCH_DIR} )

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

	include_directories ( ${AMATCH_DIR}/lib/win32/include ${AMATCH_DIR}/include ${AMATCH_DIR}/src )
    link_directories    ( ${AMATCH_DIR}/lib/win32 )

else(WIN32)

	include_directories ( ${AMATCH_DIR}/src ${AMATCH_DIR}/include )
	link_directories    ( ${AMATCH_DIR}/lib ${AMATCH_DIR}/Debug)

endif(WIN32)


#add_library( libamatch STATIC 
#    include/amatch.h
#    src/asearch.cpp
#    src/amatch.cpp
#	src/utils.h
#	src/utils.cpp
#	src/sigproc.h
#	src/sigproc.cpp
#	src/filters.h
#	src/filters.cpp
#)

#add_executable (amatch_sample 
#	bin/amatch_sample.cpp 
#    )

#add_executable (amatch_sample_double 
#	bin/amatch_sample_double.cpp 
#    )

#target_link_libraries ( amatch_sample
#	libamatch
#    fftw3
#    sndfile
#	)   

#target_link_libraries ( amatch_sample_double
#	libamatch
#    fftw3
#    sndfile
#	)   

add_executable (generate_fpkeys 
	bin/generate_fpkeys.cpp
	src/asearch.cpp
	src/utils.cpp
	src/sigproc.cpp
	src/filters.cpp
	)


#add_executable (split_silence 
#    bin/split_silence.cpp 
#    )

#target_link_libraries ( split_silence
#    libamatch
#    fftw3
#    sndfile
#    )


if(WIN32)
    target_link_libraries ( generate_fpkeys
	    #libamatch
            libfftw3-3 # win32
            libsndfile-1.lib # win32 
            #boost_iostreams 
            #boost_system 
            )   
else(WIN32)
    target_link_libraries ( generate_fpkeys
	    #libamatch
            fftw3
            sndfile 
            #boost_iostreams 
            #boost_system 
            )   
endif(WIN32)

