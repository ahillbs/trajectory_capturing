//
// Created by gharg on 23.06.17.
//

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "SegmentVisualization.h"

SegmentVisualization::SegmentVisualization(const char *name, int xBound, int yBound) : doc(name,svg::Dimensions(xBound+5,yBound+5)),docName(name), xBound(xBound), yBound(yBound) {

}

void SegmentVisualization::setTakenEdges(std::set<std::pair<Vector2D, Vector2D>> &edges) {
    this->edges = edges;
}

void SegmentVisualization::setSegments(std::vector< std::vector<Vector2D>> &segmentToVectorMap) {
    this->segmentToVectorMap = segmentToVectorMap;
}


void SegmentVisualization::setTakenVertices(std::vector<Vector2D> &vertices) {
    this->takenVertices = vertices;
}


void SegmentVisualization::drawSvg() {
    //LineChart chart(Dimensions(5,5));
    Point offset(0,0);
    for(auto& segVecs : segmentToVectorMap) {
        for(auto& vec : segVecs) {
            if(vec.x == 0) {
                offset.x = 5;
            }
            if(vec.y == 0) {
                offset.y = 5;
            }
        }
    }

    double circleDiameter = 3;
    std::vector<Polyline> polyLines;
    std::set<Circle> circles;

    doc << Desc(desc);

    for( std::vector<Vector2D>& secVecs : segmentToVectorMap) {

        if(!drawPolyLines) {
            Point startPoint(CGAL::to_double(secVecs[0].x), CGAL::to_double(secVecs[0].y));
            Point endPoint(CGAL::to_double(secVecs[secVecs.size() - 1].x),
                           CGAL::to_double(secVecs[secVecs.size() - 1].y));
            Line singleLine(startPoint, endPoint, Stroke(1, Color::Blue));

            singleLine.offset(offset);
            doc << singleLine;
        }
        Polyline polyline(Stroke(1, Color::Blue));

		for(Vector2D& vector : secVecs) {
            if(drawPolyLines) {
                polyline << Point(CGAL::to_double(vector.x), CGAL::to_double(vector.y));

            }
			Circle c(Point(CGAL::to_double(vector.x),CGAL::to_double(vector.y)),circleDiameter,Color::Black);
			c.offset(offset);
			if(circles.find(c) == circles.end())
				circles.insert(c);
		}
		if(drawPolyLines) {
            polyline.offset(offset);
            doc << polyline;
		}
    }
    if(!edges.empty() || !takenVertices.empty()) {
        for (std::pair<Vector2D, Vector2D> vecPair : edges) {
            Polyline line(Stroke(1, Color::Red));

            line << Point(CGAL::to_double(vecPair.first.x), CGAL::to_double(vecPair.first.y)) <<
                 Point(CGAL::to_double(vecPair.second.x), CGAL::to_double(vecPair.second.y));
            line.offset(offset);
            polyLines.push_back(line);
            doc << line;
        }

        for (unsigned int i = 0; i < takenVertices.size(); ++i) {
            Circle c(Point(CGAL::to_double(takenVertices[i].x), CGAL::to_double(takenVertices[i].y)), circleDiameter,
                     Color::Black);
            c.offset(offset);
            auto iterator = circles.find(c);
            if (iterator != circles.end()) {
                circles.erase(c);
            }
        }
        for (const Circle& c : circles) {
            doc << c;
        }
        circles.clear();
        for (unsigned int i = 0; i < takenVertices.size(); ++i) {
            Circle c2(Point(CGAL::to_double(takenVertices[i].x), CGAL::to_double(takenVertices[i].y)), circleDiameter,
                      Color::Yellow);
            c2.offset(offset);
            circles.insert(c2);
        }
        for (const Circle& c : circles) {
            doc << c;
        }
    }
}

bool SegmentVisualization::saveDocument() {
    namespace fs = boost::filesystem;
    fs::path filePath(docName);
    fs::path full_path = fs::system_complete(filePath.remove_filename().string());
    if(filePath != "." && !boost::filesystem::exists(filePath.remove_filename())) {
        boost::filesystem::create_directory(full_path);
    }
    bool saved = doc.save();
    if(!saved) {
        std::cerr << "Error printing instance" << std::endl;
    }
    return saved;
}

void SegmentVisualization::setDesc(const std::string &desc) {
    SegmentVisualization::desc = desc;
}

std::string SegmentVisualization::to_string() {
    return doc.toString();
}

void SegmentVisualization::setDrawPolyLines(bool drawPolyLines) {
    SegmentVisualization::drawPolyLines = drawPolyLines;
}
