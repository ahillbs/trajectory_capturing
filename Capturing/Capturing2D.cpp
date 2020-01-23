//
// Created by gharg on 6/12/17.
//

#include <random>
#include <functional>
#include <algorithm>
#include <set>
#include <CGAL/box_intersection_d.h>
#include "Header/Capturing2D.h"
#include "../Visualization/SegmentVisualization.h"
#include "../Import/Imports.h"

Capturing2D::Capturing2D(const std::string &instanceName, const std::string &generator, ulong sharedEndpoints)
		: CapturingGraph(), converted(false), segmentGraph() {
	metaData.name = instanceName;
	createMetaData(&metaData,generator,sharedEndpoints);
}
Capturing2D::Capturing2D(const std::string &instanceName, std::vector<Segment<NT>> segments, uint xBound, uint yBound,
						 const std::string &generator, ulong sharedEndpoints)
		: CapturingGraph(), converted(false), segmentGraph(), xBound(xBound), yBound(yBound) {
	for (uint i = 0; i < segments.size(); ++i) {
        this->segments.push_back(segments[i]);
		this->segmentGraph.addVertex();
		vectorFamilies.emplace_back(std::vector<Vector2D>());
		vertexFamilies.emplace_back(std::vector<Vertex>());
    }
    //do not always convert instances at start to save time
	//convertSegments2Graph();
	metaData.name = instanceName;
	createMetaData(&metaData,generator,sharedEndpoints);
}


Capturing2D::Capturing2D(std::vector<Segment<NT>> segments, uint xBound, uint yBound,
						 const CapturingMetaData &_metaData)
		: CapturingGraph(), converted(false), segmentGraph(), xBound(xBound), yBound(yBound) {
    for (uint i = 0; i < segments.size(); ++i) {
        this->segments.push_back(segments[i]);
        this->segmentGraph.addVertex();
        vectorFamilies.emplace_back(std::vector<Vector2D>());
        vertexFamilies.emplace_back(std::vector<Vertex>());
    }
    metaData = _metaData;
    if(metaData.generator == "Imported") {
    	createMetaData(&metaData,metaData.generator,0);
    }
}

Capturing2D::Capturing2D(std::vector<std::vector<Vector2D>> &families, uint xBound, uint yBound,
                         const CapturingMetaData &_metaData)
        : CapturingGraph(), converted(false), segmentGraph(), xBound(xBound), yBound(yBound) {
    for (uint i = 0; i < families.size(); ++i) {
        vectorFamilies.emplace_back(families[i]);
        vertexFamilies.emplace_back(std::vector<Vertex>());
    }
    metaData = _metaData;
    if(metaData.generator == "Imported") {
        createMetaData(&metaData,metaData.generator,0);
    }
}
//typedef boost::property<boost::edge_weight_t, double> edgeWeightProperty;
//typedef boost::adjacency_list<boost::listS, boost::vecS,boost::undirectedS,boost::no_property,edgeWeightProperty> Graph;
//typedef Graph::vertex_descriptor Vertex;
//typedef Graph::edge_descriptor Edge;



