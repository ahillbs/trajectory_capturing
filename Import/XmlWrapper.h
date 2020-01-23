//
// Created by gharg on 21.04.18.
//

#ifndef PROJECT_XMLWRAPPER_H
#define PROJECT_XMLWRAPPER_H

#define GET_VARIABLE_NAME(Variable) (#Variable)

#include <string>
#include <vector>

namespace Utils {
    class XmlWrapper {
        public:
        struct Node {
            std::string tagName = "unknown";
            std::vector<Node> subNodes;
            std::string value;
            //std::vector<std::pair<std::string,std::string>> attributes;
            explicit Node() {;}
            explicit Node(const std::string & _tagName)  {tagName = _tagName;}
            explicit Node(const std::string & _tagName,const std::string &_value) {
                tagName = _tagName;
                value = _value;
            }
            explicit Node(const char *_tagName)  {tagName = std::string(_tagName);}

        };
        static std::string toXml(const Utils::XmlWrapper::Node &root, ulong indent = 0);

        static std::string printIndent(ulong indent);
    };




    static std::string& operator<<(std::string& lhs,const XmlWrapper::Node& rhs) {
        std::string tag = "<" + rhs.tagName + ">";
        if(!rhs.subNodes.empty()) {
            for (auto& node : rhs.subNodes) {
                tag << node;
            }
        } else {
            tag += rhs.value;
        }
        tag += "</" + rhs.tagName + ">";
        lhs += tag;
        return lhs;
    }

    /*
    template <typename J>
    XmlWrapper::Node& operator<<(XmlWrapper::Node &node,const J &value) {
        XmlWrapper::Node node1(GET_VARIABLE_NAME(value));
        node1.value = std::to_string(value);
    }
    template <typename J>
    XmlWrapper::Node& operator<<(XmlWrapper::Node &node,const std::vector<J> &vec) {
        XmlWrapper::Node node1(GET_VARIABLE_NAME(vec));
        for (int i = 0; i < ; ++i) {
            node1.subNodes.push_back(vec[i]);
        }
    }
     */
}



#endif //PROJECT_XMLWRAPPER_H
