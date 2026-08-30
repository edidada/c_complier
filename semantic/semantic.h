#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "abstract_syntax_tree/AstNode.h"

/*
 * 语义分析（阶段 3）：类型检查
 * 在 AST 上做 符号收集 + 类型推导 + 规则检查（R1~R10），与代码生成解耦。
 * 所有错误统一收集并打印到 stderr，格式： [语义错误] 第 N 行: 描述
 * 返回错误个数（0 = 通过）。
 */
int typeCheck(AbstractAstNode* root);

#endif // SEMANTIC_H
