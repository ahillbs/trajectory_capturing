include(FindPackageHandleStandardArgs)

set(CGAL_FIND_ERROR FALSE)

find_path(
        CGAL_INCLUDE_DIR
        NAMES
            CGAL/Arrangement_2.h
        HINTS
            ${CGAL_ROOT}/include
            $ENV{ProgramFiles}/CGAL/*/include
            $ENV{SystemDrive}/CGAL/*/include
        PATH_SUFFIXES
            CGAL
            cgal
            Cgal
)

if(NOT CGAL_INCLUDE_DIR)
    set(CGAL_FIND_ERROR TRUE)
else()
	if(NOT CGAL_FIND_QUIETLY)
		message(STATUS "CGAL include directory: ${CGAL_INCLUDE_DIR}")
	endif()
endif()

find_library(
        CGAL_LIBRARY
        NAMES
            CGAL
        HINTS
            ${CGAL_ROOT}/lib
            $ENV{ProgramFiles}/CGAL/*/lib
            $ENV{SystemDrive}/CGAL/*/lib
        PATH_SUFFIXES
            CGAL
            cgal
            Cgal
)

if(NOT CGAL_LIBRARY)
    set(CGAL_FIND_ERROR TRUE)
else()
	if(NOT CGAL_FIND_QUIETLY)
		message(STATUS "CGAL library: ${CGAL_LIBRARY}")
	endif()
endif()

find_library(
        CGAL_CORE_LIBRARY
        NAMES
            CGAL_Core
        HINTS
            ${CGAL_ROOT}/lib
            $ENV{ProgramFiles}/CGAL/*/lib
            $ENV{SystemDrive}/CGAL/*/lib
        PATH_SUFFICES
            CGAL
            cgal
            Cgal
)

if(NOT CGAL_CORE_LIBRARY)
    set(CGAL_FIND_ERROR TRUE)
else()
	if(NOT CGAL_FIND_QUIETLY)
		message(STATUS "CGAL_Core library: ${CGAL_CORE_LIBRARY}")
	endif()
endif()

find_package(Boost COMPONENTS system thread)
if(NOT Boost_FOUND)
    set(CGAL_FIND_ERROR TRUE)
else()
	if(NOT CGAL_FIND_QUIETLY)
		message(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
		message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
	endif()
endif()

find_library(
        CGAL_GMP_LIBRARY
        NAMES
            gmp
        HINTS
            ${GMP_ROOT}/lib
            ${GMP_ROOT}
        PATH_SUFFIXES
            GMP
            gmp
            Gmp
)

find_path(
		CGAL_GMP_INCLUDE_DIR
		NAMES
			gmp.h
			gmpxx.h
		HINTS
			${GMP_ROOT}/include
			${GMP_ROOT}
		PATH_SUFFIXES
			GMP
			gmp
			Gmp
)

find_library(
    CGAL_MPFR_LIBRARY
    NAMES
        mpfr
    HINTS
        ${MPFR_ROOT}/lib
        ${MPFR_ROOT}
        ${GMP_ROOT}/lib
        ${GMP_ROOT}
    PATH_SUFFIXES
        MPFR
        mpfr
        Mpfr
)

find_path(
	CGAL_MPFR_INCLUDE_DIR
	NAMES
		mpfr.h
	HINTS
		${MPFR_ROOT}/include
		${MPFR_ROOT}
		${GMP_ROOT}/include
		${GMP_ROOT}
	PATH_SUFFIXES
		MPFR
		mpfr
		Mpfr
)

if(NOT CGAL_GMP_LIBRARY OR NOT CGAL_MPFR_LIBRARY)
    set(CGAL_FIND_ERROR TRUE)
else()
	if(NOT CGAL_FIND_QUIETLY)
		message(STATUS "GMP include directory: ${CGAL_GMP_INCLUDE_DIR}")
		message(STATUS "GMP library: ${CGAL_GMP_LIBRARY}")
		message(STATUS "MPFR include directory: ${CGAL_MPFR_INCLUDE_DIR}")
		message(STATUS "MPFR library: ${CGAL_MPFR_LIBRARY}")
	endif()
endif()

if(NOT CGAL_FIND_ERROR)
    if(CGAL_FIND_COMPONENTS AND "ImageIO" IN_LIST CGAL_FIND_COMPONENTS)
        find_library(
                CGAL_IMAGEIO_LIBRARY
                NAMES
                    CGAL_ImageIO
					CGAL_ImageIo
					CGAL_imageio
                HINTS
                    ${CGAL_ROOT}/lib
                    $ENV{ProgramFiles}/CGAL/*/lib
                    $ENV{SystemDrive}/CGAL/*/lib
                PATH_SUFFIXES
                    CGAL
                    cgal
                    Cgal
        )

		if(NOT CGAL_FIND_QUIETLY)
			message(STATUS "CGAL_ImageIO library: ${CGAL_IMAGEIO_LIBRARY}")
		endif()

		if(CGAL_IMAGEIO_LIBRARY)
			set(CGAL_IMAGEIO_FOUND TRUE)
		endif()
	endif()

    if(CGAL_FIND_COMPONENTS AND "qt" IN_LIST CGAL_FIND_COMPONENTS)
        find_library(
                CGAL_QT_LIBRARY
                NAMES
                    CGAL_Qt
                    CGAL_Qt4
                    CGAL_Qt5
                HINTS
                    ${CGAL_ROOT}/lib
                    $ENV{ProgramFiles}/CGAL/*/lib
                    $ENV{SystemDrive}/CGAL/*/lib
                PATH_SUFFIXES
                    CGAL
                    cgal
                    Cgal
        )

		if(NOT CGAL_FIND_QUIETLY)
			message(STATUS "CGAL_Qt library: ${CGAL_QT_LIBRARY}")
		endif()

		if(CGAL_QT_LIBRARY)
			set(CGAL_QT_FOUND TRUE)
		endif()
	endif()
endif()

set(CGAL_INCLUDE_DIRS ${CGAL_INCLUDE_DIR} ${CGAL_GMP_INCLUDE_DIR} ${CGAL_MPFR_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
set(CGAL_LIBRARIES ${CGAL_LIBRARY} ${CGAL_CORE_LIBRARY} ${CGAL_GMP_LIBRARY} ${CGAL_MPFR_LIBRARY})

if(CGAL_QT_FOUND)
	list(APPEND CGAL_LIBRARIES ${CGAL_QT_LIBRARY})
endif()

if(CGAL_IMAGEIO_FOUND)
	list(APPEND CGAL_LIBRARIES ${CGAL_IMAGEIO_LIBRARY})
endif()

list(APPEND CGAL_LIBRARIES ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})

if(UNIX)
	list(APPEND CGAL_LIBRARIES ${CGAL_M_LIBRARY})
	list(APPEND CGAL_LIBRARIES ${CGAL_PTHREAD_LIBRARY})
endif()

if(CGAL_FIND_ERROR)
	set(CGAL_LIBRARY FALSE)
endif()

find_package_handle_standard_args(CGAL DEFAULT_MSG CGAL_LIBRARY CGAL_CORE_LIBRARY CGAL_INCLUDE_DIR CGAL_GMP_LIBRARY CGAL_MPFR_LIBRARY Boost_INCLUDE_DIRS Boost_LIBRARIES)

mark_as_advanced(CGAL_INCLUDE_DIR CGAL_LIBRARY CGAL_CORE_LIBRARY CGAL_FIND_ERROR CGAL_QT_LIBRARY CGAL_IMAGEIO_LIBRARY CGAL_GMP_LIBRARY CGAL_MPFR_LIBRARY)
