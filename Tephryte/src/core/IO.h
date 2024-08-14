//
// Created by Timothy on 8/14/2024.
//

#ifndef IO_H
#define IO_H

#include <fstream>
#include <vector>

namespace Tephryte::IO {

    inline std::vector<char> readFile(const std::string& path) {
        std::ifstream reader(path, std::ios::ate | std::ios::binary);


        if (!reader.is_open()) {
            TPR_ENGINE_ERROR("Failed to open file: ", path)
        }

        int fileSize = reader.tellg();
        std::vector<char> fileData(fileSize);

        reader.seekg(0);
        reader.read(fileData.data(), fileSize);

        return fileData;
    }
}

#endif //IO_H
