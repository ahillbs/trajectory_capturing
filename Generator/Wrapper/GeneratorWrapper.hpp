//
// Created by gharg on 19.09.17.
//

#ifndef PROJECT_GENERATORWRAPPER_H
#define PROJECT_GENERATORWRAPPER_H

#include "../../Capturing/Header/Capturing2D.h"
#include "../../MacroTestingFramework/TestRawPoints.hpp"
#include "../Header/Cap2DGenerator.h"

namespace GeneratorCap2D {

	struct GeneratorSettings {
		ulong m = 0;
		ulong xBound = 0;
		ulong yBound = 0;
		ulong seed = 0;
		ulong horAmount = 0;
		ulong vertAmount = 0;
		ulong segIncrease = 0;
		ulong horIncrease = 0;
		ulong vertIncrease = 0;
		bool isRelativeSegmentAmount = false;
	};

	static std::vector<Vector2D> transformTestInstanceToVector2D(const std::vector<std::vector<NT>> &points, ulong amount) {
		std::vector<Vector2D> vecs;
		for (uint i = 0; i < amount; ++i) {
			//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
			Vector2D v(points[0][i], points[1][i]);
			vecs.push_back(v);
		}
		return vecs;
	}

	static TestRawPoints<NT> transformVector2DToTestInstance(const std::vector<Vector2D> &points) {
		std::vector<NT> vecs;
		for (uint i = 0; i < points.size(); ++i) {
			//std::cout << "points: " << points[0][i] << " " << points[1][i] << std::endl;
			vecs.push_back(points[i].x);
			vecs.push_back(points[i].y);
		}
		return TestRawPoints<NT>("randomPoints", &vecs[0], &vecs[vecs.size()], 2);
	}

	class RandomSharedEndPointsInstanceGenerator {
		uint maxK = 5;
		GeneratorSettings settings;
	public:


		const GeneratorSettings &getSettings() const {
			return settings;
		}

		void setSettings(const GeneratorSettings &settings) {
			RandomSharedEndPointsInstanceGenerator::settings = settings;
		}

		const std::string GeneratorName = "RandomShared";
		const std::string GeneratorNameShort = "RandS";

		ulong getSegmentAmount() const {
			return settings.m;
		}

		void setSegmentAmount(ulong segmentAmount) {
			this->settings.m = segmentAmount;
		}

		uint getMaxK() const {
			return maxK;
		}

		void setMaxK(uint maxK) {
			this->maxK = maxK;
		}

		bool isIsRelativeSegmentAmount() const {
			return settings.isRelativeSegmentAmount;
		}

		void setIsRelativeSegmentAmount(bool isRelativeSegmentAmount) {
			this->settings.isRelativeSegmentAmount = isRelativeSegmentAmount;
		}

		Capturing2D
		generateInstance(const std::vector<std::vector<NT>> &points, ulong amount, ulong segments, ulong seedStart,
								 const std::string &instanceName) {
			auto vecs = transformTestInstanceToVector2D(points, amount);
			ulong totalSegmentAmount = calcSegmentAmount(vecs.size(), segments);
			Capturing2D cap2D = Cap2DGenerator::generateRandomSharedEndPointsInstance(vecs, totalSegmentAmount,
                                                                                      seedStart, instanceName);
			//cap2D.convertSegments2Graph();
			return cap2D;
		}

		void generateInstances(std::vector<Capturing2D> *instances, std::vector<TestRawPoints<NT>*> rawPoints, ulong iterations, ulong seed) {

			for (ulong i = 0; i < rawPoints.size(); ++i) {
				std::cout << "Now building: " << rawPoints[i]->name << std::endl;
				for (ulong j = 0; j < iterations; ++j) {
					ulong actualSeed = seed + j * j * 15175;
					Capturing2D p = generateInstance(rawPoints[i]->points, rawPoints[i]->amount,
													 ((ulong) settings.m) + j * settings.segIncrease, actualSeed,
													 rawPoints[i]->name);

					instances->push_back(p);
				}
			}
		}

