# my

```shell
/mnt/d/develops/git/github/c/c_complier/cmake-build-debug/scanner_parser_test ../test_code/test1.c
[i]>>>Read data from file:../test_code/test1.c 
ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!|____ Type: ROOT Content: Program Depth: 0
    |____ Type: STATEMENT Content: Single_Block Depth: 1
        |____ Type: DEFINITION Content: Def_Func_Body_Block Depth: 2
            |____ Type: MODIFY Content: INT_TYPE Depth: 3
            |____ Type: DEFINITION Content: Func_NParam Depth: 3
            |   |____ Type: ID Content: main Depth: 4
            |____ Type: BODY Content: Body Depth: 3
                |____ Type: STATEMENT Content: Some_Stmt Depth: 4
                    |____ Type: STATEMENT Content: Some_Stmt Depth: 5
                    |   Qy|____ Type: STATEMENT Content: Some_Stmt Depth: 6
                    |   |   |____ Type: STATEMENT Content: Some_Stmt Depth: 7
                    |   |   |   |____ Type: STATEMENT Content: Some_Stmt Depth: 8
                    |   |   |   |   |____ Type: STATEMENT Content: Some_Stmt Depth: 9
                    |   |   |   |   |   �x|____ Type: STATEMENT Content: Some_Stmt Depth: 10
                    |   |   |   |   |   |   |____ Type: STATEMENT Content: Def_Stmt Depth: 11
                    |   |   |   |   |   |       |____ Type: DEFINITION Content: Def_Var Depth: 12
                    |   |   |   |   |   |           |____ Type: MODIFY Content: INT_TYPE Depth: 13
                    |   |   |   |   |   |           |____ Type: DEFINITION Content: Def_Single_Var Depth: 13
                    |   |   |   |   |   |               Qw|____ Type: DEFINITION Content: Var_ONLY Depth: 14
                    |   |   |   |   |   |                   |____ Type: DEFINITION Content: Block_Single_Vardef Depth: 15
                    |   |   |   |   |   |                       |____ Type: ID Content: a Depth: 16
                    |   |   |   |   |   �v|____ Type: STATEMENT Content: Exp_Stmt Depth: 10
                    |   |   |   |   |       |____ Type: OPERATION Content: Assign Depth: 11
                    |   |   |   |   |           |____ Type: EXPRESSION Content: ID_Exp Depth: 12
                    |   |   |   |   |           |   |____ Type: DEFINITION Content: Single_ID Depth: 13
                    |   |   |   |   |           |       �u|____ Type: ID Content: a Depth: 14
                    |   |   |   |   |           |____ Type: EXPRESSION Content: Const_Exp Depth: 12
                    |   |   |   |   |               |____ Type: CONST_INT Content: 1 Depth: 13
                    |   |   |   |   |____ Type: STATEMENT Content: Def_Stmt Depth: 9
                    |   |   |   |       �t|____ Type: DEFINITION Content: Def_Var Depth: 10
                    |   |   |   |           |____ Type: MODIFY Content: INT_TYPE Depth: 11
                    |   |   |   |           |____ Type: DEFINITION Content: Def_Single_Var Depth: 11
                    |   |   |   |               |____ Type: DEFINITION Content: Var_ONLY Depth: 12
                    |   |   |   |                   |____ Type: DEFINITION Content: Block_Single_Vardef Depth: 13
                    |   |   |   |                       as|____ Type: ID Content: b Depth: 14
                    |   |   |   |____ Type: STATEMENT Content: Exp_Stmt Depth: 8
                    |   |   |       |____ Type: OPERATION Content: Assign Depth: 9
                    |   |   |           �r|____ Type: EXPRESSION Content: ID_Exp Depth: 10
                    |   |   |           |   |____ Type: DEFINITION Content: Single_ID Depth: 11
                    |   |   |           |       |____ Type: ID Content: b Depth: 12
                    |   |   |           !r|____ Type: EXPRESSION Content: Const_Exp Depth: 10
                    |   |   |               |____ Type: CONST_INT Content: 2 Depth: 11
                    |   |   |____ Type: STATEMENT Content: Exp_Stmt Depth: 7
                    |   |       |____ Type: OPERATION Content: Assign Depth: 8
                    |   |           |____ Type: EXPRESSION Content: ID_Exp Depth: 9
                    |   |           |   !q|____ Type: DEFINITION Content: Single_ID Depth: 10
                    |   |           |       |____ Type: ID Content: a Depth: 11
                    |   |           |____ Type: OPERATION Content: Addition Depth: 9
                    |   |               �p|____ Type: EXPRESSION Content: ID_Exp Depth: 10
                    |   |               |   |____ Type: DEFINITION Content: Single_ID Depth: 11
                    |   |               |       |____ Type: ID Content: a Depth: 12
                    |   |               �o|____ Type: EXPRESSION Content: ID_Exp Depth: 10
                    |   |                   |____ Type: DEFINITION Content: Single_ID Depth: 11
                    |   |                       |____ Type: ID Content: b Depth: 12
                    |   1o|____ Type: STATEMENT Content: Exp_Stmt Depth: 6
                    |       |____ Type: CALL Content: Call_Args_Func Depth: 7
                    |           |____ Type: ID Content: print_int Depth: 8
                    |           |____ Type: ID Content: Func_Single_Arg Depth: 8
                    |               |____ Type: EXPRESSION Content: ID_Exp Depth: 9
                    |                   An|____ Type: DEFINITION Content: Single_ID Depth: 10
                    |                       |____ Type: ID Content: a Depth: 11
                    |____ Type: STATEMENT Content: Exp_Stmt Depth: 5
                        �m|____ Type: CALL Content: Call_Args_Func Depth: 6
                            |____ Type: ID Content: print_int Depth: 7
                            |____ Type: ID Content: Func_Single_Arg Depth: 7
                                |____ Type: EXPRESSION Content: ID_Exp Depth: 8
                                    |____ Type: DEFINITION Content: Single_ID Depth: 9
                                        �l|____ Type: ID Content: b Depth: 10
Gen Program
Gen Single_Block
Gen Def_Func_Body_Block
Gen INT_TYPE
Gen Func_NParam
Gen main
Gen Body
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Def_Stmt
Body_Gen Def_Var
Body_Gen Def_Single_Var

Process finished with exit code 132
```


