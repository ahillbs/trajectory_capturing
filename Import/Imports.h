//
// Created by gharg on 30.08.17.
//

#ifndef PROJECT_SVGIMPORT_H
#define PROJECT_SVGIMPORT_H

#include <vector>
#include "../Utilities/GeoUtil.h"
#include "../Capturing/Header/Capturing2D.h"
//#include "../Capturing/CapturingMetaData.h"
//#include "../MacroTestingFramework/CapturingInstanceContainer.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace TestingFramework {
	class CapturingInstanceContainer;
}
struct Polyline {
	std::vector<Vector2D> points;
};

struct Line {
	Vector2D vec1;
	Vector2D vec2;
};

class Imports {

//	read_xml(is, pt);
public:
	static Capturing2D importCapturingSVG(std::istream &istream);
	static CapturingMetaData importCapturingSVGMetaData(const std::string &path);
	static std::vector<Vector2D> importCSV(std::ifstream &ifstream, const char *lineseparator,
										   const char *columnseparator);

	static Capturing2D importSVG(std::string path);

	template<typename type=uint64_t >
	static std::string hashfile(const char *filename)
	{
		std::ifstream fp(filename);
		std::stringstream ss;

		// Unable to hash file, return an empty hash.
		if (!fp.is_open()) {
			return "";
		}

		// Hashing
		type magic = 5381;
		char c;
		while (fp.get(c)) {
			magic = ((magic << 5) + magic) + c; // magic * 33 + c
		}
		std::cout << "sizof type: " << sizeof(type) << std::endl;
		ss << std::hex << std::setw(sizeof(type)) << std::setfill('0') << magic;
		return ss.str();
	}



};


#endif //PROJECT_SVGIMPORT_H