		ulong calcSegmentAmount(ulong pointAmount, ulong segments) const {
			ulong totalSegmentAmount = segments;
			ulong maxSegments = (pointAmount * (pointAmount - 1)) / 2;
			if (settings.isRelativeSegmentAmount) {
				if (segments >= 100) {
					totalSegmentAmount = maxSegments;
				} else {
					double percent = maxSegments * ((double) segments / (double) 100);
					totalSegmentAmount = (ulong) (percent);
				}
			} else {
				if (segments > maxSegments) {
					totalSegmentAmount = maxSegments;
				}
			}
			return totalSegmentAmount;
		}


		std::string printSettings() {
			std::string result = "";
			if (settings.isRelativeSegmentAmount) {
				result += "segment percentage ";
				result += std::to_string(settings.m);
				result += "%";
			} else {
				result += "segments: ";
				result += std::to_string(settings.m);
			}
			result += ", ";

			result += "starting seed ";
			result += std::to_string(settings.seed);
			result += ", ";

			result += "segment increase per iteration ";
			result += std::to_string(settings.segIncrease);
			result += ", ";

			result += "relative segment amount ";
			result += std::to_string(settings.isRelativeSegmentAmount);

			return result;
		}

	};

	class ProbabilisticInstanceGenerator {
		ulong maxK = 0;
		GeneratorSettings settings;

	public:
		const GeneratorSettings &getSettings() const {
			return settings;
		}

		void setSettings(const GeneratorSettings &settings) {
			ProbabilisticInstanceGenerator::settings = settings;
		}


		const std::string GeneratorName = "ProbabilisticInstance";
		const std::string GeneratorNameShort = "Prob";


		ulong getMaxK() const {
			return maxK;
		}

		void setMaxK(ulong maxK) {
			this->maxK = maxK;
		}

		double getSegmentAmount() const {
			return settings.m;
		}

		void setSegmentAmount(double prob) {
			if(settings.m > 100)
				throw std::invalid_argument("segment amount is over 100%");
			this->settings.m = (ulong)prob;
		}

		double getProbability() const {
			return settings.m;
		}

		void setProbability(double probability) {
			this->setSegmentAmount(probability);
		}

		Capturing2D
		generateInstance(const std::vector<std::vector<NT>> &points, ulong amount, ulong segments, ulong seedStart,
								 const std::string &instanceName) {
			auto vecs = transformTestInstanceToVector2D(points, amount);

			Capturing2D cap2D = Cap2DGenerator::generateProbabilisticInstance(vecs, segments, seedStart, instanceName);
			//cap2D.convertSegments2Graph();
			return cap2D;
		}

		void generateInstances(std::vector<Capturing2D> *instances, std::vector<TestRawPoints<NT>*> rawPoints, ulong iterations, ulong seed) {

			for (ulong i = 0; i < rawPoints.size(); ++i) {
				std::cout << "Now building: " << rawPoints[i]->name << std::endl;
				for (ulong j = 0; j < iterations; ++j) {
					ulong actualSeed = seed + j * j * 15175 + 51548 * i;
					Capturing2D p = generateInstance(rawPoints[i]->points, rawPoints[i]->amount,
													 ((ulong) settings.m) + j * settings.segIncrease, actualSeed,
													 rawPoints[i]->name);
					instances->push_back(p);
				}
			}
		}

		std::string printSettings() {
			std::string result = "";
			result += "probability ";
			result += std::to_string(settings.m);
			result += "%, ";

			result += "starting seed ";
			result += std::to_string(settings.seed);
			result += ", ";

			result += "segment % increase per iteration ";
			result += std::to_string(settings.segIncrease);


			return result;
		}
	};

	class RandomAllNodesInstanceGenerator {