void Capturing2D::convertSegments2Graph() {
	if(converted)
		return;
	std::cout << "start converting" << std::endl;
    //std::vector<Vertex> vecs;
    //std::map<unsigned int,std::vector<Vector2D>> vectorFamilies;
	//auto * vecSet = this->calculateSegmentIntersection();
	std::set<Vector2D> vecSet;

	//If segments are filled, it is an instance with straight seg
	if(!segments.empty()) {
		Segments cgal_segments;
		convertToCGALSegments(segments, &cgal_segments);
		std::set<Point_2> points;
		std::vector<std::set<Point_2>> familyPoints(segments.size());
		std::map<Point_2, std::set<Vertex>> vectorVertexMap;
		computeIntersectionPoints(&cgal_segments, &points, &familyPoints, &vectorVertexMap);
		ulong edgesInFamilies = 0;
		for (auto &family : familyPoints) {
			edgesInFamilies += family.size() - 1;
		}

		convertCGALToGraph(cgal_segments, &points, &familyPoints, &vectorVertexMap, &vecSet);

	} else { // if no segments are filled it is an instance that may are not straight so we won't calculate intermediate intersections
		for(auto& family : this->vectorFamilies) {
			for(auto& vec : family) {
				vecSet.insert(vec);
			}
		}
	}
	// translate points to vertices and edges in Graph
	this->translateGraph(vecSet);

	/*delete(report->segments1);
	delete(report->points);
	delete(report->vectorVertexMap);
	delete(report->familyPoints);
	delete(report->points);
	delete(report);*/
	//delete(vecSet);
    this->metaData.vAmount = this->getGraph()->getVertices().size();
    converted = true;
}
/*
CapturingSolution Capturing2D::solveKCapturing() {
	if(!converted) {
		this->convertSegments2Graph();
	}
	return CapturingGraph::solveKCapturing();
}
*/
void Capturing2D::printGraph() {
	std::string name = "TestGraph";
	printGraph(name);
}

void Capturing2D::printGraph(const char* cName) {
	std::string name(cName);
	printGraph(name);
}

void Capturing2D::printGraph(std::string docName) {
	this->printGraph(docName,CapturingSolution(this));
}

void Capturing2D::printGraph(const char *cName, const CapturingSolution &solution) {
	std::string docName(cName);
	printGraph(docName, solution);
}

void Capturing2D::printGraph(std::string docName, const CapturingSolution &solution) {
	this->convertSegments2Graph();
	if(xBound > 0 && yBound > 0  && solution.getGraph() == this) {
		docName += ".svg";
		SegmentVisualization document(docName.data(), xBound, yBound);
		document.setSegments(vectorFamilies);
		std::set<std::pair<Vector2D,Vector2D>> takenEdges;
		if(solution.getVertexSolution().size() > 0) {
		    if(!this->segments.empty()) {
                for (uint k = 0; k < vectorFamilies.size(); ++k) {
                    auto indexPair = this->getFamilyIndexPair(k, solution.getVertexSolution());
                    if (indexPair.first < indexPair.second) {
                        takenEdges.insert(std::pair<Vector2D, Vector2D>(vectorFamilies[k][indexPair.first],
                                                                        vectorFamilies[k][indexPair.second]));
                    }
                }
		    } else {
                for (const Edge& edge : solution.getEdgeSolution()) {
                    takenEdges.insert(std::pair<Vector2D, Vector2D>(this->getVectorMapping().at(edge.first),this->getVectorMapping().at(edge.second)));
                }
		    }
            std::vector<Vector2D> vertices;
            auto &vertexSol = solution.getVertexSolution();
            for (auto j = vertexSol.begin(); j != vertexSol.end(); ++j) {
                vertices.push_back(vectorMapping[*j]);
            }
            document.setTakenVertices(vertices);
            document.setTakenEdges(takenEdges);
        }
		metaData.pathToInstance = docName;
		document.setDesc(Utils::XmlWrapper::toXml(this->metaData.toXMLNode(), 2));
		document.setDrawPolyLines(segments.empty());
		document.drawSvg();
		if(document.saveDocument()) {
			std::cout << "Document " << docName << " saved" << std::endl;
		}
	}
}

undirectedAdjGraph Capturing2D::getSolutionSubgraph(const std::set<Vertex> &greedySolution) {
	std::set<Vertex> allVertexSolution;
	for (unsigned long segIndex = 0; segIndex < segments.size(); ++segIndex) {
		auto indexes = getFamilyIndexPair(segIndex, greedySolution);
		unsigned long j = indexes.first;
		unsigned long k = indexes.second;
		//found a segment track
		if(k > j) {
			for (unsigned long l = j; l <= k ; ++l) {
				// add every vertex to the new subgraph
				allVertexSolution.insert(vertexFamilies[segIndex][l]);
			}
		}
	}
	return graph.createSubgraph(allVertexSolution);
}

