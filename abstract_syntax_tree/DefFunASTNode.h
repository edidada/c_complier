//
// Created by wdidada on 2025/11/7.
//

#ifndef C_COMPLIER_DEFFUNASTNODE_H
#define C_COMPLIER_DEFFUNASTNODE_H
#include "AstNode.h"              // 子目录
#include "../symbol_table/symbol.h"              // 子目录

class DefFunASTNode :public AbstractAstNode{
public:
    // 构造函数
    DefFunASTNode();
    DefFunASTNode(AstNodeType nodeType, char* content = NULL);

    // 析构函数
    virtual ~DefFunASTNode();

    AbstractAstNode * getArgList();
};
class DefVarASTNode:public AbstractAstNode {
    public:
    DefVarASTNode();

    SymbolType && getSymbolType();
};

#endif //C_COMPLIER_DEFFUNASTNODE_H