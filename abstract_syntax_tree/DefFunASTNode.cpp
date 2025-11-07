//
// Created by wdidada on 2025/11/7.
//

#include "DefFunASTNode.h"
#include <iostream>

// DefFunASTNode 实现
DefFunASTNode::DefFunASTNode() {
    // 初始化代码
}

DefFunASTNode::DefFunASTNode(AstNodeType nodeType, char* content)
    : AbstractAstNode(nodeType, content) {
    // 初始化代码
}

DefFunASTNode::~DefFunASTNode() {
    // 清理资源
}

AbstractAstNode* DefFunASTNode::getArgList() {
}

// DefVarASTNode 实现
DefVarASTNode::DefVarASTNode() {
}
SymbolType && DefVarASTNode::getSymbolType() {
    return SymbolType::function_name;
}

