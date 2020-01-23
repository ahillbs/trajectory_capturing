//
// Created by gharg on 28.05.18.
//

#ifndef CAPTURINGGENERATOR_PATHNORMIFIER_H
#define CAPTURINGGENERATOR_PATHNORMIFIER_H

#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace PathNormifier {
    boost::filesystem::path norm_Path(const std::string &path, const std::string &directoryAddition) {
        boost::filesystem::path filePath;
        ulong startPointPath = 0;
        if(path.substr(0,2) == "./") {
            startPointPath = 2;
        }
        std::string slashAdd;
        if(!path.empty() && path[path.size()-1] != "/"[0]) {
            slashAdd = "/";
        }
        if(!path.empty()) {
            filePath = boost::filesystem::path(path.substr(startPointPath) + slashAdd + directoryAddition + "/");
        } else {
            filePath = boost::filesystem::path(directoryAddition + "/");
        }
        return boost::filesystem::absolute(filePath);

    }
}
#endif //CAPTURINGGENERATOR_PATHNORMIFIER_H
