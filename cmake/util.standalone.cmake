cmake_minimum_required(VERSION 3.4)

# store the current list dir for later use
set(UTIL_HELPER_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# add this directory to the module path so our modules can be found
set(CMAKE_MODULE_PATH ${UTIL_HELPER_DIRECTORY} ${CMAKE_MODULE_PATH})

# set C++ standard to C++14
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# enable warnings
if(MSVC)
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
	
	if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
	endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-long-long")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-long-long -Wno-missing-braces")
endif()

# enable platform specific optimizations on GCC-like compilers
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT NO_HOST_SPECIFIC_OPTIMIZATIONS)
	if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -mtune=native")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")

		if(UNIX AND APPLE AND CMAKE_COMPILER_IS_GNUCXX)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-q")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-q")
		endif()
		
		# linker optimizations (do not confuse with link time optimizations!)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O1")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -O1")

		if(UNIX AND NOT APPLE)
			message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
			if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
				set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
				set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
			endif()
		endif()
		
		# some (possibly linux-specific) linker flags
		if(UNIX AND NOT APPLE)
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed -Wl,--hash-style=gnu")
		endif()
	endif()
endif()

# threading support (use Threads::Threads as library name, or extend/use LIBRARIES variable); for instance, this pulls in pthread on Linux
find_package(Threads REQUIRED)
set(UTIL_LIBRARIES Threads::Threads)

# finding boost requires some non-trivial stuff on some platforms
if(UTIL_BOOST_LIBRARIES)
	set(Boost_DEBUG OFF)
	
	if(WIN32 OR UTIL_STATIC_BOOST)
		set(Boost_USE_STATIC_LIBS ON)
	endif()
	
	if(WIN32)
		if(UTIL_STATIC_RUNTIME_BOOST)
			set(Boost_USE_STATIC_RUNTIME ON)
		else()
			set(Boost_USE_STATIC_RUNTIME OFF)
		endif()
	endif()
	
	set(Boost_USE_MULTITHREADED ON)

	find_package(Boost REQUIRED COMPONENTS ${UTIL_BOOST_LIBRARIES})
	set(UTIL_LIBRARIES ${UTIL_LIBRARIES} ${Boost_LIBRARIES})
	set(UTIL_INCLUDE_DIRS ${UTIL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
endif()

# Generally, on Linux, the math library may be required
if(NOT UTIL_NO_MATH_LIBRARY)
	if(UNIX)
		find_library(UTIL_LIBM_LIBRARY_ NAMES m)
		if(UTIL_LIBM_LIBRARY_)
			if(UTIL_VERBOSE)
				message(STATUS "Using math library: ${UTIL_LIBM_LIBRARY_}")
			endif()

			set(UTIL_LIBRARIES ${UTIL_LIBRARIES} ${UTIL_LIBM_LIBRARY_})
		endif()
	endif()
endif()

# In many cases, a dynamic linking library is necessary (libdl on Linux)
if(NOT UTIL_NO_DL_LIBRARY)
	if(UNIX AND NOT APPLE)
		find_library(UTIL_LIBDL_LIBRARY_ NAMES dl)
		if(UTIL_LIBDL_LIBRARY_)
			if(UTIL_VERBOSE)
				message(STATUS "Using dynamic linker library: ${UTIL_LIBDL_LIBRARY_}")
			endif()
			set(UTIL_LIBRARIES ${UTIL_LIBRARIES} ${UTIL_LIBDL_LIBRARY_})
		endif()
	endif()
endif()

# use the util_setup_target function to add libraries and include dirs found/setup here to the given target
function(util_setup_target TARGETNAME)
	if(NOT "${UTIL_LIBRARIES}" STREQUAL "")
		target_link_libraries(${TARGETNAME} ${UTIL_LIBRARIES})
	endif()
	
	if(NOT "${UTIL_INCLUDE_DIRS}" STREQUAL "")
		target_include_directories(${TARGETNAME} PRIVATE ${UTIL_INCLUDE_DIRS})
	endif()
endfunction(util_setup_target)

