//
// Created by gharg on 29.03.18.
//

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "CapturingInstanceContainer.h"
#include "../Import/Imports.h"
#include <limits>
#include <unordered_map>


TestingFramework::CapturingInstanceContainer::CapturingInstanceContainer() = default;

TestingFramework::CapturingInstanceContainer::CapturingInstanceContainer(const std::string &instancesPath) {
    addInstances(instancesPath, false);
}

CapturingMetaData TestingFramework::CapturingInstanceContainer::readMetaData(std::string path) {
	//Capturing2D instance = Imports::importSVG(path);
	//std::ifstream s(path.c_str());
	return Imports::importCapturingSVGMetaData(path);
}

void TestingFramework::CapturingInstanceContainer::addInstances(const std::string &instancesPath, bool recursive) {
	boost::filesystem::path filePath(instancesPath);

	//try to make the paths the same for relative paths with "./"
	filePath = boost::filesystem::absolute(filePath);
	

	if (boost::filesystem::exists(filePath)) {
		std::cout << "Search directory: "<< filePath << std::endl;
		if (boost::filesystem::is_directory(filePath)) {

			for (boost::filesystem::directory_entry &it : boost::filesystem::directory_iterator(filePath)) {
				std::string pathString = it.path().c_str();
			    if(boost::filesystem::is_regular(it) && pathString.find(".svg") != std::string::npos)
			        this->metaData.insert(readMetaData(it.path().string()));
				    //this->metaData.emplace_back(readMetaData(it.path().string()));
			    else if(boost::filesystem::is_directory(it) && recursive) {
			        this->addInstances(it.path().string(),recursive);
			    }
			}
		} else if (boost::filesystem::is_regular_file(filePath)) {
			std::string pathString = filePath.c_str();
			if(pathString.find(".svg")  != std::string::npos) {
				this->metaData.insert(readMetaData(filePath.string()));
				//this->metaData.emplace_back(readMetaData(filePath.string()));
			}
		}
	}
}

template<typename Type>
inline bool isInBetween(Type value,Type rangeStart,Type rangeEnd) {
	return (value >= rangeStart) && (value <= rangeEnd);
}

std::vector<CapturingMetaData>
TestingFramework::CapturingInstanceContainer::getMetaInstances(ulong maxInstanceAmount, ulong segStart, ulong segEnd,
															   ulong vertStart, ulong vertEnd, ulong sharedMin,
															   ulong sharedMax, const std::string &nameFilter,
															   ulong skipInstances) {
	std::vector<CapturingMetaData> resultDataPtrs;
	ulong maxLong = std::numeric_limits<ulong>::max();
	if(segEnd == 0)
		segEnd = maxLong;
	if(vertEnd == 0)
		vertEnd = maxLong;
	if(sharedMax == 0)
		sharedMax = maxLong;
	if(maxInstanceAmount == 0)
		maxInstanceAmount = maxLong;

	ulong instanceAmount = 0;
	for (const CapturingMetaData &metaInstance : metaData) {
		bool passed = true;

		if(!isInBetween(metaInstance.segmentAmount,segStart,segEnd))
			passed = false;

		if(!isInBetween(metaInstance.vAmount,vertStart,vertEnd))
			passed = false;

		if(!isInBetween(metaInstance.sharedEndpoints,sharedMin,sharedMax))
			passed = false;

		if(!nameFilter.empty()) {
			if(metaInstance.name.find(nameFilter) == std::string::npos) {
				passed = false;
			}
		}

		if(passed) {
			if(skipInstances > 0) {
				--skipInstances;
				continue;
			}
			resultDataPtrs.push_back(metaInstance);
			++instanceAmount;
			if(instanceAmount >= maxInstanceAmount)
				break;
		}
	}
	return resultDataPtrs;
}

Capturing2D TestingFramework::CapturingInstanceContainer::getInstance(const CapturingMetaData instanceMetaData) {
	Capturing2D capturing2D = Imports::importSVG(instanceMetaData.pathToInstance);
	//capturing2D.setMetaData(*instanceMetaData);
	return capturing2D;
	
}

void TestingFramework::CapturingInstanceContainer::addInstance(const CapturingMetaData &metaData) {
    //Add instance if not in set. Else just change entry
	auto it = this->metaData.find(metaData);
	if(it != this->metaData.end()) {
		this->metaData.erase(it);
	}
	this->metaData.insert(metaData);
}

ulong TestingFramework::CapturingInstanceContainer::getContainerSize() {
    return this->metaData.size();
}

void
TestingFramework::CapturingInstanceContainer::append(std::string &instanceName, ulong segAmount, ulong vertexAmount,
													 std::string &path, std::string &gen, ulong sharedEndP) {
	metaData.insert(CapturingMetaData(instanceName,segAmount,vertexAmount,path,gen,sharedEndP));
}


