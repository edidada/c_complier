%option noyywrap
%option yylineno
LETTER [a-zA-Z_]
DIGIT [0-9]
WHITE_SPACE [ \t\n]
CONST_INT (([1-9]){DIGIT}*)|0

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "parser.tab.h"
#include "error_report.h"

/* ===== 词法分析模式（--lex）=====
 * 供 parser.y main 的 "--lex" 分支使用：输出 单词/词素/属性 三列，
 * 对照《词法分析器上机要求》样例格式。
 */
int lex_only_mode = 0;

/* ===== 阶段7：列号跟踪 =====
 * flex 不直接提供列号，用 YY_USER_ACTION 在每次匹配后维护：
 * lex_col 为扫描游标列号，lex_tok_col 为当前 token 起始列号（报错定位用）。
 */
static int lex_col = 1;
static int lex_tok_col = 1;
#define YY_USER_ACTION update_lex_column();
static void update_lex_column(void) {
    lex_tok_col = lex_col;
    for (int i = 0; i < yyleng; ++i) {
        if (yytext[i] == '\n') lex_col = 1;
        else lex_col++;
    }
}
int lexCurrentCol(void) { return lex_tok_col; }

static const char* lex_token_name(int tok, const char* lexeme);
static void lex_emit(int tok, const char* lexeme);

%}

%%

"while"     { return WHILE; }
"for"       { return FOR; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
"if"        { return IF; }
"else"      { return ELSE; }
"then"      { return THEN; }
"printf"    { return PRINTF; }
"scanf"     { return SCANF; }
"return"    { return RETURN; }
"void"      { return VOID; }
"int"       { return INT; }
"'"         { return S_QUO; /* 单引号 */}
"\""        { return D_QUO; /* 双引号 */}
","         { return COMMA; }
";"         { return SEMI; }
"="         { return ASSIGN_OP; }
"("         { return '('; }
")"         { return ')'; }
"["         { return '['; }
"]"         { return ']'; }
"{"         { return '{'; }
"}"         { return '}'; }

{LETTER}([a-zA-Z_]|{DIGIT})*  { yylval.str = strdup(yytext); return IDENTIFIER; }

{CONST_INT} {
    /* 整数溢出检查：超出 int 范围时报词法错误（不中断扫描） */
    long v = strtol(yytext, NULL, 10);
    if (v > 2147483647L) {
        lexReportError(yylineno, lex_tok_col,
                       std::string("整数常量溢出 '") + yytext + "' (超出 int 范围)");
    }
    yylval.str = strdup(yytext);
    return CONST;
}

"+"    { return ADD; /* 加 */}
"-"    { return SUB; /* 减 */}
"*"    { return MUL; /* 乘 */}
"/"    { return DIV; /* 除 */}
"%"    { return MOD; /* 取余 */}
"^"    { return POW; /* 求幂 */}

"=="   { return EQ_OP; /* 等于 */}
">"    { return GT_OP; /* 大于 */}
"<"    { return LT_OP; /* 小于 */}
">="   { return GE_OP; /* 大于等于 */}
"<="   { return LE_OP; /* 小于等于 */}
"!="   { return NE_OP; /* 不等于 */}

"&&"   { return AND; /* 逻辑与 */}
"||"   { return OR;  /* 逻辑或 */}
"!"    { return NOT; /* 逻辑非 */}
"&"    { return SINGLAND; /* 取地址 */}

"/*"([^*]|\*+[^*/])*\*+"/"    { /* 完整块注释：跳过 */ }
"/*" {
    lexReportError(yylineno, lex_tok_col, "未闭合的块注释");
    /* 丢弃行尾剩余内容，避免其余字符被逐字节报为非法 */
    int c;
    while ((c = yyinput()) != EOF && c != '\n') { }
    if (c == '\n') unput(c);
}
"//"[^\n]*   { /* 行注释：跳过 */ }
{WHITE_SPACE} { /* 忽略空白 */ }
. {
    /* 非法字符：报错并继续扫描 */
    lexReportError(yylineno, lex_tok_col, std::string("非法字符 '") + yytext + "'");
}
%%

/* ==================== 词法输出工具（--lex 模式） ==================== */

/* token 编号 → 上机要求风格的单词名 */
static const char* lex_token_name(int tok, const char* lexeme) {
    if (tok == IDENTIFIER && strcmp(lexeme, "main") == 0) return "MAIN";
    switch (tok) {
        case WHILE:    return "WHILE";
        case FOR:      return "FOR";
        case IF:       return "IF";
        case ELSE:     return "ELSE";
        case THEN:     return "THEN";
        case RETURN:   return "RETURN";
        case INT:      return "TYPE";
        case VOID:     return "VOID";
        case BREAK:    return "BREAK";
        case CONTINUE: return "CONTINUE";
        case PRINTF:   return "PRINTF";
        case SCANF:    return "SCANF";
        case IDENTIFIER: return "ID";
        case CONST:    return "NUMBER";
        case COMMA:    return "COMMA";
        case SEMI:     return "SEMICOLON";
        case S_QUO:    return "S_QUO";
        case D_QUO:    return "D_QUO";
        case ASSIGN_OP: return "ASSIGN";
        case '(':      return "LP";
        case ')':      return "RP";
        case '[':      return "LBRACKET";
        case ']':      return "RBRACKET";
        case '{':      return "LBRACE";
        case '}':      return "RBRACE";
        case ADD:      return "ADD";
        case SUB:      return "SUB";
        case MUL:      return "MUL";
        case DIV:      return "DIV";
        case MOD:      return "MOD";
        case POW:      return "POW";
        case EQ_OP:    return "EQUAL";
        case NE_OP:    return "NOT_EQUAL";
        case GT_OP:    return "GREATER";
        case LT_OP:    return "LESS";
        case GE_OP:    return "GREATER_EQUAL";
        case LE_OP:    return "LESS_EQUAL";
        case AND:      return "AND";
        case OR:       return "OR";
        case NOT:      return "NOT";
        case SINGLAND: return "REF";
        default:       return "?";
    }
}

/* 输出一行：单词  词素  属性（常数为数值，标识符为符号表地址） */
static void lex_emit(int tok, const char* lexeme) {
    const char* name = lex_token_name(tok, lexeme);
    char attr[64] = "";
    if (tok == CONST)
        snprintf(attr, sizeof(attr), "%d", (int)strtol(lexeme, NULL, 10));
    else if (tok == IDENTIFIER && strcmp(lexeme, "main") != 0)
        snprintf(attr, sizeof(attr), "%p", (void*)yylval.str);
    printf("%-10s %-8s %s\n", name, lexeme, attr);
}

/* 词法模式驱动：yylex 后补打一行 token（编译模式行为不变） */
int yylex_dump(void) {
    int tok = yylex();
    if (lex_only_mode && tok) lex_emit(tok, yytext);
    return tok;
}
