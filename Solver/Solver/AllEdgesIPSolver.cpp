//
// Created by gharg on 13.02.18.
//

#include <unordered_map>
#include "AllEdgesIPSolver.h"

CapturingSolution AllEdgesIPSolver::solve(Capturing2D *instance, ulong maxK) {
	if(!instance->isConverted())
		instance->convertSegments2Graph();

	IloEnv env;
	IloModel model(env);
	IloCplex cplex(model);

	//convert problem to cplex model
	//first create all necessary variables
	IloNumVarArray xVars(env);
	ulong constraintAmount = 0;
	auto &vertices = instance->getGraph()->getVertices();
	auto *families = instance->getVertexFamilies();
	//std::cout << "create x (vertex) variables" << std::endl;
	for (ulong i = 0; i < vertices.size(); ++i) {
		std::string name = std::string("x" + std::to_string(i));
		if(settings.relaxation) {
            name.append("N");
            IloNumVar var = IloNumVar(env,0,1,name.c_str());
            xVars.add(var);

        } else {
            name.append("B");
            IloBoolVar var = IloBoolVar(env, name.c_str());
            xVars.add(var);
            //std::cout << "variable name: " << name << std::endl;
        }
	}
	//std::cout << "x variables created" << std::endl;
	IloNumVarArray yVars(env);
	std::vector<IloNumVarArray> familiesInCplex;
	std::vector<std::map<std::pair<Vertex,Vertex>,ulong>> vertexEdgeToCplexVarsMap(families->size());
	std::map<ulong,std::pair<Vertex,Vertex>> cplexVarsToVertexEdgeMap;

	//std::cout << "create max expression and y (edge) vars" << std::endl;
	IloExpr maxExpr(env);
	ulong index = 0;
	for (ulong j = 0; j < families->size(); ++j) {
		familiesInCplex.emplace_back(env);
		auto &family = (*families)[j];
		for (ulong k = 0; k < family.size(); ++k) {
			for (ulong l = k+1; l < family.size(); ++l) {
				std::string name = "y_s";
				name.append(std::to_string(j));
				name.append("_(");
				name.append(std::to_string(family[k]));
				name.append(",");
				name.append(std::to_string(family[l]));
				name.append(")");
                if(settings.relaxation || settings.edgeRelaxation) {
                    name.append("N");
                    IloNumVar yVar(env,0,1,name.c_str());
                    yVars.add(yVar);
                    familiesInCplex[j].add(yVar);
                    maxExpr += instance->getWeightInFamilyWithIndex(j,k,l) * yVar;

                } else {
                    name.append("B");
                    IloBoolVar yVar(env,name.c_str());
                    yVars.add(yVar);
                    familiesInCplex[j].add(yVar);
                    maxExpr += instance->getWeightInFamilyWithIndex(j,k,l) * yVar;

                }
				auto vertexPair = std::make_pair(family[k],family[l]);
				auto edgeIndexes = index;
				vertexEdgeToCplexVarsMap[j].insert(std::make_pair(vertexPair,edgeIndexes));
				cplexVarsToVertexEdgeMap.insert(std::make_pair(edgeIndexes,vertexPair));
				index++;
			}
		}
	}
	model.add(IloMaximize(env,maxExpr,"objective function"));
	++constraintAmount;

	//std::cout << "create sum <= 1 expression" << std::endl;
	//forall segments s create sum edges y of s <= 1
	for (ulong m = 0; m < families->size(); ++m) {
		IloExpr expr(env);
		for (int j = 0; j < familiesInCplex[m].getSize(); ++j) {
			expr += familiesInCplex[m][j];
		}
		IloRange range(env,expr ,1);
		model.add(range);
		++constraintAmount;
	}
	//std::cout << "create segments and vertex sum expression" << std::endl;
	//forall segments s and vertex p in s: sum edges y incident to p in s <= x_p
	for (ulong n = 0; n < families->size(); ++n) {
		auto &family = (*families)[n];
		for (ulong j = 0; j < family.size(); ++j) {
			IloExpr expr(env);
			expr += xVars[family[j]];
			for (ulong k = 0; k < family.size(); ++k) {
				if(j==k) {
					continue;
				}
				Vertex v1 = family[j];
				Vertex v2 = family[k];
				if(v1 > v2) {
					std::swap(v1,v2);
				}
				auto innerIndex = vertexEdgeToCplexVarsMap[n][std::make_pair(v1,v2)];
				expr -= yVars[innerIndex];
			}
			IloRange range(env,0,expr);
			model.add(range);
			++constraintAmount;
		}
	}
	//std::cout << "create max k constraint" << std::endl;
	// If a k is omitted take k, else take k of instance

	IloExpr kExpr(env);
	for (int i1 = 0; i1 < xVars.getSize(); ++i1) {
		kExpr += xVars[i1];
	}
	model.add(IloRange(env, kExpr, maxK, "Max K Constraint"));
	++constraintAmount;

	std::cout << "IP formulated" << std::endl;
	if(settings.output && settings.modelOutput) {
		std::cout << cplex.getModel() << std::endl;
	}
	try{

		if (settings.output) {
			std::cout << "Amount nodes: " << xVars.getSize() << " Amount edges: " << yVars.getSize() << std::endl;
			//cplex.setOut(std::cout);
			std::cout << "Amount constraints: " << constraintAmount << std::endl;

		} else {
			cplex.setOut(env.getNullStream());
		}

        if(settings.presolution != nullptr && !settings.presolution->getVertexSolution().empty()) {
            //fill initial solution
            auto& initVertexSol = settings.presolution->getVertexSolution();
            //auto& initEdgeSol = settings.presolution->getEdgeSolution();
            IloNumArray startValY(env);
            double capSolutionVal = 0;
            for (int i = 0; i < xVars.getSize(); ++i) {
                startValY.add(IloBool(initVertexSol.find(i) != initVertexSol.end()));
            }
            std::cout << "Value computed: " << capSolutionVal << std::endl;
            cplex.addMIPStart(xVars,startValY,IloCplex::MIPStartAuto,"YBoolStart");
        }

		cplex.setParam(IloCplex::Param::TimeLimit,settings.timeLimit);
		if (cplex.solve()) {
			IloNumArray vals(env);
			if (settings.output) {
				env.out() << "Solution status = " <<
						  cplex.getStatus() << std::endl;
				env.out() << "Solution value = " <<
						  cplex.getObjValue() << std::endl;

			}
			cplex.getValues(vals, xVars);
			std::set<Vertex> vertexSolution;
			std::vector<Edge> edgeSolution;
			if(settings.output && settings.variableResultOutput) {
				std::cout << "Vertex solution:" << std::endl;
			}
			for (unsigned int i = 0; i < vals.getSize(); ++i) {
				if (vals[i] > 0) {
					if(settings.output && settings.variableResultOutput) {
						std::cout << xVars[i] <<  vals[i] << std::endl;
					}
					vertexSolution.insert((Vertex) i);
				}
			}
			if(settings.output && settings.variableResultOutput) {
				std::cout << "Edge solution:" << std::endl;
			}
			cplex.getValues(vals, yVars);
			for (unsigned int i = 0; i < vals.getSize(); ++i) {
				if (vals[i] > 0) {
					if(settings.output && settings.variableResultOutput) {
						std::cout << yVars[i] << vals[i] << std::endl;
					}
					Edge e = instance->getGraph()->getEdge(cplexVarsToVertexEdgeMap[i]).first;
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
		std::cout << "Error solving instance" << std::endl;
		cplex.clear();
		cplex.end();
		model.end();
		env.end();
	}

	return CapturingSolution(std::set<Vertex>(), instance);
}

const CplexSettings &AllEdgesIPSolver::getSettings() const {
	return settings;
}

void AllEdgesIPSolver::setSettings(const CplexSettings &settings) {
	AllEdgesIPSolver::settings = settings;
}
