#include "symbol.h"
#include <iostream>

int main() {
    std::string var_name = "a";
    Symbol* var = new Symbol(var_name, SymbolType::var, 4);

    // 修复：实例化 SymbolTable 对象
    SymbolTable* symbol_table = new SymbolTable(false);  // 或者使用栈对象

    symbol_table->setOffset(symbol_table->getOffset() + var->getWidth());
    var->setSymOffset(symbol_table->getOffset());
    symbol_table->addSymbol(var);

    // 清理资源
    delete symbol_table;

    return 0;
}