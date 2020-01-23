//
// Created by gharg on 30.08.17.
//

#include "Imports.h"
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdint>
#include <iomanip>
#include "../MacroTestingFramework/CapturingInstanceContainer.h"

Capturing2D Imports::importCapturingSVG(std::istream &istream) {
	using boost::property_tree::ptree;
	ptree tree;
	try {
		read_xml(istream, tree);
	}catch(const boost::property_tree::ptree_error &e)
	{
		std::cout << e.what() << std::endl;
	}

	std::vector<Segment<NT>> segments;
    std::vector<std::vector<Vector2D>> families;
	double maxX =  std::numeric_limits<double>::min();
	double maxY = std::numeric_limits<double>::min();
	CapturingMetaData metaData;
	metaData.generator = "Imported";
	BOOST_FOREACH(auto const& v, tree.get_child("svg") ) {
					ptree::value_type val = ((ptree::value_type)v);
					if(val.first == "desc") {
                        ptree val1 = val.second.get_child("CapturingMetaData");
						const boost::property_tree::ptree & subtree = v.second; // value (or a subnode)
						if(subtree.empty()) {
							// This is a key:value
							//  use subtree.data() as string value or subtree.get_value<T>()
							std::cout << "got empty subtree" << std::endl;
						} else {
							// This is a subtree
							//  use subtree as child
							metaData = CapturingMetaData::fromXML(val1);
						}
					}
					if(val.first == "line") {
						double x1,x2,y1,y2;
						x1 = val.second.get<double>("<xmlattr>.x1",0);
						x2 = val.second.get<double>("<xmlattr>.x2",0);
						y1 = val.second.get<double>("<xmlattr>.y1",0);
						y2 = val.second.get<double>("<xmlattr>.y2",0);
						Segment<NT> seg(x1,y1,x2,y2);
						segments.push_back(seg);
						if(std::max(x1,x2) > maxX)
							maxX = std::max(x1,x2);

						if(std::max(y1,y2) > maxY)
							maxY = std::max(y1,y2);
					}
					if(val.first == "polyline") {
						std::string pointsStr = val.second.get<std::string>("<xmlattr>.points","");
						if(!pointsStr.empty()) {
							families.emplace_back(std::vector<Vector2D>());
							std::string delimiter = " ";
							std::string substring = pointsStr;
							while(!substring.empty()) {
								auto delimiterPoint = substring.find(delimiter);
								std::string pointStr = substring.substr(0,delimiterPoint);
								char* pEnd;
								auto x = strtod(&pointStr[0],&pEnd);
								auto y = strtod(pEnd+1,nullptr);
								Vector2D vec(x,y);
								families[families.size()-1].emplace_back(vec);
								if(delimiterPoint != std::string::npos) {
									++delimiterPoint;
								}
								substring.erase(0,delimiterPoint);


								if(vec.x > maxX)
									maxX = CGAL::to_double(vec.x);

								if(vec.y > maxY)
									maxY = CGAL::to_double(vec.y);
							}
						}

					}
				}
	std::cout << families.size() << std::endl;
	if(!segments.empty()) {
        return Capturing2D(segments, (uint) (maxX * 1.05), (uint) (maxY * 1.05), metaData);
    } else if(!families.empty()) {
	    return Capturing2D(families, (uint) (maxX * 1.05), (uint) (maxY * 1.05), metaData);
	} else {
	    throw std::length_error("No lines or polylines found in SVG");
	}
}

Capturing2D Imports::importSVG(std::string path) {	
	boost::filesystem::path filePath(path);
	std::ifstream stream(path);
	std::string name = filePath.filename().generic_string();
	name = name.substr(0,name.size()-4);	
	
	return Imports::importCapturingSVG(stream);

}

std::vector<Vector2D> Imports::importCSV(std::ifstream &ifstream, const char *lineseparator,
										 const char *columnseparator) {

	std::vector<Vector2D> points;
	if (ifstream) {
		// get length of file:
		ifstream.seekg (0, ifstream.end);
		unsigned long length;
		length = (unsigned long) ifstream.tellg();
		ifstream.seekg (0, ifstream.beg);

		char * buffer = new char [length];

		std::cout << "Reading " << length << " characters... " << std::endl;
		// read data as a block:
		ifstream.read (buffer,length);

		if (ifstream)
			std::cout << "all characters read successfully." << std::endl;
		else
			std::cout << "error: only " << ifstream.gcount() << " could be read" << std::endl;
		ifstream.close();

		// ...buffer contains the entire file...

		std::string csvString(buffer,length);
		// split string into substrings of lines
		unsigned long pos = 0;
		unsigned long endPos = csvString.find(lineseparator,pos);
		if (endPos == std::string::npos) {
			endPos = csvString.size()-1;
		}
		while(pos < csvString.size()) {
			// ignore comment lines
			if(csvString[pos] != '#') {
				unsigned long innerPosMid = csvString.find(columnseparator,pos);
				std::string subStrX = csvString.substr(pos,innerPosMid);
				std::string subStrY = csvString.substr(innerPosMid+1,endPos);
				double x = std::stod(subStrX);
				double y = std::stod(subStrY);
				points.push_back(Vector2D(x,y));
			}


			pos = endPos + 1;
			endPos = csvString.find(lineseparator,pos);
			if (endPos == std::string::npos) {
				endPos = csvString.size()-1;
			}
		}
		delete[] buffer;
	} else {
		std::cout << "file could not be read" << std::endl;
	}
	if(points.size() == 0) {
		std::cout << "no points added!" << std::endl;
	}

	return points;
}



CapturingMetaData Imports::importCapturingSVGMetaData(const std::string &path) {
	std::ifstream ifstream(path);
	using boost::property_tree::ptree;
	ptree tree;
	try {
		read_xml(ifstream, tree);
	} catch (const boost::property_tree::ptree_error &e) {
		std::cout << e.what() << std::endl;
	}

	std::vector<Segment<NT>> segments;
	CapturingMetaData metaData;
	metaData.generator = "Imported";
	ptree svgTree = tree.get_child("svg");
	try{
		ptree descTree = svgTree.get_child("desc");
		ptree val1 = descTree.get_child("CapturingMetaData");
		metaData = CapturingMetaData::fromXML(val1);
	} catch (boost::property_tree::ptree_error e) {
		std::cout << "Couldn't read metadata description. Name will be path" << std::endl;
		metaData.name = path;
	}
	metaData.pathToInstance = path;
	return metaData;
}


/*
Sked read( std::istream & is )
{
    // populate tree structure pt
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(is, pt);

    // traverse pt
    Sked ans;
    BOOST_FOREACH( ptree::value_type const& v, pt.get_child("sked") ) {
        if( v.first == "flight" ) {
            Flight f;
            f.carrier = v.second.get<std::string>("carrier");
            f.number = v.second.get<unsigned>("number");
            f.date = v.second.get<Date>("date");
            f.cancelled = v.second.get("<xmlattr>.cancelled", false);
            ans.push_back(f);
        }
    }

    return ans;
}
 */