undirectedAdjGraph Capturing2D::computeConnectedSubgraph(std::map<Vector2D, Vertex> *vertexMappingSub,
														 std::map<Vertex, Vector2D> *vectorMappingSub,
														 std::vector<std::vector<Vector2D>> *vectorFamiliesSub) const {
	undirectedAdjGraph subGraph = graph;
	std::set<Vertex> connectedVertices;
	while(connectedVertices.size() != subGraph.getSize()) {
		connectedVertices.clear();
		vectorMappingSub->clear();
		vertexMappingSub->clear();
		for (Vertex v : subGraph.getVertices()) {
			if (subGraph.getEdges(v).size() > 1) {
				connectedVertices.insert(v);
				vertexMappingSub->insert(std::pair<Vector2D, Vertex>(vectorMapping.at(v), v));
				vectorMappingSub->insert(std::pair<Vertex, Vector2D>(v, vectorMapping.at(v)));
			}
		}
		subGraph = subGraph.createSubgraph(connectedVertices);
	}
	//translate the points from original graph to subgraph
	for (unsigned int i = 0; i < vectorFamilies.size(); ++i) {
		vectorFamiliesSub->push_back(std::vector<Vector2D>());
		for (Vector2D vec : vectorFamilies[i]) {
			if(connectedVertices.find(vertexMapping.at(vec)) != connectedVertices.end()) {
				(*vectorFamiliesSub)[i].push_back(vec);
			}
		}
	}
	return subGraph;
}

void Capturing2D::setBounds(uint xBound, uint yBound) {
	this->xBound = xBound;
	this->yBound = yBound;
}

void Capturing2D::computeIntersectionPoints(Segments *segments, std::set<Point_2> *points,
											std::vector<std::set<Point_2>> *familyPoints,
											std::map<Point_2, std::set<Vertex>> *vectorVertexMap) {
	// Create the corresponding vector of bounding boxes
	std::vector<Box> boxes;
	for ( Iterator i = segments->begin(); i != segments->end(); ++i) {
		Box b = Box(i->bbox(), i);
		boxes.push_back(b);
	}


	// Create the corresponding vector of pointers to bounding boxes
	std::vector<Box *> ptr;
	for ( std::vector<Box>::iterator i = boxes.begin(); i != boxes.end(); ++i)
		ptr.push_back(&*i);


	// Run the self intersection algorithm with all defaults on the
	// indirect pointers to bounding boxes. Avoids copying the boxes.

	CGAL::box_self_intersection_d( ptr.begin(), ptr.end(), CGAL_CapturingReport(segments,familyPoints,vectorVertexMap,points));

}

void Capturing2D::convertToCGALSegments(const std::vector<Segment<NT>> &segs, Segments *cgal_segments) {
	cgal_segments->clear();
	for (auto s : segs) {
		Point_2 cgal_start(s.GetStartVector().x,s.GetStartVector().y);
		Point_2 cgal_end(s.GetEndVector().x,s.GetEndVector().y);


		cgal_segments->push_back(Segment_2(cgal_start,cgal_end));
	}
}

