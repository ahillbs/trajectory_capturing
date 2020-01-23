add_library(solver Wrapper/SolverWrapper.hpp Metaheuristics/SimulatedAnnealing.cpp  Metaheuristics/EvolutionComputation.cpp Solver/AllEdgesIPSolver.cpp Solver/ThesisIPSolver.cpp Solver/CplexSettings.h Solver/GreedySolver.cpp)
target_link_libraries(solver ${LIBRARIES})
target_include_directories(solver PRIVATE ${INCLUDE_DIRS})