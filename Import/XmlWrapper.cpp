//
// Created by gharg on 21.04.18.
//

#include "XmlWrapper.h"

std::string Utils::XmlWrapper::toXml(const Utils::XmlWrapper::Node &root, ulong indent) {
    std::string xml = "";
    if(indent == 0)
        xml += R"(<?xml version="1.0" encoding="UTF-8"?>)";

    std::string tag = printIndent(indent) + "<" + root.tagName + ">";
    if(!root.subNodes.empty()) {
        tag += "\n";
        for (auto& node : root.subNodes) {
            tag += toXml(node,indent+1) ;
        }
        tag += printIndent(indent);
    } else {
        tag += root.value;
    }
    tag += "</" + root.tagName + ">" +  "\n";
    xml += tag;
    return xml;
}

std::string Utils::XmlWrapper::printIndent(ulong indent) {
    std::string indentString;
    for (ulong i = 0; i < indent; ++i) {
        indentString += "\t";
    }
    return indentString;
}

