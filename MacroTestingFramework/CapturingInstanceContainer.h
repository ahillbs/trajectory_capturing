//
// Created by gharg on 29.03.18.
//

#ifndef PROJECT_CAPTURINGINSTANCECONTAINER_H
#define PROJECT_CAPTURINGINSTANCECONTAINER_H


#include <vector>
#include "../Capturing/Header/CapturingMetaData.h"
#include "../Capturing/Header/Capturing2D.h"

namespace TestingFramework {
	class CapturingInstanceContainer {
		std::set<CapturingMetaData> metaData;
	public:
		CapturingInstanceContainer();

		explicit CapturingInstanceContainer(const std::string &instancesPath);



		void addInstances(const std::string &instancesPath, bool recursive);
		void addInstance(const CapturingMetaData &metaData);
		std::vector<CapturingMetaData>
        getMetaInstances(ulong maxInstanceAmount, ulong segStart, ulong segEnd, ulong vertStart, ulong vertEnd,
                         ulong sharedMin,
                         ulong sharedMax, const std::string &nameFilter, ulong skipInstances);

		static Capturing2D getInstance(const CapturingMetaData instanceMetaData);


		void append(std::string &instanceName, ulong segAmount,ulong vertexAmount,std::string &path, std::string &gen, ulong sharedEndP);

		ulong getContainerSize();
	private:


		CapturingMetaData readMetaData(std::string path);


	};
}

#endif //PROJECT_CAPTURINGINSTANCECONTAINER_H
