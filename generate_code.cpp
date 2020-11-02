#include <fstream>

#include "generate_code.h"
#include "log.h"

void generateCode(const StructInfoVector& structInfos) {
    LOG("generateCode...");
    std::ofstream file("reg_def.h");
    file.clear();
    file << "#include <stdint.h>\n\n";
    for(const auto& info : structInfos) {
        file << "typedef struct\n{\n";
        for(const auto& prop : info.props) {
            /// 处理名称的特殊字符
            std::string name = prop.name;
            std::replace_if(name.begin(), name.end(), [](const char& c){
                return c == '[' || c == ']' || c == ':';
            }, '_');
            if (name[name.length()-1] == '_') {
                name = name.substr(0, name.length()-1);
            }

            /// bit有两种形式: [x] [x:y]
            std::string bits(prop.bits.begin()+1, prop.bits.end()-1); // 去掉[]
            {
                auto r = bits.find(':');
                if (r != std::string::npos) {
                    int L = atoi(bits.substr(0, r).c_str());
                    int R = atoi(bits.substr(r + 1).c_str());
                    bits = std::to_string(L - R + 1);
                } else {
                    bits = "1";
                }
            }

            /// desc处理一些格式问题
            auto desc = prop.desc;
            // 不换行
            std::replace_if(desc.begin(), desc.end(), [](const char& c){
                return c == '\n';
            }, ' ');

            file << "    volatile uint32_t "
                 << name
                 << ":"
                 << bits
                 << "; "
                 << "/* "
                 << prop.bits << " "
                 << prop.rw << " "
                 << desc
                 << " */"
                 << std::endl;
        }
        file << "} " << "T_" << info.name << ";\n\n";
    }
    file.flush();
}
