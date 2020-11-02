#include <fstream>
#include <vector>

#include "log.h"
#include "xlnt/xlnt.hpp"

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

static const int COL_OFFSET     = 'A' - 1;
static const int COL_IP_NAME    = 'A' - COL_OFFSET;
static const int COL_ADDRESS    = 'C' - COL_OFFSET;
static const int COL_BITS       = 'D' - COL_OFFSET;
static const int COL_REG        = 'E' - COL_OFFSET;
static const int COL_RW         = 'F' - COL_OFFSET;
static const int COL_DESC       = 'H' - COL_OFFSET;
static const int COL_TYPE       = 'I' - COL_OFFSET;

StructInfoVector parseExcel(const std::string& excelPath) {
    auto wb = std::make_unique<xlnt::workbook>(xlnt::path(excelPath));
    auto sheetNR = wb->sheet_by_title("V_NR");
    auto sheetNRSW = wb->sheet_by_title("V_NR_SW");

    LOG("parse start...");
    StructInfoVector structInfos;
    uint32_t row = 0;
    NEW_STRUCT:
    row++;
    auto ipName = sheetNR.cell(COL_IP_NAME, row).value<std::string>();
    if (ipName.empty()) {
        LOG("parse end");
        return structInfos;
    } else if (ipName != "V_NR") {
        throw std::runtime_error("ip name should be V_NR!!!");
    } else {
        StructInfo info;
        info.address = sheetNR.cell(COL_ADDRESS, row).value<std::string>();
        info.name = info.address; // todo: name it from table/file
        // info 包含多个位域 循环解析
        for(;;)
        {
            auto strOfCol = [&](int column) -> std::string {
                return sheetNR.cell(column, row).value<std::string>();
            };
            StructInfo::Prop prop{
                    .name   = strOfCol(COL_REG),
                    .bits   = strOfCol(COL_BITS),
                    .rw     = strOfCol(COL_RW),
                    .desc   = strOfCol(COL_DESC),
                    .type   = strOfCol(COL_TYPE),
            };
            info.props.push_back(prop);
            auto endWith = [](const std::string &str, const std::string &tail) {
                return str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
            };
            if (prop.bits == "[0]" or endWith(prop.bits, ":0]")) {
                structInfos.emplace_back(info);
                goto NEW_STRUCT;
            } else {
                row++;
                continue;
            }
        }
    }
}

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
            << prop.bits
            << " "
            << desc
            << " */"
            << std::endl;
        }
        file << "} " << "T_" << info.name << ";\n\n";
    }
    file.flush();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("need 1 params: excel path\n");
        exit(EXIT_FAILURE);
    }
    std::string excelPath = argv[1];
    LOG("read excel from: %s", excelPath.c_str());
    auto infos = parseExcel(excelPath);
    LOG("struct size: %lu", infos.size());
    generateCode(infos);
    return 0;
}
