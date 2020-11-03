#include <fstream>
#include <sstream>

#include "generate_code.h"
#include "template.h"
#include "string_utils.h"
#include "log.h"

void generateCode(const StructInfoVector& structInfos) {
    LOG("generateCode...");
    std::ofstream file("reg_def.h");
    file.clear();
    file << "#include <stdint.h>\n\n";
    for(const auto& info : structInfos) {
        int spareId = 1;
        int reservedId = 1;
        std::stringstream allProp;
        for(auto iter = info.props.rbegin(); iter != info.props.rend(); iter++) {
            const auto& prop = *iter;
            /// 处理名称的特殊字符
            std::string name = prop.name;
            std::replace_if(name.begin(), name.end(), [](const char& c){
                return c == '[' || c == ']' || c == ':' || c == '\''
                || c == '{' || c == '}'
                || c == ','
                || c == '\n'
                || c == ' ';
            }, '_');
            auto deleteEndChar = [](std::string& str, char c) {
                if (str[str.length() - 1] == c) {
                    str = str.substr(0, str.length() - 1);
                }
            };
            deleteEndChar(name, '_');//转换多余的
            deleteEndChar(name, ',');//来自excel
            // 处理这种形式的名字：5'h0
            if ('0' <= name[0] and name[0] <= '9') {
                name = "_" + name;
            }
            if (name == "spare") {
                name += std::to_string(spareId++);
            } else if (name == "reserved") {
                name += std::to_string(reservedId++);
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

            /// rw
            std::string rw;
            if (prop.rw == "R/W") rw = "__IO";
            else if (prop.rw == "R") rw = "__O";
            else if (prop.rw == "W") rw = "__I";

            std::string tmp = ReplaceAll(TEMPLATE_PROP, "${RW}", rw);
            tmp = ReplaceAll(tmp, "${NAME}", name);
            tmp = ReplaceAll(tmp, "${WIDTH}", bits);
            allProp << tmp
                 << "/* "
                 << prop.bits << " "
                 << prop.rw << " "
                 << desc
                 << " */"
                 << std::endl;
        }
        std::string tmp(TEMPLATE_DEFINE);
        auto rp = [&](auto target, auto rep){
            tmp = ReplaceAll(tmp, target, rep);
        };
        rp("${BLOCK}", info.block);
        rp("${OFFSET}", info.address.substr(7));
        rp("${STRUCT_PROP}", allProp.str());
        file << tmp;
    }
    file.flush();
}
