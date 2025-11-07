#include "symbol.h"

int main() {
    std::string var_name = "a";
    Symbol* var = new Symbol(var_name, SymbolType:: var, 4);
    SymbolTable* symbol_table;
    symbol_table->setOffset(symbol_table->getOffset()+var->getWidth());
    var->setSymOffset(symbol_table->getOffset());
    symbol_table->addSymbol(var);
}