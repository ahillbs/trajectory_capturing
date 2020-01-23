//
// Created by gharg on 26.09.17.
//

#ifndef PROJECT_SIMULATEDANNEALING_H
#define PROJECT_SIMULATEDANNEALING_H

#include <iostream>
#include "../../Capturing/Header/Capturing2D.h"

#ifndef BITSIZE
#define BITSIZE 64
#endif
namespace Metaheuristic {
	struct SASettings {
		double (*temperatureFunc)(double, ulong) = nullptr;
		double (*probabiltyFunc)(double,double,double) = nullptr;
		double startT = 50;
		ulong maxIteration = 400000;
		ulong maxTime = 900;
		ulong maxNoIncrements = maxIteration / 5;
		ulong maxNoIncrementsAndSwaps = maxNoIncrements / 6;
		uint maxThreads = uint() - 1;
		ulong seed = 3458723426394;
		bool useLocal = true;
		Neighborhood* neighborhood;
	};
	void computeSolution(CapturingSolution *sol, ulong innerSeed, SASettings settings);
	static double temperatureSlowDown05Reheat50Function(double T, ulong i) {
		i = (i == 0) ? 1 : i;
		double I;
		I = T *0.05;
		if (i % 50 != 0)
			return (T - I);
		return (11.5*T);
	}
	static double temperatureSlowDown05NoReheatFunction(double T,ulong i) {
		i = (i == 0) ? 1 : i;
		double I;
		I = T *0.05;
		return (T - I);
	}
	static double bolzmannFunction(double currSolVal, double innerSolVal, double temperature) {
		double percentDifference = (1 - innerSolVal/currSolVal) * 100;
		return std::exp(-((percentDifference)/temperature));
	}
	class SimulatedAnnealing {
	private:
		SASettings settings;
	public:
		bool isUseLocal() const;
		void setUseLocal(bool useLocal);
		ulong getSeed() const;
		void setSeed(ulong seed);
		ulong getMaxTime() const;
		void setMaxTime(ulong maxTime);
		void setTemperatureFunc(double temp(double,ulong)) { settings.temperatureFunc = temp; }
		void setProbabilityFunc(double temp(double,double, double)) { settings.probabiltyFunc = temp; }
		CapturingSolution solve(Capturing2D *problem, ulong maxK);
		CapturingSolution solve(Capturing2D *problem, ulong maxK, const CapturingSolution *paramSol);
		ulong getMaxIteration() const;
		void setMaxIteration(ulong maxIteration);
		double getStartT() const;
		void setStartT(double startT);
		ulong getMaxNoIncrementsAndSwaps() const;
		void setMaxNoIncrementsAndSwaps(ulong maxNoIncrementsAndSwaps);
		ulong getMaxNoIncrements() const;
		void setMaxNoIncrements(ulong maxNoIncrements);
		const SASettings &getSettings() const;
		void setSettings(const SASettings &settings);
	};
}

#endif //PROJECT_SIMULATEDANNEALING_H
