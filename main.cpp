#include "parse_excel.h"
#include "generate_code.h"
#include "log.h"

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