是的，你的理解完全正确！FuncSymbol 确实是一个类似于符号表的概念，它是编译器中用于管理函数符号信息的关键数据结构。

让我详细解释一下：

FuncSymbol 与符号表的关系

1. 符号表

符号表是编译器中的一个全局数据结构，用于存储程序中所有标识符的信息，包括：
• 变量名、函数名、类名等

• 类型信息

• 作用域信息

• 内存地址或偏移量

2. FuncSymbol

FuncSymbol 通常是符号表中专门用于管理函数信息的一个条目或子系统。
// 简化的概念示例
class SymbolTable {
private:
std::unordered_map<std::string, Symbol*> symbols;

public:
void addFunction(const std::string& name, FuncSymbol* funcSym);
FuncSymbol* lookupFunction(const std::string& name);
};

class FuncSymbol : public Symbol {
private:
std::string functionName;
Type* returnType;
std::vector<VarSymbol*> parameters;
int stackSize;
// ... 其他函数特有信息
};


FuncSymbol 通常包含的信息

一个完整的 FuncSymbol 可能会包含：

基本标识信息

std::string name;          // 函数名
int lineNumber;            // 定义所在行
std::string sourceFile;    // 源文件名


类型信息

Type* returnType;                  // 返回值类型
std::vector<Type*> paramTypes;     // 参数类型列表
bool isVariadic;                   // 是否可变参数


作用域信息

Scope* functionScope;             // 函数内部作用域
SymbolTable* localSymbols;        // 局部变量符号表


代码生成相关信息

int stackFrameSize;               // 栈帧大小
int parameterOffset;              // 参数偏移
std::string labelName;            // 汇编标签名


在实际编译器中的角色

语法分析阶段

// 当解析到函数定义时
FuncSymbol* funcSym = new FuncSymbol("functionName");
funcSym->setReturnType(intType);
funcSym->addParameter("param1", intType);
symbolTable.addSymbol(funcSym);


语义分析阶段

