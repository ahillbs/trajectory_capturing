cmake_minimum_required(VERSION 3.4)

# if the build type is not set, set it to RelWithDebInfo
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# if we are not given the path to the alg/util project root, assume its in the parent directory/util
if(NOT UTIL_ALGUTIL_ROOT)
	if(NOT "$ENV{ALG_UTIL_ROOT}" STREQUAL "")
		set(UTIL_ALGUTIL_ROOT "$ENV{ALG_UTIL_ROOT}")
	else()
		set(UTIL_ALGUTIL_ROOT ${CMAKE_CURRENT_LIST_DIR}/../util)
	endif()
endif()

# if we are not given the path to the alg/cplex project root, assume its in the parent directory/cplex
if(NOT UTIL_ALGCPLEX_ROOT)
	if(NOT "$ENV{ALG_CPLEX_ROOT}" STREQUAL "")
		set(UTIL_ALGCPLEX_ROOT "$ENV{ALG_CPLEX_ROOT}")
	else()
		set(UTIL_ALGCPLEX_ROOT ${CMAKE_CURRENT_LIST_DIR}/../cplex)
	endif()
endif()

# store the current list dir for later use
set(UTIL_HELPER_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# add this directory to the module path so our modules can be found
set(CMAKE_MODULE_PATH ${UTIL_HELPER_DIRECTORY} ${CMAKE_MODULE_PATH})

if(UTIL_VERBOSE)
	message(STATUS "Module path: ${CMAKE_MODULE_PATH}")
endif()

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
		if(OPTIMIZE_FOR_OLD_ALGPCS)
			if(UTIL_VERBOSE)
				message(STATUS "Optimizing for old algpcs (05-08)")
			endif()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=core-avx2 -mtune=core-avx2 -mcx16 -msahf -mmovbe -maes -mpclmul -mpopcnt -mabm -mno-lwp -mfma -mno-fma4 -mno-xop -mbmi -mbmi2 -mno-tbm -mavx -mavx2 -msse4.2 -msse4.1 -mlzcnt -mrtm -mhle -mrdrnd -mf16c -mfsgsbase -mno-rdseed -mno-prfchw -mno-adx -mfxsr -mxsave -mxsaveopt --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=8192")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=core-avx2 -mtune=core-avx2 -mcx16 -msahf -mmovbe -maes -mpclmul -mpopcnt -mabm -mno-lwp -mfma -mno-fma4 -mno-xop -mbmi -mbmi2 -mno-tbm -mavx -mavx2 -msse4.2 -msse4.1 -mlzcnt -mrtm -mhle -mrdrnd -mf16c -mfsgsbase -mno-rdseed -mno-prfchw -mno-adx -mfxsr -mxsave -mxsaveopt --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=8192")
		elseif(OPTIMIZE_FOR_NEW_ALGPCS)
			if(UTIL_VERBOSE)
				message(STATUS "Optimizing for new algpcs (01-04)")
			endif()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=broadwell -mmmx -msse -msse2 -msse3 -mssse3 -mcx16 -msahf -mmovbe -maes -mpclmul -mpopcnt -mabm -mfma -mbmi -mbmi2 -mavx -mavx2 -msse4.2 -msse4.1 -mlzcnt -mrtm -mhle -mrdrnd -mf16c -mfsgsbase -mrdseed -mprfchw -madx -mfxsr -mxsave -mxsaveopt -mclflushopt -mxsavec -mxsaves --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=8192")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -march=broadwell -mmmx -msse -msse2 -msse3 -mssse3 -mcx16 -msahf -mmovbe -maes -mpclmul -mpopcnt -mabm -mfma -mbmi -mbmi2 -mavx -mavx2 -msse4.2 -msse4.1 -mlzcnt -mrtm -mhle -mrdrnd -mf16c -mfsgsbase -mrdseed -mprfchw -madx -mfxsr -mxsave -mxsaveopt -mclflushopt -mxsavec -mxsaves --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=8192")
		else()
			if(UTIL_VERBOSE)
				message(STATUS "Optimizing for host machine")
			endif()

			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -mtune=native")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")

			if(UNIX AND APPLE AND CMAKE_COMPILER_IS_GNUCXX)
				set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-q")
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-q")
			endif()
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

# set UTIL_INCLUDE_DIRS so that the alg/cplex and alg/util projects are ready to use in targets prepared with util_setup_target
set(UTIL_INCLUDE_DIRS ${UTIL_INCLUDE_DIRS} ${UTIL_ALGCPLEX_ROOT}/include ${UTIL_ALGUTIL_ROOT}/include)

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

# generate the header file "shared_library_util.h" (compatible to both C and C++) in the directory OUTPUTDIR;
# this header should be included in every compilation unit (ideally in config header) of the shared library.
# the macros "toupper(${LIBNAME})_API_(PUBLIC|PRIVATE)" can then be used to hide/export symbols.
function(util_setup_shared_library TARGETNAME LIBNAME OUTPUTDIR)
	# generate header file (needs to know UPPERCASE_TARGETNAME to make symbol unique)
	string(TOUPPER "${LIBNAME}" UPPERCASE_TARGETNAME)
	configure_file("${UTIL_HELPER_DIRECTORY}/shared_library_util.h" "${OUTPUTDIR}/${LIBNAME}/shared_library_util.h")
	
	# compile-time definition when building the shared library (DO NOT DEFINE THIS WHEN LINKING AGAINST SHARED LIBRARY)
	set(DEFINE_SHARED_SYMBOL "${UPPERCASE_TARGETNAME}_BUILD_SHARED")
	target_compile_definitions(${TARGETNAME} PRIVATE "${DEFINE_SHARED_SYMBOL}=1")

	# bring the generated header into the include path
	target_include_directories(${TARGETNAME} PRIVATE "${OUTPUTDIR}")
	
	# default to hidden symbols on non-Windows systems (Windows symbols are hidden by default)
	if(NOT WIN32 AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
		target_compile_options(${TARGETNAME} PRIVATE "-fvisibility=hidden" "-fvisibility-inlines-hidden")
		get_target_property(EXISTING_LINK_FLAGS ${TARGETNAME} LINK_FLAGS)
		if(NOT EXISTING_LINK_FLAGS)
			set(EXISTING_LINK_FLAGS "")
		endif()
		
		set(ADDITIONAL_LINK_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
		
		# On Linux, there is a nice way to hide symbols from static libraries (like CPLEX) we link against
		# This unfortunately does not work on Mac where we would have to build an export symbol file
		if(UNIX AND NOT APPLE)
			set(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--exclude-libs=ALL")
		endif()
		
		set_target_properties(${TARGETNAME} PROPERTIES LINK_FLAGS "${EXISTING_LINK_FLAGS} ${ADDITIONAL_LINK_FLAGS}")
	endif()
	
	set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${LIBNAME} DIRECT_OUTPUT 1)
endfunction(util_setup_shared_library)

# if util_setup_shared_library is used and a static version of the library is built, use util_setup_static_library on the static library target
function(util_setup_static_library TARGETNAME LIBNAME)
	string(TOUPPER "${LIBNAME}" UPPERCASE_TARGETNAME)
	set(DEFINE_STATIC_SYMBOL "${UPPERCASE_TARGETNAME}_STATIC")
	target_compile_definitions(${TARGETNAME} PUBLIC "${DEFINE_STATIC_SYMBOL}=1")
	set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${LIBNAME} DIRECT_OUTPUT 1)
endfunction(util_setup_static_library)
