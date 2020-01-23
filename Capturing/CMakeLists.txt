add_library(capturing Segment.cpp Capturing2D.cpp CapturingSolution.cpp CapturingGraph.cpp Neighborhoods/OuterSharedSegmentsVertices.cpp Neighborhoods/Neighborhood.h Header/CapturingMetaData.h)
#target_link_libraries(capturing ${LIBRARIES})
target_include_directories(capturing PRIVATE ${INCLUDE_DIRS})