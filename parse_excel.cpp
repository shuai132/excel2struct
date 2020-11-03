#include "StructInfo.h"
#include "xlnt/xlnt.hpp"
#include "log.h"

static const int COL_OFFSET     = 'A' - 1;
static const int COL_IP_NAME    = 'A' - COL_OFFSET;
static const int COL_BLOCK      = 'B' - COL_OFFSET;
static const int COL_ADDRESS    = 'C' - COL_OFFSET;
static const int COL_BITS       = 'D' - COL_OFFSET;
static const int COL_REG        = 'E' - COL_OFFSET;
static const int COL_RW         = 'F' - COL_OFFSET;
static const int COL_DESC       = 'H' - COL_OFFSET;
static const int COL_TYPE       = 'I' - COL_OFFSET;

StructInfoVector parseExcel(const std::string& excelPath) {
    auto wb = std::make_unique<xlnt::workbook>(xlnt::path(excelPath));
    auto sheetNR = wb->sheet_by_title("V_NR");

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
        info.name = ipName;
        info.address = sheetNR.cell(COL_ADDRESS, row).value<std::string>();
        info.block = sheetNR.cell(COL_BLOCK, row).value<std::string>();
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
