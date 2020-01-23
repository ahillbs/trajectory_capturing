add_library(generator Cap2DGenerator.cpp   Wrapper/GeneratorWrapper.hpp)
#target_link_libraries(generator ${LIBRARIES})
target_include_directories(generator PRIVATE ${INCLUDE_DIRS})