// 检查函数调用是否合法
FuncSymbol* callee = symbolTable.lookupFunction(funcName);
if (!callee) {
// 错误：函数未定义
}
if (!checkParameters(callee, actualParams)) {
// 错误：参数不匹配
}


代码生成阶段

// 生成函数入口代码
void generateFunctionPrologue(FuncSymbol* funcSym) {
emit("push ebp");
emit("mov ebp, esp");
emit("sub esp, " + std::to_string(funcSym->getStackSize()));
}


具体实现示例

这是一个简化的 FuncSymbol 实现：
class FuncSymbol : public Symbol {
public:
enum Linkage {
EXTERNAL,    // 外部链接
INTERNAL,    // 内部链接
STATIC       // 静态链接
};

private:
Type* returnType;
std::vector<std::pair<std::string, Type*>> parameters;
Linkage linkage;
bool isDefined;
int frameSize;

public:
    FuncSymbol(const std::string& name, Type* retType, Linkage link = EXTERNAL)
        : Symbol(name, SYMBOL_FUNCTION), returnType(retType), linkage(link), 
          isDefined(false), frameSize(0) {}
    
    void addParameter(const std::string& name, Type* type) {
        parameters.push_back({name, type});
    }
    
    bool matchesCall(const std::vector<Type*>& argTypes) const {
        if (parameters.size() != argTypes.size()) return false;
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (!parameters[i].second->isCompatibleWith(argTypes[i])) {
                return false;
            }
        }
        return true;
    }
    
    // ... 其他方法
};


总结

FuncSymbol 是符号表系统中专门用于表示函数信息的组件，它：
• 继承自通用的 Symbol 基类
• 包含函数特有的信息（参数、返回类型等）
• 在编译的各个阶段被使用
• 帮助实现类型检查、重载解析、代码生成等功能
所以你的直觉是对的 - FuncSymbol 确实是符号表概念在函数管理方面的具体体现！

```shell
[3/3] Linking CXX executable test_func_symbol
FAILED: [code=1] test_func_symbol 
: && /usr/bin/c++ -g  CMakeFiles/test_func_symbol.dir/symbol_table/test_func_symbol.cpp.o CMakeFiles/test_func_symbol.dir/symbol_table/symbol.cpp.o CMakeFiles/test_func_symbol.dir/abstract_syntax_tree/AstNode.cpp.o CMakeFiles/test_func_symbol.dir/abstract_syntax_tree/DefFunASTNode.cpp.o CMakeFiles/test_func_symbol.dir/symbol_table/test_symbol_table.cpp.o CMakeFiles/test_func_symbol.dir/symbol_table/FuncSymbol.cpp.o -o test_func_symbol   && :
/usr/bin/ld: CMakeFiles/test_func_symbol.dir/symbol_table/test_symbol_table.cpp.o: in function `std::__new_allocator<char>::~__new_allocator()':
/mnt/d/develops/git/github/c/c_complier/symbol_table/test_symbol_table.cpp:6: multiple definition of `main'; CMakeFiles/test_func_symbol.dir/symbol_table/test_func_symbol.cpp.o:/mnt/d/develops/git/github/c/c_complier/symbol_table/test_func_symbol.cpp:6: first defined here
/usr/bin/ld: CMakeFiles/test_func_symbol.dir/symbol_table/FuncSymbol.cpp.o: in function `FuncSymbol::FuncSymbol(AbstractAstNode*)':
/mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:8: undefined reference to `AbstractAstNode::getAstNodeType()'
/usr/bin/ld: /mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:13: undefined reference to `AbstractAstNode::getContent[abi:cxx11]()'
/usr/bin/ld: /mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:14: undefined reference to `AbstractAstNode::getContent[abi:cxx11]()'
/usr/bin/ld: /mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:15: undefined reference to `DefFunASTNode::getArgList()'
/usr/bin/ld: /mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:18: undefined reference to `DefVarASTNode::getSymbolType()'
/usr/bin/ld: /mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:19: undefined reference to `DefVarASTNode::getSymbolType()'
/usr/bin/ld: /mnt/d/develops/git/github/c/c_complier/symbol_table/FuncSymbol.cpp:21: undefined reference to `DefVarASTNode::getSymbolType()'
```

parser.y
里面有main函数
