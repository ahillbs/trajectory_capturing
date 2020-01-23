add_library(macroTesting MacroTestingInstance.hpp TestRawPoints.hpp "Wrapper Templates/Generator.h" "Wrapper Templates/Solver.h" "Wrapper Templates/Solution.h" TestRawPointsContainer.h InstanceGenerator.h CapturingInstanceContainer.cpp CapturingInstanceContainer.h)
target_link_libraries(macroTesting  imports) #${LIBRARIES}
target_include_directories(macroTesting PRIVATE ${INCLUDE_DIRS})