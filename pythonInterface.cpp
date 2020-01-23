//
// Created by gharg on 12.06.19.
//

#include "Solver/Solver/AllEdgesIPSolver.h"
#include "Solver/Solver/ThesisIPSolver.h"
#include "Generator/Header/Cap2DGenerator.h"

extern "C"
{
    struct CapSolution {
        double solutionValue;
        double durationInSeconds;
    };
    struct VectorWrapper {
        double x;
        double y;
    };

    struct SegWrapper {
        VectorWrapper start;
        VectorWrapper end;
    };

    struct SegmentsWrapper {
        SegWrapper* segs;
        ulong size;
    };

    ThesisIPSolver* getThesisSolver() {
        return new ThesisIPSolver();
    }
    AllEdgesIPSolver* getAllEdgesSolver() {
        return new AllEdgesIPSolver();
    }
    void deleteThesisSolver(ThesisIPSolver* solver) {
        delete solver;
    }
    void deleteAllEdgesSolver(AllEdgesIPSolver* solver) {
        delete solver;
    }

    Capturing2D* createCapturingInstance(SegmentsWrapper segmentsWrapper,uint endPoints) {
        CapturingMetaData meta("custom",0,0,"","Imported",endPoints);

        //convert points to vector
        std::vector<Segment<NT>> segVec;
        uint maxX = 0,maxY = 0;
        for (int j = 0; j < segmentsWrapper.size; j++) {
            SegWrapper segWrapper = segmentsWrapper.segs[j];
            Vector2D start(segWrapper.start.x,segWrapper.start.y);
            Vector2D end(segWrapper.end.x,segWrapper.end.y);
            maxX = std::max(std::max(maxX,(uint)CGAL::to_double(start.x)),(uint)CGAL::to_double(end.x));
            maxY = std::max(std::max(maxY,(uint)CGAL::to_double(start.y)),(uint)CGAL::to_double(end.y));
            segVec.emplace_back(Segment<NT>(start,end));
        }
        Capturing2D* capturing2D = new Capturing2D(segVec,maxX,maxY,meta);
        capturing2D->convertSegments2Graph();
        return capturing2D;
    }
    void deleteCapturingInstance(Capturing2D* instance) {
        delete instance;
    }

    CapSolution thesisSolveInstance(ThesisIPSolver* solver,Capturing2D* instance, ulong k) {
        auto begin_time = std::chrono::high_resolution_clock::now();
        CapturingSolution solution = solver->solve(instance,k);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - begin_time).count();
        double duration = nanoseconds * std::pow(10,-9);
        return CapSolution{solution.getSolutionValue(),duration};
    }

    CapSolution allEdgeSolveInstance(AllEdgesIPSolver* solver,Capturing2D* instance, ulong k) {
        auto begin_time = std::chrono::high_resolution_clock::now();
        CapturingSolution solution = solver->solve(instance,k);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - begin_time).count();
        double duration = nanoseconds * std::pow(10,-9);
        return CapSolution{solution.getSolutionValue(),duration};
    }

    Capturing2D* generateProbInstance(double points[],int size, double probability, ulong seed, char* name) {
        std::vector<Vector2D> segVec;
        uint maxX = 0,maxY = 0;
        for (int j = 0; j < size; j+=2) {
            Vector2D start(points[j],points[j+1]);
            maxX = std::max(maxX,(uint)CGAL::to_double(start.x));
            maxY = std::max(maxY,(uint)CGAL::to_double(start.y));
            segVec.emplace_back(start);
        }

        return Cap2DGenerator::generateProbabilisticInstanceRef(segVec, probability, seed,
                                                                name);
    }

    SegmentsWrapper getSegments(Capturing2D* instance) {
        auto size = instance->getSegments().size();
        SegWrapper* segPtr;
        segPtr = (SegWrapper *)malloc(sizeof(SegWrapper) * size);
        for (int j = 0; j < size; ++j) {
            const Segment<NT>& segment = instance->getSegments()[j];
            segPtr[j].start.x = CGAL::to_double(segment.GetStartVector().x);
            segPtr[j].start.y = CGAL::to_double(segment.GetStartVector().y);
            segPtr[j].end.x = CGAL::to_double(segment.GetEndVector().x);
            segPtr[j].end.y = CGAL::to_double(segment.GetEndVector().y);
        }
        SegmentsWrapper sw{segPtr,size};
        return sw;
    }

    void deleteSegments(SegmentsWrapper segmentsWrapper) {
        free(segmentsWrapper.segs);
    }

    char* getInstanceSVG(Capturing2D* instance) {
        return strdup(instance->to_string().c_str());
    }

    void freeStr(char* str) {
        free(str);
    }
}