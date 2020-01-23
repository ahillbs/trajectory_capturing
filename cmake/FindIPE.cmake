include(FindPackageHandleStandardArgs)

set(IPE_FIND_ERROR FALSE)

find_path(IPE_INCLUDE_DIR NAMES ipelib.h HINTS ${IPE_ROOT}/include)
if(NOT IPE_INCLUDE_DIR)
	set(IPE_FIND_ERROR TRUE)
else()
	if(NOT IPE_FIND_QUIETLY)
		message(STATUS "IPE include directory: ${IPE_INCLUDE_DIR}")
	endif()
endif()

find_library(IPE_LIBRARY NAMES ipe HINTS ${IPE_ROOT}/lib /Applications/Ipe.app/Contents/Frameworks)
if(NOT IPE_LIBRARY)
	set(IPE_FIND_ERROR TRUE)
else()
	if(NOT IPE_FIND_QUIETLY)
		message(STATUS "IPE library: ${IPE_LIBRARY}")
	endif()
endif()

if(UNIX)
	set(IPE_LIBRARIES "dl")
endif()

if(NOT IPE_FIND_ERROR)
	set(IPE_LIBRARIES ${IPE_LIBRARIES} ${IPE_LIBRARY})
	set(IPE_INCLUDE_DIRS ${IPE_INCLUDE_DIR})
endif()

find_package_handle_standard_args(IPE DEFAULT_MSG IPE_LIBRARY IPE_INCLUDE_DIR)
mark_as_advanced(IPE_LIBRARY IPE_INCLUDE_DIR IPE_FIND_ERROR)

