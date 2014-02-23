cmake_minimum_required (VERSION 2.6) 
project (amatch)

set(AMATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

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

	include_directories ( ${AMATCH_DIR}/src ${AMATCH_DIR}/include )
    link_directories    ( ${AMATCH_DIR}/lib/win32 )

else(WIN32)

    link_directories    ( ${AMATCH_DIR}/lib )

endif(WIN32)


add_library( libamatch
    include/amatch.h
    src/asearch.cpp
    src/amatch.cpp
	src/utils.h
	src/utils.cpp
	src/sigproc.h
	src/sigproc.cpp
	src/filters.h
	src/filters.cpp
)

add_executable (generate_fpkeys 
	bin/generate_fpkeys.cpp 
    )

add_executable (amatch 
	bin/amatch_main.cpp 
    )

target_link_libraries ( amatch
        libamatch.lib
        )   
if(WIN32)
    target_link_libraries ( generate_fpkeys
            libamatch.lib
            libfftw3-3 # win32
            libsndfile-1.lib # win32 
            #boost_iostreams 
            #boost_system 
            )   
else(WIN32)
    target_link_libraries ( generate_fpkeys
            libamatch.lib
            fftw3
            sndfile 
            #boost_iostreams 
            #boost_system 
            )   
endif(WIN32)

