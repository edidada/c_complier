#include "error_report.h"

#include <cstdio>

/* 分阶段错误计数 */
static int s_counts[ERR_STAGE_COUNT] = {0, 0, 0};
static int s_warnings = 0;

void lexReportError(int line, int col, const std::string& msg) {
    s_counts[ERR_STAGE_LEX]++;
    if (col > 0) {
        fprintf(stderr, "词法错误 第 %d 行 第 %d 列: %s\n", line, col, msg.c_str());
    } else {
        fprintf(stderr, "词法错误 第 %d 行: %s\n", line, msg.c_str());
    }
}

void syntaxReportError(int line, int col, const std::string& msg) {
    s_counts[ERR_STAGE_SYNTAX]++;
    if (col > 0) {
        fprintf(stderr, "语法错误 第 %d 行 第 %d 列: %s\n", line, col, msg.c_str());
    } else {
        fprintf(stderr, "语法错误 第 %d 行: %s\n", line, msg.c_str());
    }
}

void semanticReportError(int line, const std::string& msg) {
    s_counts[ERR_STAGE_SEMANTIC]++;
    if (line > 0) {
        fprintf(stderr, "[语义错误] 第 %d 行: %s\n", line, msg.c_str());
    } else {
        fprintf(stderr, "[语义错误] %s\n", msg.c_str());
    }
}

void semanticReportErrorStr(const std::string& msgWithLine) {
    s_counts[ERR_STAGE_SEMANTIC]++;
    fprintf(stderr, "[语义错误] %s\n", msgWithLine.c_str());
}

void semanticReportWarning(int line, const std::string& msg) {
    s_warnings++;
    if (line > 0) {
        fprintf(stderr, "[警告] 第 %d 行: %s\n", line, msg.c_str());
    } else {
        fprintf(stderr, "[警告] %s\n", msg.c_str());
    }
}

int errorTotal() {
    return s_counts[0] + s_counts[1] + s_counts[2];
}

int warningTotal() {
    return s_warnings;
}

int stageErrorCount(ErrorStage stage) {
    if (stage < 0 || stage >= ERR_STAGE_COUNT) return 0;
    return s_counts[stage];
}

void printErrorSummary() {
    if (errorTotal() == 0 && s_warnings == 0) return;
    /* 措辞刻意避开 "词法错误/语法错误/语义错误" 连续子串，避免与
     * run_parse/run_semantic 测试的计数/否定断言互相干扰 */
    fprintf(stderr, "错误统计: 词法%d 语法%d 语义%d | 警告%d | 共%d个\n",
            s_counts[0], s_counts[1], s_counts[2], s_warnings, errorTotal());
}
