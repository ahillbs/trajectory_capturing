#cmake_minimum_required(VERSION 3.5)
cmake_minimum_required(VERSION 3.14)
project(BA)
project(CapturingTestSuite)
project(CapturingGenerator)
#set(gtest_build_samples 1)
#add_subdirectory(googletest)

#set(BUILD_SHARED_LIBS OFF)
#SET(BUILD_SHARED_LIBRARIES OFF)
#set(ITK_DYNAMIC_LOADING OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#set(CPLEX_FIND_QUIETLY OFF)
#set(CGAL_FIND_QUIETLY OFF)
set(Boost_USE_STATIC_LIBS   ON)
set(UTIL_BOOST_LIBRARIES system filesystem program_options thread serialization)
if(NOT ALG_CMAKE_DIR)
    if(NOT "$ENV{ALG_CMAKE_DIR}" STREQUAL "")
        set(ALG_CMAKE_DIR "$ENV{ALG_CMAKE_DIR}")
    else()
        set(ALG_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    endif()
endif()


include(${ALG_CMAKE_DIR}/util.cmake)
#set(CPLEX_ROOT /opt/ibm/ILOG/CPLEX_Studio1271/)


set(Boost_USE_STATIC_LIBS   ON)
find_package(CGAL REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options thread REQUIRED)

find_package(CPLEX REQUIRED)
#include_directories(${CPLEX_INCLUDE_DIRS})

set(SOURCE_FILES ${SOURCE_FILES} )

set(ALGCPLEX_INCLUDE_DIR ${UTIL_ALGCPLEX_ROOT}/include/algcplex)
set(LIBRARIES ${LIBRARIES} ${CPLEX_LIBRARIES} ${CGAL_LIBRARIES} ${Boost_LIBRARIES} m pthread dl)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CPLEX_INCLUDE_DIRS} ${ALGCPLEX_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

set(INCLUDE_FILES_FOR_CLION ${INCLUDE_FILES_FOR_CLION}
        Capturing/Header/Capturing2D.h Capturing/Header/CapturingGraph.h
        Capturing/Header/Segment.h Capturing/Header/CapturingSolution.h Capturing/Neighborhoods/OuterSharedSegmentsVertices.h
        Generator/Header/Cap2DGenerator.h
        Utilities/GeoUtil.h Utilities/Graph/undirectedMatrixGraph.h Utilities/Graph/undirectedAdjGraph.h Utilities/Graph/undirectedGraph.h
        Import/Imports.h Import/XmlWrapper.h
        Visualization/SegmentVisualization.h Visualization/simple_svg_1.0.0.hpp
        Solver/Metaheuristics/SimulatedAnnealing.h Solver/Metaheuristics/EvolutionComputation.h Solver/Solver/GreedySolver.h
        Solver/Solver/ThesisIPSolver.h Solver/Solver/AllEdgesIPSolver.h)

set(SOURCE_FILES ${SOURCE_FILES} ${INCLUDE_FILES_FOR_CLION}  )



###### SUB MODULES ##############
add_subdirectory(Visualization)
add_subdirectory(Utilities)
add_subdirectory(Capturing)
add_subdirectory(Import)
add_subdirectory(Solver)
add_subdirectory(Generator)
add_subdirectory(MacroTestingFramework)
#add_library(cplex_util )
#add_library(capturing Capturing/Segment.cpp Capturing/Capturing2D.cpp Capturing/CapturingSolution.cpp Capturing/CapturingGraph.cpp)
#target_link_libraries(capturing ${LIBRARIES})
#target_include_directories(capturing PRIVATE ${INCLUDE_DIRS})

set(MY_LIBS capturing imports utilities visualization macroTesting solver generator)
#################################


#add_executable(BA main.cpp ${SOURCE_FILES})
#util_setup_target(BA)

# add other libraries (CPLEX)
#target_include_directories(BA PRIVATE ${INCLUDE_DIRS})
#target_link_libraries(BA ${LIBRARIES} ${MY_LIBS})

message(${LIBRARIES})
#add_executable(Visualization Visualization/main_1.0.0.cpp Visualization/simple_svg_1.0.0.hpp)
#util_setup_target(Visualization)

add_executable(CapturingTestSuite capturingMain.cpp ${SOURCE_FILES})
util_setup_target(CapturingTestSuite)
target_include_directories(CapturingTestSuite PRIVATE ${INCLUDE_DIRS})
target_link_libraries(CapturingTestSuite ${LIBRARIES} ${MY_LIBS} )

add_executable(CapturingGenerator generatorMain.cpp ${SOURCE_FILES})
util_setup_target(CapturingGenerator)
target_include_directories(CapturingGenerator PRIVATE ${INCLUDE_DIRS})
target_link_libraries(CapturingGenerator ${LIBRARIES} ${MY_LIBS} )

# add other libraries (CPLEX)
#target_include_directories(Visualization PRIVATE ${INCLUDE_DIRS})
#target_link_libraries(Visualization ${LIBRARIES})

set(TEST_SOURCE_FILES Tests/GraphTests.cpp Tests/MacroTestFrameWorkTests.cpp Solver/Metaheuristics/TabuSearch.cpp Solver/Metaheuristics/TabuSearch.h Tests/CGALTests.cpp Capturing/Neighborhoods/Neighborhood.h)
add_executable(testGoogle ${TEST_SOURCE_FILES} ${SOURCE_FILES})
#include_directories(./googletest )
#link_directories(./googletest)
target_include_directories(testGoogle PRIVATE gtest gtest_main ${INCLUDE_DIRS})
target_link_libraries(testGoogle gtest gtest_main ${LIBRARIES} ${MY_LIBS} )



add_library(CapturingLib SHARED ${SOURCE_FILES} capturingMain.cpp pythonInterface.cpp)
target_include_directories(CapturingLib PRIVATE ${INCLUDE_DIRS})
target_link_libraries(CapturingLib ${LIBRARIES} -Wl,--start-group ${MY_LIBS} -Wl,--end-group )