#ifndef C_COMPILER_ERROR_REPORT_H
#define C_COMPILER_ERROR_REPORT_H

#include <string>

/* ============ 阶段7：错误分析增强 ============
 * 统一错误报告：词法/语法/语义各阶段错误集中收集，
 * 输出 阶段 + 行号 + 列号 定位信息，编译结束打印分级统计摘要。
 * 退出码由错误是否存在决定（警告不阻断编译）。
 */

enum ErrorStage {
    ERR_STAGE_LEX = 0,
    ERR_STAGE_SYNTAX = 1,
    ERR_STAGE_SEMANTIC = 2,
    ERR_STAGE_COUNT = 3
};

/* 各阶段错误报告（统一格式）：
 *   词法错误 第 N 行 第 M 列: msg
 *   语法错误 第 N 行 第 M 列: msg
 *   [语义错误] 第 N 行: msg
 *   [警告] 第 N 行: msg
 */
void lexReportError(int line, int col, const std::string& msg);
void syntaxReportError(int line, int col, const std::string& msg);
void semanticReportError(int line, const std::string& msg);
void semanticReportWarning(int line, const std::string& msg);

/* 语义错误（行号已嵌入消息串，如 "第 10 行: 变量 'a' 未声明"） */
void semanticReportErrorStr(const std::string& msgWithLine);

/* 统计与摘要 */
int errorTotal();
int warningTotal();
int stageErrorCount(ErrorStage stage);
/* 存在错误或警告时打印统一统计到 stderr，否则不输出 */
void printErrorSummary();

#endif
