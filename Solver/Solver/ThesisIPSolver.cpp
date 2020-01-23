//
// Created by root on 17.02.18.
//

#include "ThesisIPSolver.h"

using EdgeToCplexPair = std::pair<std::pair<Vertex ,Vertex >,int>;
using VertexPair = std::pair<Vertex,Vertex>;
using SegmentEdge = std::pair<ulong,std::pair<Vertex,Vertex>>;
using EdgeToCplex =  std::map<SegmentEdge,int>;
using CplexToEdge = std::map<ulong,VertexPair >;

const CplexSettings &ThesisIPSolver::getSettings() const {
    return settings;
}

void ThesisIPSolver::setSettings(const CplexSettings &settings) {
    ThesisIPSolver::settings = settings;
}

CapturingSolution ThesisIPSolver::solve(Capturing2D *instance, ulong k) {
    instance->convertSegments2Graph();
    //translate graph to cplex
    //generate variables
    IloEnv env;
    IloModel model(env);
    IloCplex cplex(model);

    IloNumVarArray cplexBoolArrayX(env);
    IloNumVarArray cplexBoolArrayY(env);


    for (unsigned int l = 0; l < instance->getGraph()->getVertices().size(); ++l) {
        std::string name = "Vertex" + std::to_string(l);
        if(settings.relaxation) {
            name.append("N");
            cplexBoolArrayY.add(IloNumVar(env,0,1,name.c_str()));
        } else {
            name.append("B");
            cplexBoolArrayY.add(IloBoolVar(env,name.c_str()));
        }
    }
    model.add(cplexBoolArrayX);
    model.add(cplexBoolArrayY);

    //Adding Constraints
    CplexToEdge cplexToEdge;
    uint familyConstraints = 0;
    uint families = 0;
    uint edgesUsed = 0;
    ulong counter = 0;
    for (auto vertexFamily : *instance->getVertexFamilies()) {

        edgesUsed += vertexFamily.size()-1;
        cplexToEdge.insert(std::make_pair(counter,std::make_pair(vertexFamily[0],vertexFamily[1])));

        std::string name = "Edge" + std::to_string(families) +
                           "," +std::to_string(vertexFamily[0]) +
                           "," + std::to_string(vertexFamily[1]);
        if(settings.relaxation) {
            name.append("N");
            cplexBoolArrayX.add(IloNumVar(env,0,1, name.c_str()));
        } else {
            name.append("B");
            cplexBoolArrayX.add(IloBoolVar(env, name.c_str()));
        }

        IloRange  rangeStart(env, 0 ,  cplexBoolArrayY[vertexFamily[0]] - cplexBoolArrayX[counter]);
        model.add(rangeStart);
        ++familyConstraints;
        familyConstraints += (vertexFamily.size()-2) * 2;
        //edgeUsedCounter[counter]++;
        ++counter;
        for (unsigned int i = 1; i < vertexFamily.size()-1; ++i) {


            cplexToEdge.insert(std::make_pair(counter,std::make_pair(vertexFamily[i],vertexFamily[i+1])));
            std::string name1 = "Edge" + std::to_string(families) +
                               "," +std::to_string(vertexFamily[i]) +
                               "," + std::to_string(vertexFamily[i+1]);
            if(settings.relaxation) {
                name1.append("N");
                cplexBoolArrayX.add(IloNumVar(env,0,1, name1.c_str()));
            } else {
                name1.append("B");
                cplexBoolArrayX.add(IloBoolVar(env, name1.c_str()));
            }

            IloRange  range(env,0, cplexBoolArrayY[ vertexFamily[i]] + cplexBoolArrayX[counter] - cplexBoolArrayX[counter-1] );

            IloRange range2(env,0, cplexBoolArrayY[vertexFamily[i]] + cplexBoolArrayX[counter-1] - cplexBoolArrayX[counter] );

            model.add(range);
            model.add(range2);

            //edgeUsedCounter[counter]++;
            ++counter;
        }
        //edgeX1 = edgeToCplexVar[g.getEdge(vertexFamily.second[vertexFamily.second.size()-2],vertexFamily.second[vertexFamily.second.size()-1]).first];
        IloRange  rangeEnd(env, 0 ,cplexBoolArrayY[vertexFamily[vertexFamily.size()-1]] - cplexBoolArrayX[counter-1]);
        model.add(rangeEnd);
        ++familyConstraints;
        ++families;
    }

    // SUM(Vertices) <= maxK Constraint
    IloNumExpr expr2(env);
    for(unsigned int i = 0; i < cplexBoolArrayY.getSize(); ++i) {
        expr2 += cplexBoolArrayY[i];
    }
    ulong maxK = k;
    model.add(expr2 <= (int)maxK);
    expr2.end();



    counter = 0;
    // Adding maximize Subject
    IloNumExpr maxExpr(env);
    for(unsigned int i = 0; i < cplexBoolArrayX.getSize(); ++i) {
        Edge edge = instance->getGraph()->getEdge(cplexToEdge[i]).first;
        maxExpr += edge.third * cplexBoolArrayX[i];
    }
    model.add(IloMaximize(env, maxExpr));
    maxExpr.end();
    //*/
    ///*
    try {
        if (settings.output) {
            std::cout << "Amount nodes: " << cplexBoolArrayY.getSize() << " Amount edges: " << cplexBoolArrayX.getSize() << std::endl;
            //cplex.setOut(std::cout);
            std::cout << "Amount constraints: " << familyConstraints << " amount families: " << families << std::endl;
            std::cout << "Amount edges in families: " << edgesUsed << std::endl;

        } else {
            cplex.setOut(env.getNullStream());
        }
        if(settings.output && settings.modelOutput) {
            std::cout << cplex.getModel() << std::endl;
        }
        //if initial solution is present add to cplex as MIPstart
        if(settings.presolution != nullptr && !settings.presolution->getVertexSolution().empty()) {
            //fill initial solution
            auto& initVertexSol = settings.presolution->getVertexSolution();
            //auto& initEdgeSol = settings.presolution->getEdgeSolution();
            IloNumArray startValY(env);
            double capSolutionVal = 0;
            for (int i = 0; i < cplexBoolArrayY.getSize(); ++i) {
                startValY.add(IloBool(initVertexSol.find(i) != initVertexSol.end()));
            }
            std::cout << "Value computed: " << capSolutionVal << std::endl;
            cplex.addMIPStart(cplexBoolArrayY,startValY,IloCplex::MIPStartAuto,"YBoolStart");
        }
        //cplex.setParam(IloCplex::Param::Emphasis::Numerical,true);
        cplex.setParam(IloCplex::Param::TimeLimit,settings.timeLimit);
        if (cplex.solve()) {
            IloNumArray vals(env);
            if (settings.output) {
                env.out() << "Solution status = " <<
                          cplex.getStatus() << std::endl;
                env.out() << "Solution value = " <<
                          cplex.getObjValue() << std::endl;

            }
            cplex.getValues(vals, cplexBoolArrayY);
            std::set<Vertex> vertexSolution;
            std::vector<Edge> edgeSolution;
            if(settings.output && settings.variableResultOutput) {
                std::cout << "Vertex solution:" << std::endl;
            }
            for(unsigned int i = 0; i < vals.getSize() ; ++i) {
                if (vals[i] > 0) {
                    if(settings.output && settings.variableResultOutput) {
                        std::cout << cplexBoolArrayY[i] <<  vals[i] << std::endl;
                    }
                    vertexSolution.insert((Vertex) i);
                }
            }
            if(settings.output && settings.variableResultOutput) {
                std::cout << "Edge solution:" << std::endl;
            }
            cplex.getValues(vals, cplexBoolArrayX);
            for(unsigned int i = 0; i < vals.getSize() ; ++i) {
                if (vals[i] > 0) {
                    if(settings.output && settings.variableResultOutput) {
                        std::cout << cplexBoolArrayX[i] << vals[i] << std::endl;
                    }
                    Edge e = instance->getGraph()->getEdge(cplexToEdge[i]).first;
                    edgeSolution.push_back(e);
                }
            }
            double objVal = cplex.getObjValue();
            double upperBound = cplex.getBestObjValue();

            cplex.clear();
            cplex.end();
            model.end();
            env.end();
            CapturingSolution sol(vertexSolution, edgeSolution, objVal, instance);
            sol.setUpperValue(upperBound);
            return sol;

        }
    } catch (IloException &iloException) {
        iloException.print(std::cout);
    }
    return CapturingSolution(instance);
}
