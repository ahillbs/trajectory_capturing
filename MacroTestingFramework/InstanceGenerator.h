//
// Created by gharg on 28.03.18.
//

#ifndef PROJECT_INSTANCEGENERATOR_H
#define PROJECT_INSTANCEGENERATOR_H

#include <zconf.h>
#include <chrono>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <future>
#include "../Capturing/Header/Capturing2D.h"
#include "TestRawPoints.hpp"
#include "CapturingInstanceContainer.h"
#include "../Utilities/PathNormifier.h"

namespace TestingFramework {
	template<typename Generator>
	class InstanceGenerator {

		Generator *generator = nullptr;

		void saveInstances(const std::string &path, const std::vector<Capturing2D *> &instances, const std::string &namePrefix) {
			boost::filesystem::path filePath = PathNormifier::norm_Path(path,generator->GeneratorName);


			//Create directory, if it does not exist before
			if(!boost::filesystem::exists(filePath)) {
				boost::filesystem::create_directories(filePath);
			}

			for (auto& instance : instances) {
				//Gather some information for instance name
				if(!namePrefix.empty()) {
					auto metaData = instance->getMetaData();
					metaData.name = namePrefix + "_" + metaData.name;
					instance->setMetaData(metaData);
				}
				std::string name = instance->getMetaData().name;
				if(instance->getMetaData().name == "NoName" || instance->getMetaData().name.empty()) {
					std::cout << "had no name" << std::endl;
					ulong seed = generator->getSettings().seed;
					ulong segmentAmount = instance->getVertexFamilies()->size();
					name = generator->GeneratorNameShort;
					name += "_S";
					std::stringstream sstream;
					sstream << std::hex << seed;
					name += sstream.str();
					name += "_F";
					name += std::to_string(segmentAmount);


				}


				//add an 0 seed to distinguish between various machines, that created instances
                std::mt19937_64 random(0);
				std::uniform_int_distribution<uint> distribution;
				std::stringstream zeroSeed;
				zeroSeed << std::hex << distribution(random);
                std::string zeroSeedStr = "_M" + zeroSeed.str();

				auto metaData = instance->getMetaData();
				metaData.name = name + zeroSeedStr;
				instance->setMetaData(metaData);

				std::string fullPath;
				fullPath += filePath.generic_string();
                fullPath += name;
                fullPath += zeroSeedStr;
				instance->printGraph(fullPath);
				//std::cout << filePath.generic_string() + name + ".svg" << std::endl;
			}
		}
	public:


		explicit InstanceGenerator(Generator *gen) {
			this->generator = gen;
		}

		void GenerateInstances(const std::string &savePath, ulong uniqueInstanceAmount, ulong maxIterations,
									   const std::string &namePrefix) {
			if(generator == nullptr) {
				throw std::invalid_argument("No pointer to a valid generator");
			}

			std::vector<Capturing2D> instances;
			auto seed = generator->getSettings().seed;
			/*
			for (uint i = 0; i < uniqueInstanceAmount; ++i) {

				generator->generateInstances(&instances,maxIterations,seed + i*8645);

			}*/

            for (uint i = 0; i < uniqueInstanceAmount; ++i) {

                generator->generateInstances(&instances,maxIterations,seed + i*8645);

            }

            unsigned threadsSupported = std::thread::hardware_concurrency();

            if(threadsSupported > 1) {

                //distribute instances to every core
                std::vector<std::vector<Capturing2D*>> threadCapturings(threadsSupported);
                for (uint k = 0; k < instances.size(); ++k) {
                    threadCapturings[k % threadsSupported].emplace_back(&instances[k]);
                }

                {
                    std::vector<std::future<void>> threadInstances;
                    for (uint j = 0; j < threadsSupported; ++j) {
                        threadInstances.push_back(
                                std::async(std::launch::async, &InstanceGenerator::saveInstances,this, savePath, std::ref(threadCapturings[j]),namePrefix));
                    }
                }

            } else {
                std::vector<Capturing2D*> instancesPtr;
                for(auto& instance : instances) {
                    instancesPtr.emplace_back(&instance);
                }
				saveInstances(savePath, instancesPtr, namePrefix);
            }
		}

		void GenerateInstances(const std::string &savePath, std::vector<TestRawPoints<NT> *> &rawPointsInstances,
									   ulong maxIterations, const std::string &namePrefix) {
			if(generator == nullptr) {
				throw std::invalid_argument("No pointer to a valid generator");
			}

			std::vector<Capturing2D> instances;
			auto seed = generator->getSettings().seed;
			/*
			for (uint iteration = 0; iteration < maxIterations; ++iteration) {

				generator->generateInstances(&instances,rawPointsInstances,maxIterations,seed + iteration*8645);
			}*/

            generator->generateInstances(&instances,rawPointsInstances,maxIterations,seed );

            unsigned threadsSupported = std::thread::hardware_concurrency();

            if(threadsSupported > 1) {

                //distribute instances to every core
                std::vector<std::vector<Capturing2D*>> threadCapturings(threadsSupported);
                for (uint k = 0; k < instances.size(); ++k) {
                    threadCapturings[k % threadsSupported].emplace_back(&instances[k]);
                }

                {
                    std::vector<std::future<void>> threadInstances;
                    for (uint j = 0; j < threadsSupported; ++j) {
                        threadInstances.push_back(
                                std::async(std::launch::async, &InstanceGenerator::saveInstances,this, savePath, std::ref(threadCapturings[j]),namePrefix));
                    }
                }

            } else {
                std::vector<Capturing2D*> instancesPtr;
                for(auto& instance : instances) {
                    instancesPtr.emplace_back(&instance);
                }
				saveInstances(savePath, instancesPtr, namePrefix);
            }


		}

	};

}


#endif //PROJECT_INSTANCEGENERATOR_H