		uint maxK = 5;
		GeneratorSettings settings;
	public:

		const std::string GeneratorName = "RandomAll";
		const std::string GeneratorNameShort = "RandA";

		const GeneratorSettings &getSettings() const {
			return settings;
		}

		void setSettings(const GeneratorSettings &settings) {
			RandomAllNodesInstanceGenerator::settings = settings;
		}


		ulong getSegmentAmount() const {
			return settings.m;
		}

		void setSegmentAmount(ulong segmentAmount) {
			this->settings.m = segmentAmount;
		}

		uint getMaxK() const {
			return maxK;
		}

		void setMaxK(uint maxK) {
			this->maxK = maxK;
		}

		bool isIsRelativeSegmentAmount() const {
			return settings.isRelativeSegmentAmount;
		}

		void setIsRelativeSegmentAmount(bool isRelativeSegmentAmount) {
			this->settings.isRelativeSegmentAmount = isRelativeSegmentAmount;
		}

		Capturing2D
		generateInstance(const std::vector<std::vector<NT>> &points, ulong amount, ulong segments, ulong seedStart,
								 const std::string &name) {
			auto vecs = transformTestInstanceToVector2D(points, amount);
			ulong totalSegmentAmount = calcSegmentAmount(vecs.size(), segments);
			Capturing2D cap2D = Cap2DGenerator::generateRandomAllNodesInstance(vecs, totalSegmentAmount, seedStart,
                                                                               name);
			//cap2D.convertSegments2Graph();
			return cap2D;
		}

		void generateInstances(std::vector<Capturing2D> *instances, std::vector<TestRawPoints<NT>*> rawPoints, ulong iterations, ulong seed) {

			for (ulong i = 0; i < rawPoints.size(); ++i) {
				std::cout << "Now building: " << rawPoints[i]->name << std::endl;
				for (ulong j = 0; j < iterations; ++j) {
					ulong actualSeed = seed + j * j * 15175;
					Capturing2D p = generateInstance(rawPoints[i]->points, rawPoints[i]->amount,
													 ((ulong) settings.m) + j * settings.segIncrease, actualSeed,
													 rawPoints[i]->name);

					instances->push_back(p);
				}
			}
		}

		ulong calcSegmentAmount(ulong pointAmount, ulong segments) const {
			ulong totalSegmentAmount = segments;
			ulong maxSegments = (pointAmount * (pointAmount - 1)) / 2;
			if (settings.isRelativeSegmentAmount) {
				if (segments >= 100) {
					totalSegmentAmount = maxSegments;
				} else {
					double percent = maxSegments * ((double) segments / (double) 100);
					totalSegmentAmount = (ulong) (percent);
				}
			} else {
				if (segments > maxSegments) {
					totalSegmentAmount = maxSegments;
				}
			}
			return totalSegmentAmount;
		}

		std::string printSettings() {
			std::string result = "";
			if (settings.isRelativeSegmentAmount) {
				result += "segment percentage ";
				result += std::to_string(settings.m);
				result += "%";
			} else {
				result += "segments: ";
				result += std::to_string(settings.m);
			}
			result += ", ";
			result += "starting seed ";
			result += std::to_string(settings.seed);
			result += ", ";

			result += "segment increase per iteration ";
			result += std::to_string(settings.segIncrease);
			result += ", ";

			result += "relative segment amount ";
			result += std::to_string(settings.isRelativeSegmentAmount);

			return result;
		}
	};
	class TwoAxisCapturingNoOverlap_Generator {
	public:

	private:
		GeneratorSettings settings;
	public:
		const std::string GeneratorName = "TwoAxisNoOverlapCapturing";
		const std::string GeneratorNameShort = "TANOC";
		const GeneratorSettings &getSettings() const {
			return settings;
		}

		void setSettings(const GeneratorSettings &settings) {
			TwoAxisCapturingNoOverlap_Generator::settings = settings;
		}

