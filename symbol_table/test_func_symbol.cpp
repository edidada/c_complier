#include <iostream>
#include "FuncSymbol.h"
#include "symbol.h"
#include "../abstract_syntax_tree/AstNode.h"

int main() {
  FuncTable table;

  // void foo()
  FuncSymbol* foo = new FuncSymbol(
      "foo",
      std::vector<SymbolType>{},
      SymbolType::integer
  );

  // int bar(int a, float b)
  FuncSymbol* bar = new FuncSymbol(
      "bar",
      std::vector<SymbolType>{
          SymbolType{SymbolType::integer},
          SymbolType{SymbolType::var}
      },
      SymbolType{SymbolType::integer}
  );

  // float bar(float x)   // overload
  FuncSymbol* bar_f = new FuncSymbol(
      "bar",
      std::vector<SymbolType>{SymbolType{SymbolType::integer}},
      SymbolType{SymbolType::integer}
  );

  // 添加
  table.addFunction(foo);
  table.addFunction(bar);
  table.addFunction(bar_f);

  // 查询
  const char* keys[] = {"foo", "bar_i_f", "bar_f", "unknown"};
  for (const auto* k : keys) {
    FuncSymbol* f = table.findFunction(k);
    if (f) {
      std::cout << "Found: " << f->getFunName()
                << "  key=" << f->getKeyName()
                << "  offset=" << f->getTotalArgOffset() << '\n';
    } else {
      std::cout << "Not found: " << k << '\n';
    }
  }

  // 清理（实际项目建议使用 unique_ptr）
  delete foo;
  delete bar;
  delete bar_f;
  return 0;
}