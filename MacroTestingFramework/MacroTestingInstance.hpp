//
// Created by gharg on 18.09.17.
//

#define LOG

#ifndef PROJECT_MACROTESTING_H
#define PROJECT_MACROTESTING_H


#include <iostream>
#include <fstream>
#include <chrono>
#include "string"
#include "boost/filesystem.hpp"
#include <boost/filesystem/fstream.hpp>
#include <thread>
#include "TestRawPoints.hpp"
#include "../Capturing/Header/Capturing2D.h"
#include "Wrapper Templates/Solver.h"
#include "CapturingInstanceContainer.h"

namespace MacroTesting {

struct Report {
	Report(ulong initialColumns) : header(initialColumns) {}
	std::string name;
	std::string addInfo;
	std::vector<std::string> header;
	std::vector<std::vector<std::string>> data;
    std::vector<std::string> solution;

	std::string to_string() {
		std::string filestring = "#";
		filestring += name;
		filestring += "\t";
		filestring += addInfo;
		filestring += "\n";
		filestring += "#";
		for (uint i = 0; i < header.size(); ++i) {
			filestring += header[i];
			filestring += "\t";
		}
		filestring += "\n";

		for (uint j = 0; j < data.size(); ++j) {
			for (uint i = 0; i < data[j].size(); ++i) {
				filestring += data[j][i];
				filestring += "\t";
			}
			filestring += "\n";
		}
		return filestring;
	}
};


	template<typename Problem, typename Generator, typename Solver, typename Solution>
	class MacroTestingInstance {
	private:
		Generator *generator;
		Solver *solver;
		Solution *solution;
		std::vector<std::string> instanceNames;
		std::vector<Problem> problems;
	public:

		MacroTestingInstance(Generator *generator = nullptr, Solver *solver = nullptr) {
			this->generator = generator;
			this->solver = solver;
		}

		Generator *getGenerator() const {
			return generator;
		}

		void setGenerator(Generator *generator) {
			this->generator = generator;
		}

		Solver *getSolver() const {
			return solver;
		}


		void setSolver(Solver *solver) {
			this->solver = solver;
		}

		const std::vector<Problem> &getProblems() const {
			return problems;
		}

		const std::vector<std::string> &getInstanceNames() const {
			return instanceNames;
		}

		void clearInstances() {
			problems.clear();
			instanceNames.clear();
		}
		Report
		runAvgTests(std::string testName, TestRawPoints<NT> **instances, ulong instanceSize, ulong iterations,
					ulong seedStart = 0) {
			Report report(3);
			report.name = testName;
			report.header[0] = "Instance";
			report.header[1] = "avg time";
			report.header[2] = "avg solution value";
			Generator reserveGenerator;
			Solver reserveSolver;
			Generator* genptr = generator;
			Solver *solvptr = solver;
			if (genptr == nullptr) {
				genptr = &reserveGenerator;
			}
			if (solvptr == nullptr) {
				solvptr = &reserveSolver;
			}
			report.addInfo = genptr->printSettings();
#ifdef LOG
			std::cout << "Settings: " << genptr->printSettings() << std::endl;
#endif
			for (uint i = 0; i < instanceSize; ++i) {
				ulong avg_time = 0;
				double avg_solValue = 0;
#ifdef LOG
				std::cout << "Start tests for instance " << instances[i]->name << std::endl;
#endif

				for (uint j = 0; j < iterations; ++j) {
#ifdef LOG
					std::cout << "Iteration: " << j << std::endl;
					std::cout << "Generate problem" << std::endl;
#endif
					Problem p = genptr->generateInstance(instances[i]->points, instances[i]->amount,genptr->getSegmentAmount(),
														 seedStart + j * j,instances[i]->name);

#ifdef LOG
					std::cout << "solving..." << std::endl;
#endif
					auto begin_time = std::chrono::high_resolution_clock::now();
					Solution s = solvptr->solve(&p,p.getK());

					auto end_time = std::chrono::high_resolution_clock::now();
#ifdef LOG
					std::cout << "solving complete. Write data in Report" << std::endl;
#endif
					double solValue = s.getSolutionValue();
					avg_solValue += solValue;
					auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
							end_time - begin_time).count();
					avg_time += nanoseconds;
				}
				avg_solValue = avg_solValue / iterations;
				double averageTime = avg_time / iterations;
				std::vector<std::string> dataColumn = {instances[i]->name, std::to_string(averageTime),
											 std::to_string(avg_solValue)};
				report.data.push_back(dataColumn);
			}
			return report;
		}