		Capturing2D generateInstance() {
			return Cap2DGenerator::generate2DAxisAlignedInstance(settings.m,settings.horAmount,settings.vertAmount,settings.xBound,settings.yBound,settings.seed);
		}

		std::string printSettings() {
			std::string result = "segments: ";
			result += std::to_string(settings.m);
			result += ", ";

			result += "segment increase per iteration ";
			result += std::to_string(settings.segIncrease);
			result += ", ";

			result += "xBound ";
			result += std::to_string(settings.xBound);
			result += ", ";

			result += "yBound ";
			result += std::to_string(settings.yBound);
			result += ", ";

			result += "seed ";
			result += std::to_string(settings.seed);
			result += ", ";

			result += "horizontal lines ";
			result += std::to_string(settings.horAmount);
			result += ", ";

			result += "horizontal increase per iteration ";
			result += std::to_string(settings.horIncrease);
			result += ", ";

			result += "vertical lines ";
			result += std::to_string(settings.vertAmount);
			result += ", ";

			result += "vertical increase per iteration ";
			result += std::to_string(settings.vertIncrease);


			return result;
		}
		void generateInstances(std::vector<Capturing2D> *instanceContainer,ulong amount,ulong seed=0) {
			for (ulong j = 0; j < amount; ++j) {
				ulong actualSeed = seed+j*252;
				Capturing2D p = Cap2DGenerator::generate2DAxisAlignedInstance(settings.m+j*settings.segIncrease,
																			  settings.horAmount+j*settings.horIncrease,
																			  settings.vertAmount+j*settings.vertIncrease,
																			  settings.xBound,settings.yBound,
																			  actualSeed);

				instanceContainer->push_back(p);
			}
		}
	};

	class TwoAxisCapturingOverlap_Generator {

		GeneratorSettings settings;

	public:
		const std::string GeneratorName = "TwoAxisOverlapCapturing";
		const std::string GeneratorNameShort = "TAOC";
		const GeneratorSettings &getSettings() const {
			return settings;
		}

		void setSettings(const GeneratorSettings &settings) {
			this->settings = settings;
		}

		Capturing2D generateInstance() {
			return Cap2DGenerator::generate2DAxisAlignedOverlapInstance(settings.m,settings.horAmount,settings.vertAmount,settings.xBound,settings.yBound,settings.seed);
		}

		std::string printSettings() {
			std::string result = "segments: ";
			result += std::to_string(settings.m);
			result += ", ";


			result += "segment increase per iteration ";
			result += std::to_string(settings.segIncrease);
			result += ", ";

			result += "xBound ";
			result += std::to_string(settings.xBound);
			result += ", ";

			result += "yBound ";
			result += std::to_string(settings.yBound);
			result += ", ";

			result += "seed ";
			result += std::to_string(settings.seed);
			result += ", ";

			result += "horizontal lines ";
			result += std::to_string(settings.horAmount);
			result += ", ";

			result += "horizontal increase per iteration ";
			result += std::to_string(settings.horIncrease);
			result += ", ";

			result += "vertical lines ";
			result += std::to_string(settings.vertAmount);
			result += ", ";

			result += "vertical increase per iteration ";
			result += std::to_string(settings.vertIncrease);

			return result;
		}
		void generateInstances(std::vector<Capturing2D> *instanceContainer,ulong amount, ulong seed) {
			for (ulong j = 0; j < amount; ++j) {
				Capturing2D p = Cap2DGenerator::generate2DAxisAlignedOverlapInstance(settings.m+j*settings.segIncrease,
																						   settings.horAmount+j*settings.horIncrease,
																						   settings.vertAmount+j*settings.vertIncrease,
																						   settings.xBound,settings.yBound,
																						   seed+j*252);
				instanceContainer->emplace_back(p);
			}
		}
	};

}

#endif //PROJECT_GENERATORWRAPPER_H
