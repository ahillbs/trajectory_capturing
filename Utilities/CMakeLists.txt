add_library(utilities Graph/undirectedMatrixGraph.cpp Graph/undirectedAdjGraph.cpp Graph/undirectedGraph.cpp PathNormifier.h)
#target_link_libraries(utilities ${LIBRARIES})
target_include_directories(utilities PRIVATE ${INCLUDE_DIRS})