void Capturing2D::convertCGALToGraph(const Segments &segments, std::set<Point_2> *points,
									 std::vector<std::set<Point_2>> *familyPoints,
									 std::map<Point_2, std::set<Vertex>> *vectorVertexMap, std::set<Vector2D> *vecSet) {

	for (uint i = 0; i < segments.size() ; ++i) {
		if(points->find(segments[i].source()) == points->end()) {
			points->insert(segments[i].source());
		}
		if(points->find(segments[i].target()) == points->end()) {
			points->insert(segments[i].target());
		}
	}
	vectorFamilies = std::vector<std::vector<Vector2D>>(segments.size());
	//translate points to vectorFamilies
	if(vectorFamilies.size() != familyPoints->size()) {
		std::cout << "vectorFamilies and reportFamilies does not have the same size: "
				  << vectorFamilies.size() << " to " << familyPoints->size() << std::endl;
	}
	//vectorFamilies.clear();
	for (uint j = 0; j < familyPoints->size(); ++j) {
		//vectorFamilies.push_back(std::vector<Vector2D>());
		//auto start = segments[j].GetStartVector();
		//auto end = segments[j].GetEndVector();
		//vectorFamilies[j].push_back(start);
		auto segStart = segments[j].source();
		auto segEnd = segments[j].target();
		if((*familyPoints)[j].find(segStart) == (*familyPoints)[j].end()) {
			(*familyPoints)[j].insert(segStart);
		}
		if((*familyPoints)[j].find(segEnd) == (*familyPoints)[j].end()) {
			(*familyPoints)[j].insert(segEnd);
		}
		for (Point_2 point_2 : (*familyPoints)[j]) {

			Vector2D vec(CGAL::to_double(point_2.x()), CGAL::to_double(point_2.y()));
			vectorFamilies[j].push_back(vec);
		}
		//vectorFamilies[j].push_back(end);
	}
	for (auto point_pair : *vectorVertexMap) {
		std::pair<Vector2D , std::set<Vertex>> vec_pair(Vector2D(CGAL::to_double(point_pair.first.x()),CGAL::to_double(point_pair.first.y()))
														,std::set<Vertex>());
		vec_pair.second.insert(point_pair.second.begin(),point_pair.second.end());
		vectorSegmentMapping.insert(vec_pair);
	}
	vecSet->clear();
	for(auto point : *points) {
		vecSet->insert(Vector2D(CGAL::to_double(point.x()),CGAL::to_double(point.y())));
	}
}

void Capturing2D::translateGraph(const std::set<Vector2D> &pSet) {
	vertexMapping.clear();
	vectorMapping.clear();
	graph.clear();

	for(auto vec : pSet) {
		Vertex v = graph.addVertex();
		vertexMapping.insert(std::pair<Vector2D,Vertex>(vec,v));
		vectorMapping.insert(std::pair<Vertex,Vector2D>(v,vec));
	}
	// translate vector families
	vertexFamilies = std::vector<std::vector<Vertex>>(vectorFamilies.size());
	for (uint j = 0; j < vectorFamilies.size(); ++j) {
	    // Only sort vertices, if the instance is coming from an segment graph
	    if (!this->segments.empty()) {
            std::sort(vectorFamilies[j].begin(), vectorFamilies[j].end());
        }
		Vertex v = vertexMapping[vectorFamilies[j][0]];
		vertexFamilies[j].push_back(v);
		for (uint k = 1; k < vectorFamilies[j].size(); ++k) {
			Vertex v2 = vertexMapping[vectorFamilies[j][k]];
			vertexFamilies[j].push_back(v2);
			NT weight = GeoUtil::getLengthVector2(vectorFamilies[j][k-1],vectorFamilies[j][k]);
			auto edgeResult = graph.setEdge(v, v2, CGAL::to_double(weight));
			//std::cout << "Vector familie: " <<  vectorFamilies[j][k].x << "," << vectorFamilies[j][k].y << std::endl;
			//std::cout << "Family: "<< j << " Edge: " << v << "," << v2 << ": " << weight << std::endl;
			//std::cout << std::fixed << std::setprecision(20) << "Family: "<< j << " Edge: " << vectorFamilies[j][k-1] << ";" << vectorFamilies[j][k-1] << std::endl;
			if(!edgeResult) {
				std::cout << "Fehler beim Eintragen in die Adjazenzliste bei " << v << "," << v2 << std::endl;
			}
			v = v2;
		}
	}

	// map vector segments to vertex family indexes
	for (auto l = vectorSegmentMapping.begin(); l != vectorSegmentMapping.end(); ++l) {
		vertexInFamiliesMapping.insert(std::pair<Vertex,std::set<Vertex>>(vertexMapping[l->first],std::set<Vertex>()));
		vertexInFamiliesMapping[vertexMapping[l->first]].insert(l->second.begin(),l->second.end());
	}
}