		void bulkBuildProblemInstances(TestRawPoints<NT>** instances, ulong instanceSize, ulong iterations,
									   ulong seedStart = 0,ulong segmentIncrease = 0) {
			problems.clear();
			instanceNames.clear();
			auto startAmount = generator->getSegmentAmount();
			for (ulong i = 0; i < instanceSize; ++i) {
				instanceNames.push_back(instances[i]->name);
				generator->setSegmentAmount(startAmount);
				std::cout << "Now building: " << instances[i]->name << std::endl;
				for (ulong j = 0; j < iterations; ++j) {
					Problem p = generator->generateInstance(instances[i]->points, instances[i]->amount, startAmount+j*segmentIncrease,
															seedStart + j * j,instances[i]->name);
					problems.push_back(p);
				}
			}
		}


		Report
		runAvgTests(std::string testName, const std::vector<std::string> &instanceNames,
					const std::vector<Problem> &instances, ulong instanceSize, ulong iterations) {
			Report report(3);
			report.name = testName;
			report.header[0] = "Instance";
			report.header[1] = "avg time";
			report.header[2] = "avg solution value";
			Generator reserveGenerator;
			Solver reserveSolver;

			Generator* genptr = generator;
			Solver *solvptr = solver;
			if (genptr == nullptr) {
				genptr = &reserveGenerator;
			}
			if (solvptr == nullptr) {
				solvptr = &reserveSolver;
			}
			report.addInfo = genptr->printSettings();
#ifdef LOG
			std::cout << "Settings: " << genptr->printSettings() << std::endl;
#endif
			for (uint i = 0; i < instanceSize; ++i) {
				ulong avg_time = 0;
				double avg_solValue = 0;
#ifdef LOG
				std::cout << "Start tests for instance " << instanceNames[i] << std::endl;
#endif
				for (uint j = 0; j < iterations; ++j) {
#ifdef LOG
					std::cout << "Iteration: " << j << std::endl;
#endif

					Problem p = instances[i*iterations + j];
#ifdef LOG
					std::cout << "solving..." << std::endl;
#endif
					auto begin_time = std::chrono::high_resolution_clock::now();
					Solution s = solvptr->solve(&p, p.getK());

					auto end_time = std::chrono::high_resolution_clock::now();
					double solValue = s.getSolutionValue();
					avg_solValue += solValue;
#ifdef LOG
					std::cout << "Solving complete. Value is: " << s.getSolutionValue() << ". Write data in Report" << std::endl;
#endif
					auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
							end_time - begin_time).count();
					avg_time += nanoseconds;
				}
				avg_solValue = avg_solValue / iterations;
				double averageTime = avg_time / iterations;
				std::vector<std::string> dataColumn = {instanceNames[i], std::to_string(averageTime),
											 std::to_string(avg_solValue)};
				report.data.push_back(dataColumn);
			}

