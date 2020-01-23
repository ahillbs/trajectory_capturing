//
// Created by gharg on 29.03.18.
//

#ifndef PROJECT_CAPTURINGMETADATA_H
#define PROJECT_CAPTURINGMETADATA_H


#include <zconf.h>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "../../Import/XmlWrapper.h"

struct CapturingMetaData {

	std::string name = "";
	ulong segmentAmount = 0;
	ulong vAmount = 0;
	std::string pathToInstance = "";
	std::string generator = "";
	ulong sharedEndpoints = 0;
	explicit CapturingMetaData() {};
	CapturingMetaData(const std::string &instanceName, ulong segAmount,ulong vertexAmount,const std::string &path) : name(instanceName),segmentAmount(segAmount),vAmount(vertexAmount),pathToInstance(path) {};
	CapturingMetaData(const std::string &instanceName, ulong segAmount,ulong vertexAmount,const std::string &path, const std::string &gen, ulong sharedEndP)
			: name(instanceName),segmentAmount(segAmount),vAmount(vertexAmount),pathToInstance(path),generator(gen),sharedEndpoints(sharedEndP) {};

    Utils::XmlWrapper::Node toXMLNode() {
        using Utils::XmlWrapper;
		XmlWrapper::Node node("CapturingMetaData");
		node.subNodes.emplace_back("Name",name);
        node.subNodes.emplace_back("SegAmount",std::to_string(segmentAmount));
        node.subNodes.emplace_back("VAmount",std::to_string(vAmount));
        //node.subNodes.emplace_back("PathToInstance",pathToInstance);
        node.subNodes.emplace_back("Generator",generator);
        node.subNodes.emplace_back("SharedEndPoint",std::to_string(sharedEndpoints));
        return node;
	}

	static CapturingMetaData fromXML(boost::property_tree::ptree val) {
			CapturingMetaData metaData;
			metaData.name = val.get<std::string>("Name", "");
			metaData.segmentAmount = val.get<ulong>("SegAmount", 0);
			metaData.vAmount = val.get<ulong>("VAmount", 0);
			metaData.generator = val.get<std::string>("Generator", "");
			metaData.sharedEndpoints = val.get<ulong>("SharedEndPoint", 0);
			return metaData;
    }

    bool operator> (const CapturingMetaData &metaData) const {
        return this->name > metaData.name;
    }

	bool operator< (const CapturingMetaData &metaData) const {
		return this->name < metaData.name;
	}
};

/*static bool operator> (const CapturingMetaData &metaData,const CapturingMetaData &metaData2) {
	return metaData.name > metaData2.name;
}

static bool operator< (const CapturingMetaData &metaData,const CapturingMetaData &metaData2) {
	return metaData.name < metaData2.name;
}*/

#endif //PROJECT_CAPTURINGMETADATA_H