std::pair<std::pair<Vector2D, Vector2D>, NT>
Capturing2D::getFamilyMaxLength(ulong segment, std::vector<std::vector<Vector2D>> vectorFamiliesSub) {
	NT longestEdgeLength = 0;
	Vector2D longestVectorPair[2];
	if (vectorFamiliesSub[segment].size() > 1) {
		NT edgeLength = GeoUtil::getLengthVector2(vectorFamiliesSub[segment][0],
													   vectorFamiliesSub[segment][vectorFamiliesSub[segment].size() -
																				  1]);
		longestEdgeLength = edgeLength;

		longestVectorPair[0] = vectorFamiliesSub[segment][0];
		longestVectorPair[1] = vectorFamiliesSub[segment][vectorFamiliesSub[segment].size() - 1];

		return std::make_pair(std::make_pair(longestVectorPair[0], longestVectorPair[1]), longestEdgeLength);
	} else {
		return std::make_pair(std::make_pair(Vector2D(0,0), Vector2D(0,0)), longestEdgeLength);
	}

}

const std::map<Vector2D, Vertex> &Capturing2D::getVertexMapping() const {
	return vertexMapping;
}

const std::map<Vertex, Vector2D> &Capturing2D::getVectorMapping() const {
	return vectorMapping;
}

const std::vector<std::vector<Vector2D>> &Capturing2D::getVectorFamilies() const {
	return vectorFamilies;
}

const std::map<Vector2D, std::set<Vertex>> &Capturing2D::getVectorSegmentMapping() const {
	return vectorSegmentMapping;
}

const CapturingMetaData &Capturing2D::getMetaData() const {
	return metaData;
}

void Capturing2D::createMetaData(CapturingMetaData *data, const std::string &generatorName, ulong endpoints) {
	//this->convertSegments2Graph();
	std::string name = data->name;
	ulong segAmount = this->getVertexFamilies()->size();
	ulong vertAmount = 0;//this->getGraph()->getVertices().size();
	std::string emptyString = "";
	*data = CapturingMetaData(name,segAmount,vertAmount,emptyString,generatorName,endpoints);
}

void Capturing2D::setMetaData(const CapturingMetaData &metaData) {
	Capturing2D::metaData = metaData;
}

bool Capturing2D::isConverted() const {
    return converted;
}

std::string Capturing2D::to_string() {
	return this->to_string(CapturingSolution(this));
}

std::string Capturing2D::to_string(CapturingSolution solution) {
	this->convertSegments2Graph();
	if(xBound > 0 && yBound > 0  && solution.getGraph() == this) {
		SegmentVisualization document("", xBound, yBound);
		document.setSegments(vectorFamilies);
		std::set<std::pair<Vector2D,Vector2D>> takenEdges;
		if(solution.getVertexSolution().size() > 0) {
			if(!this->segments.empty()) {
                std::cout << "Print edge solution from start and endpoints" << std::endl;
                for (uint k = 0; k < vectorFamilies.size(); ++k) {
                    auto indexPair = this->getFamilyIndexPair(k, solution.getVertexSolution());
                    if (indexPair.first < indexPair.second) {
                        takenEdges.insert(std::pair<Vector2D, Vector2D>(vectorFamilies[k][indexPair.first],
                                                                        vectorFamilies[k][indexPair.second]));
                    }
                }
            } else {
                std::cout << "Print edge solution instead of start and endpoints" << std::endl;
                for (const Edge& edge : solution.getEdgeSolution()) {
                    takenEdges.insert(std::pair<Vector2D, Vector2D>(this->getVectorMapping().at(edge.first),this->getVectorMapping().at(edge.second)));
                }
            }

			std::vector<Vector2D> vertices;
			auto &vertexSol = solution.getVertexSolution();
			for (auto j = vertexSol.begin(); j != vertexSol.end(); ++j) {
				vertices.push_back(vectorMapping[*j]);
			}
			document.setTakenVertices(vertices);
			document.setTakenEdges(takenEdges);
		}
		document.setDesc(Utils::XmlWrapper::toXml(this->metaData.toXMLNode(), 2));
		document.setDrawPolyLines(segments.empty());
		document.drawSvg();
		return document.to_string();
	}
	return "";
}

const std::vector<Segment<NT>> &Capturing2D::getSegments() const {
    return segments;
}






