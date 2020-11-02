#pragma once

#include <string>
#include <vector>

struct StructInfo {
    class Comment {
        std::string ipName;
        std::string block;
        std::string comments;
    };
    std::string comments;
    std::string name;
    std::string address;

    struct Prop {
        std::string name;
        std::string bits;
        std::string rw;
        std::string desc;
        std::string type;
    };
    std::vector<Prop> props;
};

using StructInfoVector = std::vector<StructInfo>;
