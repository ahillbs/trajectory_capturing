//
// Created by gharg on 23.06.17.
//

#ifndef BA_SEGMENTVISUALIZATION_H
#define BA_SEGMENTVISUALIZATION_H


#include <utility>
#include <vector>
#include <map>
#include <set>
#include "../Capturing/Header/Segment.h"
#include "simple_svg_1.0.0.hpp"

using namespace svg;

class SegmentVisualization {
private:
    std::vector< std::vector<Vector2D>> segmentToVectorMap;
    std::set<std::pair<Vector2D,Vector2D>> edges;
    std::vector<Vector2D> takenVertices;
    std::string desc;
public:
    void setDesc(const std::string &desc);

private:
    Document doc;
    std::string docName;
    int xBound;
    int yBound;
    bool drawPolyLines = false;


public:
    SegmentVisualization(const char* name, int xBound, int yBound);
    void setSegments(std::vector< std::vector<Vector2D>> &segmentToVectorMap);
    void setTakenEdges(std::set<std::pair<Vector2D,Vector2D>> &edges);
    void setTakenVertices(std::vector<Vector2D> &vertices);
    void setDrawPolyLines(bool drawPolyLines);

    void drawSvg();
    bool saveDocument();
    std::string to_string();

};


#endif //BA_SEGMENTVISUALIZATION_H
