#include "symbol.h"
#include <iostream>

int main() {
    std::cout << "程序开始..." << std::endl;

    try {
        std::string var_name = "a";
        std::cout << "创建 Symbol 对象..." << std::endl;
        Symbol* var = new Symbol(var_name, SymbolType::var, 4);
        std::cout << "Symbol 创建成功" << std::endl;

        std::cout << "创建 SymbolTable 对象..." << std::endl;
        SymbolTable* symbol_table = new SymbolTable(false);
        std::cout << "SymbolTable 创建成功" << std::endl;

        std::cout << "获取偏移量..." << std::endl;
        int offset = symbol_table->getOffset();
        std::cout << "当前偏移量: " << offset << std::endl;

        std::cout << "设置新偏移量..." << std::endl;
        symbol_table->setOffset(offset + var->getWidth());

        std::cout << "设置符号偏移量..." << std::endl;
        var->setSymOffset(symbol_table->getOffset());

        std::cout << "添加符号到表..." << std::endl;
        symbol_table->addSymbol(var);
        std::cout << "符号添加成功" << std::endl;

        std::cout << "清理资源..." << std::endl;
        delete symbol_table;
        // 注意：var 已经被 symbol_table 管理，不要重复删除

        std::cout << "程序正常结束" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "异常捕获: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "未知异常" << std::endl;
        return 1;
    }

    return 0;
}