			if (generator == &reserveGenerator) {
				generator = nullptr;
			}
			if (solver == &reserveSolver) {
				solver = nullptr;
			}
			return report;
		}

		Report
		runTests(std::string testName, const std::vector<std::string> &instanceNames,
					const std::vector<Problem> &instances, ulong instanceSize, ulong iterations,ulong k,ulong kIncrease = 5,ulong maxK = 25) {
			std::cout << "Start test for " << testName << std::endl;
			Report report(9);
			report.name = testName;
			report.header[0] = "Instance";
			report.header[1] = "Iteration";
			report.header[2] = "Segments";
			report.header[3] = "Edges";
			report.header[4] = "Vertices";
			report.header[5] = "Max k";
			report.header[6] = "Time";
			report.header[7] = "Solution value";
			report.header[8] = "Upper bound value";
			Generator reserveGenerator;
			Solver reserveSolver;

			Solver *solvptr = solver;
			if (solvptr == nullptr) {
				solvptr = &reserveSolver;
			}
			for (uint i = 0; i < instanceSize; ++i) {
#ifdef LOG
				std::cout << "Start tests for instance " << instanceNames[i] << " " << i << std::endl;
#endif
				for (uint j = 0; j < iterations; ++j) {
#ifdef LOG
					Problem p = instances[i*iterations + j];
					for (ulong internalK = k; internalK <= maxK; internalK += kIncrease) {
						std::cout << "Iteration: " << j << " k: " << internalK << std::endl;
#endif

						//p.setK(internalK);

#ifdef LOG
						std::cout << "solving..." << std::endl;
#endif
						auto begin_time = std::chrono::high_resolution_clock::now();
						Solution s = solvptr->solve(&p,internalK);

						auto end_time = std::chrono::high_resolution_clock::now();
						double solValue = s.getSolutionValue();
#ifdef LOG
						std::cout << "Solving complete. Value is: " << s.getSolutionValue() << ". Write data in Report"
								  << std::endl;
#endif
						auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
								end_time - begin_time).count();
						report.data.push_back(
								{instanceNames[i], std::to_string(j), std::to_string(p.getVertexFamilies()->size()),
								 std::to_string(p.getGraph()->getEdges().size()),
								 std::to_string(p.getGraph()->getSize()),
								 std::to_string(internalK), std::to_string(nanoseconds), std::to_string(solValue),
								 std::to_string(s.getUpperValue())});
					}
				}
			}

			if (generator == &reserveGenerator) {
				generator = nullptr;
			}
			if (solver == &reserveSolver) {
				solver = nullptr;
			}
			return report;
		}

		Report
		runTests(std::string testName, TestRawPoints<NT> **instances, ulong instanceSize,
				 ulong iterations,ulong k,ulong kIncrease = 5,ulong maxK = 25,ulong segmentIncrease = 5, ulong seedStart = 0) {
			Report report(9);
			report.name = testName;
			report.header[0] = "Instance";
			report.header[1] = "Iteration";
			report.header[2] = "Segments";
			report.header[3] = "Edges";
			report.header[4] = "Vertices";
			report.header[5] = "Max k";
			report.header[6] = "Time";
			report.header[7] = "Solution value";
			report.header[8] = "Upper bound value";
			Generator reserveGenerator;
			Solver reserveSolver;

			Generator* genptr = generator;
			Solver *solvptr = solver;
			if (solvptr == nullptr) {
				solvptr = &reserveSolver;
			}
			if (genptr == nullptr) {
				genptr = &reserveGenerator;
			}
			for (uint i = 0; i < instanceSize; ++i) {
#ifdef LOG
				std::cout << "Start tests for instance " << instances[i]->name << std::endl;
#endif
				auto segAmount = genptr->getSegmentAmount();
				for (uint j = 0; j < iterations; ++j) {

					Problem p = genptr->generateInstance(instances[i]->points, instances[i]->amount,segAmount + (j * segmentIncrease),
														 seedStart + j * j);
					for (ulong internalK = k; internalK <= maxK; internalK += kIncrease) {
#ifdef LOG
						std::cout << "Iteration: " << j << " k: " << internalK << std::endl;
#endif
						//p.setK(internalK);

#ifdef LOG
						std::cout << "Solving..." << std::endl;
#endif
						auto begin_time = std::chrono::high_resolution_clock::now();
						Solution s = solvptr->solve(&p,internalK);
						auto end_time = std::chrono::high_resolution_clock::now();
						double solValue = s.getSolutionValue();
#ifdef LOG
						std::cout << "Solving complete. Value is: " << s.getSolutionValue() << ". Write data in Report"
								  << std::endl;
#endif
						auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
								end_time - begin_time).count();
						report.data.push_back(
								{instances[i]->name, std::to_string(j), std::to_string(p.getVertexFamilies()->size()),
								 std::to_string(p.getGraph()->getEdges().size()),
								 std::to_string(p.getGraph()->getSize()),
								 std::to_string(p.getK()), std::to_string(nanoseconds), std::to_string(solValue),
								 std::to_string(s.getUpperValue())});
					}

				}
			}


			if (generator == &reserveGenerator) {
				generator = nullptr;
			}
			if (solver == &reserveSolver) {
				solver = nullptr;
			}
			return report;
		}



	};




	/// Runs the test instances and writes signature data into a report.
	/// \tparam Solver Solver class to be used to solve the problem
	/// \tparam time std::chrono time, in which data will be shown (e.g. nanoseconds or seconds etc.)
	/// \param testName Name of the report/test
	/// \param instances Test instances to be solved
	/// \param solver Solver to be used to solve the instances
	/// \param k Starting k for the Problem
	/// \param kIncrease Increase of k per Iteration
	/// \param maxK Maximum K to be solved
	/// \return Finished report
	template<typename Solver,typename time = std::chrono::nanoseconds>
	MacroTesting::Report runTests(const std::string &testName,std::vector<CapturingMetaData> &instances,Solver &solver,ulong k = 5,ulong kIncrease = 5,ulong maxK = 25) {

		MacroTesting::Report report(8);
		report.name = testName;
		report.header[0] = "Instance";
		report.header[1] = "Segments";
		report.header[2] = "Edges";
		report.header[3] = "Vertices";
		report.header[4] = "Max k";
		report.header[5] = "Time";
		report.header[6] = "Solution value";
		report.header[7] = "Upper bound value";

		for (CapturingMetaData &instance : instances) {
			Capturing2D p = TestingFramework::CapturingInstanceContainer::getInstance(instance);
			p.convertSegments2Graph();
			for (ulong internalK = k; internalK <= maxK; internalK += kIncrease) {
#ifdef LOG
				std::cout << "Start tests for instance " << instance.name << std::endl;
#endif

				auto begin_time = std::chrono::high_resolution_clock::now();
				CapturingSolution s = solver.solve(&p,internalK);
				auto end_time = std::chrono::high_resolution_clock::now();
				double solValue = s.getSolutionValue();
#ifdef LOG
				std::cout << "Solving complete. Value is: " << s.getSolutionValue() << ". Write data in Report"
						  << std::endl;
#endif
				auto nanoseconds = std::chrono::duration_cast<time>(
						end_time - begin_time).count();
				report.data.push_back(
						{instance.name, std::to_string(p.getVertexFamilies()->size()),
						 std::to_string(p.getGraph()->getEdges().size()),
						 std::to_string(p.getGraph()->getSize()),
						 std::to_string(internalK), std::to_string(nanoseconds), std::to_string(solValue),
						 std::to_string(s.getUpperValue())});
				report.solution.push_back(p.to_string(s));
			}
		}
		return report;
	}




	static bool saveReport(const std::string &path, MacroTesting::Report report) {
		boost::filesystem::path filePath(path + "/" + report.name + ".csv");
		uint i = 0;
		while (i < 100) {
			try {
				if (boost::filesystem::exists(filePath)) {
					boost::filesystem::file_status fstatus = boost::filesystem::status(filePath);
					if (!boost::filesystem::permissions_present(fstatus)) {
						return false;
					}
				}
				boost::filesystem::ofstream f(filePath);
				f << report.to_string();
				f.close();
				return true;
			} catch (std::exception &e) {
				std::cout << "Error saving file. Try again. Try number: " << i << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
			++i;
		}
		return false;
	}

    static bool saveSolution(const std::string &path, MacroTesting::Report report) {

        //report.header[0] = "Instance";
        //report.header[1] = "Iteration";
        //report.header[5] = "Max k";
	    uint instanceIndex = 0;
	    uint kIndex = 4;
        for (uint k = 0; k < report.header.size(); ++k) {
            if(report.header[k] == "Intance") {
                instanceIndex = k;
            }
            if(report.header[k] == "Max k") {
                kIndex = k;
            }
        }

	    for(ulong j = 0; report.solution.size() > j; ++j) {
            std::string name = report.data[j][instanceIndex];
            if(name[0] == '/') {
                name.erase(0,1);
            }
            std::replace(name.begin(),name.end(),'/','-');
            std::string::size_type pos = name.find(".svg");

            if (pos != std::string::npos)
                name.erase(pos, 4);

            boost::filesystem::path filePath(path + "/" + name + "_K" + report.data[j][kIndex] + "_sol.svg");
            uint i = 0;
            bool saved = false;
            while (i < 100 && !saved) {
                try {
                    if (boost::filesystem::exists(filePath)) {
                        boost::filesystem::file_status fstatus = boost::filesystem::status(filePath);
                        if (!boost::filesystem::permissions_present(fstatus)) {
                            return false;
                        }
                    }

                    std::cout << "saving solution for " << boost::filesystem::absolute(filePath) << std::endl;
                    boost::filesystem::ofstream f(filePath);
                    f << report.solution[j];
                    f.close();
                    saved = true;
                    continue;

                } catch (std::exception &e) {
                    std::cout << "Error saving file. Try again. Try number: " << i << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10000));
                ++i;
            }
            if(i >= 100)
                return false;
        }
        return true;
	}
}
#endif //PROJECT_MACROTESTING_H
