add_library(visualization SegmentVisualization.cpp)
#target_link_libraries(visualization ${LIBRARIES})
target_include_directories(visualization PRIVATE ${INCLUDE_DIRS})