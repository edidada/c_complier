#include "semantic.h"

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

/*
 * ============ 类型系统（03_详细设计 5.1） ============
 * Type ::= int | int* | int[] | void | function(ret, args...)
 * ARRAY 的大小信息由 AST 提供；此处仅做类型匹配与规则检查。
 */
enum TypeKind { TK_INT, TK_PTR, TK_ARRAY, TK_VOID, TK_FUNC, TK_ERR };

struct TypeInfo {
    TypeKind kind;
    int arraySize;  // ARRAY 元素个数（无大小为 0）
    TypeInfo() : kind(TK_ERR), arraySize(0) {}
    TypeInfo(TypeKind k, int sz = 0) : kind(k), arraySize(sz) {}
    std::string name() const {
        switch (kind) {
            case TK_INT:   return "int";
            case TK_PTR:   return "int*";
            case TK_ARRAY: return "int[]";
            case TK_VOID:  return "void";
            case TK_FUNC:  return "function";
            default:       return "unknown";
        }
    }
    bool isInt()   const { return kind == TK_INT; }
    bool isPtr()   const { return kind == TK_PTR; }
    bool isArray() const { return kind == TK_ARRAY; }
    bool isVoid()  const { return kind == TK_VOID; }
};

/* 变量符号 */
struct VarSym {
    std::string name;
    TypeInfo type;
    int line;
};

/* 函数符号（签名）：用于实参个数/类型与返回类型检查 */
struct FuncSym {
    std::string name;
    TypeInfo ret;
    std::vector<TypeInfo> params;
    int line;
};

/* ============ 全局状态 ============ */
static std::vector<std::map<std::string, VarSym>*> g_scope;  // 作用域栈（尾=当前层）
static std::map<std::string, FuncSym> g_funcs;               // 函数签名（声明+定义）
static std::vector<FuncSym*> g_funcStack;                    // 当前函数栈（return 检查）
static std::vector<std::string> g_errors;                    // 语义错误列表

/* ============ 工具 ============ */
static void semErr(int line, const std::string& msg) {
    char buf[16];
    if (line > 0) snprintf(buf, sizeof(buf), "%d", line);
    else          snprintf(buf, sizeof(buf), "未知");
    g_errors.push_back(std::string("第 ") + buf + " 行: " + msg);
}

/* 递归找节点行号（节点本身未填则取第一个有行号的子孙） */
static int findLine(AbstractAstNode* n) {
    if (n == NULL) return 0;
    if (n->line > 0) return n->line;
    for (AbstractAstNode* c = n->getFirstChild(); c != NULL; c = c->getNextSibling()) {
        int l = findLine(c);
        if (l > 0) return l;
    }
    return 0;
}

/* 取最左 ID 节点（ID_Exp → Single_ID → ID） */
static AbstractAstNode* getFirstName(AbstractAstNode* n) {
    while (n != NULL && n->nodeType != AstNodeType::ID) {
        AbstractAstNode* c = n->getFirstChild();
        if (c == NULL) return NULL;
        n = c;
    }
    return n;
}

/* 常量 0 判断（常量除零检测） */
static bool isConstZero(AbstractAstNode* n) {
    if (n == NULL) return false;
    if (n->content == "Const_Exp") {
        AbstractAstNode* c = n->getFirstChild();
        return c != NULL && c->nodeType == AstNodeType::CONST_INT && c->content == "0";
    }
    return false;
}

static bool compatible(const TypeInfo& a, const TypeInfo& b) {
    if (a.kind == TK_ERR || b.kind == TK_ERR) return true;  // 已报过错，不重复
    if (a.kind == TK_INT && b.kind == TK_INT) return true;
    if (a.kind == TK_PTR && b.kind == TK_PTR) return true;
    if (a.kind == TK_PTR && b.kind == TK_ARRAY) return true; // 数组名退化为指针
    if (a.kind == TK_ARRAY && b.kind == TK_ARRAY) return true;
    if (a.kind == TK_VOID && b.kind == TK_VOID) return true;
    return false;
}

/* ============ 作用域 ============ */
static void pushScope() { g_scope.push_back(new std::map<std::string, VarSym>()); }
static void popScope() {
    if (!g_scope.empty()) { delete g_scope.back(); g_scope.pop_back(); }
}
static void insertVar(const VarSym& s) {
    if (!g_scope.empty()) (*g_scope.back())[s.name] = s;
}
static VarSym* lookupLocal(const std::string& name) {
    if (g_scope.empty()) return NULL;
    std::map<std::string, VarSym>& m = *g_scope.back();
    std::map<std::string, VarSym>::iterator it = m.find(name);
    return (it == m.end()) ? NULL : &it->second;
}
static VarSym* lookup(const std::string& name) {
    for (int i = (int)g_scope.size() - 1; i >= 0; --i) {
        std::map<std::string, VarSym>& m = *g_scope[i];
        std::map<std::string, VarSym>::iterator it = m.find(name);
        if (it != m.end()) return &it->second;
    }
    return NULL;
}

/* ============ 前向声明 ============ */
static TypeInfo checkExp(AbstractAstNode* n);
static void checkStmt(AbstractAstNode* n);
static void checkDefList(AbstractAstNode* list, const TypeInfo& base);

/* ============ 类型推导辅助 ============ */
static TypeInfo typeFromDescriptor(AbstractAstNode* desc) {
    if (desc == NULL) return TypeInfo(TK_INT);
    if (desc->content == "VOID_TYPE")  return TypeInfo(TK_VOID);
    if (desc->content == "INT*_TYPE")  return TypeInfo(TK_PTR);
    return TypeInfo(TK_INT);
}

/* 形参类型（Param 节点） */
static TypeInfo typeFromParam(AbstractAstNode* param) {
    if (param == NULL) return TypeInfo();
    AbstractAstNode* desc = param->getFirstChild();
    TypeInfo base = typeFromDescriptor(desc);
    const std::string& c = param->content;
    if (c == "Param_ID")      return base;                       // int a / int* a
    if (c == "Param_ID[]" ||
        c == "Param_ID[const]") return TypeInfo(TK_ARRAY);       // int a[] / int a[N]
    if (c == "array_*id")     return TypeInfo(TK_PTR);           // int *a
    if (c == "array_&id")     return TypeInfo(TK_PTR);           // int &a
    if (c == "Param_NID")     return base;                       // 无名参数
    return TypeInfo();
}

/* 形参名（Param 节点的 ID，无名为空） */
static std::string nameFromParam(AbstractAstNode* param) {
    if (param == NULL) return "";
    AbstractAstNode* c = param->getFirstChild();
    if (c == NULL) return "";
    AbstractAstNode* id = c->getNextSibling();  // [Descriptor, ID, (const)]
    if (id == NULL) return "";
    return (id->nodeType == AstNodeType::ID) ? id->content : "";
}

/* 收集形参类型（VarList 节点） */
static void collectParams(AbstractAstNode* vl, std::vector<TypeInfo>& params, bool insertSym) {
    if (vl == NULL) return;
    if (vl->content == "Single_Param") {
        AbstractAstNode* p = vl->getFirstChild();
        params.push_back(typeFromParam(p));
        if (insertSym) {
            VarSym s;
            s.name = nameFromParam(p);
            s.type = typeFromParam(p);
            s.line = findLine(p);
            insertVar(s);
        }
    } else if (vl->content == "Some_Param") {
        AbstractAstNode* sub = vl->getFirstChild();     // VarList
        collectParams(sub, params, insertSym);
        AbstractAstNode* last = sub ? sub->getNextSibling() : NULL;  // Param
        if (last) {
            params.push_back(typeFromParam(last));
            if (insertSym) {
                VarSym s;
                s.name = nameFromParam(last);
                s.type = typeFromParam(last);
                s.line = findLine(last);
                insertVar(s);
            }
        }
    }
}

/* ============ 声明检查 ============ */
static TypeInfo checkVardef(AbstractAstNode* vd, const TypeInfo& base) {
    if (vd == NULL) return TypeInfo();
    const std::string& c = vd->content;
    TypeInfo t = base;
    AbstractAstNode* id = NULL;
    if (c == "Block_Single_Vardef") {
        id = vd->getFirstChild();
    } else if (c == "array_id[const]") {
        id = vd->getFirstChild();
        t = TypeInfo(TK_ARRAY);
        AbstractAstNode* cn = id ? id->getNextSibling() : NULL;
        if (cn && cn->nodeType == AstNodeType::CONST_INT)
            t.arraySize = atoi(cn->content.c_str());
    } else if (c == "array_id[exp]" || c == "array_id[]") {
        id = vd->getFirstChild();
        t = TypeInfo(TK_ARRAY);
    } else if (c == "array_*id") {
        id = vd->getFirstChild();
        t = TypeInfo(TK_PTR);
    }
    if (id == NULL) return TypeInfo();
    std::string name = id->content;

    /* R2 禁止重复声明（同作用域） */
    if (lookupLocal(name) != NULL) {
        semErr(findLine(id), "变量 '" + name + "' 重复定义（同作用域）");
        return t;
    }
    VarSym s;
    s.name = name;
    s.type = t;
    s.line = findLine(id);
    insertVar(s);
    return t;
}

static void checkVar(AbstractAstNode* varNode, const TypeInfo& base) {
    if (varNode == NULL) return;
    AbstractAstNode* vd = varNode->getFirstChild();
    TypeInfo t = checkVardef(vd, base);
    if (varNode->content == "Var_ASSIGN") {
        AbstractAstNode* exp = vd ? vd->getNextSibling() : NULL;
        TypeInfo et = checkExp(exp);
        if (t.kind != TK_ERR && et.kind != TK_ERR && !compatible(t, et)) {
            semErr(findLine(exp), "初始化表达式类型不兼容（" + t.name() + " ← " + et.name() + "）");
        }
    }
}

static void checkDefList(AbstractAstNode* list, const TypeInfo& base) {
    if (list == NULL) return;
    AbstractAstNode* v = list->getFirstChild();
    if (v != NULL) checkVar(v, base);
    if (list->content == "Def_Some_Var") {
        AbstractAstNode* rest = v ? v->getNextSibling() : NULL;
        checkDefList(rest, base);
    }
}

static void checkDef(AbstractAstNode* defNode) {
    if (defNode == NULL || defNode->content != "Def_Var") return;
    AbstractAstNode* desc = defNode->getFirstChild();
    AbstractAstNode* list = desc ? desc->getNextSibling() : NULL;
    TypeInfo base = typeFromDescriptor(desc);
    if (base.isVoid()) {
        /* void 变量声明：宽松放行（不检查），后续由中间代码阶段兜底 */
    }
    checkDefList(list, base);
}

/* ============ 表达式检查（R1/R3/R4/R5/R6/R7/R8） ============ */
static TypeInfo checkExp(AbstractAstNode* n) {
    if (n == NULL) return TypeInfo();
    const std::string& c = n->content;

    if (n->nodeType == AstNodeType::EXPRESSION) {
        if (c == "Const_Exp") return TypeInfo(TK_INT);
        if (c == "ID_Exp") {
            AbstractAstNode* id = getFirstName(n);
            if (id == NULL) return TypeInfo();
            VarSym* s = lookup(id->content);
            if (s == NULL) {
                semErr(findLine(id), "变量 '" + id->content + "' 未声明");  // R1
                return TypeInfo();
            }
            return s->type;
        }
        return TypeInfo();
    }

    if (n->nodeType == AstNodeType::ARRAY) {
        if (c == "{consts}") return TypeInfo(TK_ARRAY);  // 数组初始化列表
        return TypeInfo();
    }

    if (n->nodeType == AstNodeType::CALL) {
        AbstractAstNode* id = n->getFirstChild();
        if (id == NULL) return TypeInfo();
        std::string fname = id->content;
        std::map<std::string, FuncSym>::iterator it = g_funcs.find(fname);
        if (it == g_funcs.end()) {
            semErr(findLine(id), "函数 '" + fname + "' 未定义或未声明");  // R8
            return TypeInfo();
        }
        const FuncSym& fs = it->second;
        /* 收集实参 */
        std::vector<TypeInfo> args;
        if (c == "Call_Args_Func") {
            AbstractAstNode* an = id->getNextSibling();
            while (an != NULL) {
                if (an->content == "Func_Single_Arg") {
                    args.push_back(checkExp(an->getFirstChild()));
                } else if (an->content == "Func_Some_Args") {
                    AbstractAstNode* sub = an->getFirstChild();
                    AbstractAstNode* tail = sub ? sub->getNextSibling() : NULL;
                    /* 递归收集子 Args 链 */
                    std::vector<AbstractAstNode*> chain;
                    AbstractAstNode* cur = an;
                    while (cur != NULL && cur->content == "Func_Some_Args") {
                        chain.push_back(cur);
                        cur = cur->getFirstChild();
                    }
                    if (cur != NULL && cur->content == "Func_Single_Arg") {
                        /* 从里到外翻转后依次取 */
                        for (int i = (int)chain.size() - 1; i >= 0; --i) {
                            AbstractAstNode* cc = chain[i]->getFirstChild();
                            AbstractAstNode* tailNode = cc ? cc->getNextSibling() : NULL;
                            if (tailNode) args.push_back(checkExp(tailNode));
                        }
                        args.push_back(checkExp(cur->getFirstChild()));
                    }
                    (void)sub; (void)tail;
                }
                an = an->getNextSibling();
            }
        }
        /* R8 实参个数与类型 */
        if (args.size() != fs.params.size()) {
            semErr(findLine(n), "函数 '" + fname + "' 实参个数不匹配（期望 "
                   + std::to_string(fs.params.size()) + " 个，实际 "
                   + std::to_string(args.size()) + " 个）");
        } else {
            for (size_t i = 0; i < args.size(); ++i) {
                if (!compatible(fs.params[i], args[i])) {
                    semErr(findLine(n), "函数 '" + fname + "' 第 "
                           + std::to_string(i + 1) + " 个实参类型不兼容（期望 "
                           + fs.params[i].name() + "，实际 " + args[i].name() + "）");
                }
            }
        }
        return fs.ret;
    }

    if (n->nodeType == AstNodeType::OPERATION) {
        AbstractAstNode* a = n->getFirstChild();
        AbstractAstNode* b = a ? a->getNextSibling() : NULL;

        if (c == "Assign") {
            TypeInfo lt = checkExp(a);
            TypeInfo rt = checkExp(b);
            bool lvalErr = false;
            if (a != NULL && a->content == "Const_Exp") {  // 不能给常量赋值
                semErr(findLine(n), "不能给常量赋值");
                lvalErr = true;
            }
            if (a != NULL && a->content == "ID_Exp" && lt.isArray()) {  // C6
                AbstractAstNode* id = getFirstName(a);
                semErr(findLine(n), "不能给数组名赋值（'" + (id ? id->content : "") + "' 是数组）");
                lvalErr = true;
            }
            if (!lvalErr && !compatible(lt, rt)) {
                semErr(findLine(n), "赋值类型不兼容（" + lt.name() + " ← " + rt.name() + "）");  // R3
            }
            return rt;
        }

        if (c == "Addition" || c == "Substraction" || c == "Multiply" ||
            c == "Divide" || c == "Mod" || c == "Power") {
            TypeInfo lt = checkExp(a);
            TypeInfo rt = checkExp(b);
            if (lt.kind == TK_ERR || rt.kind == TK_ERR) return TypeInfo();
            if ((c == "Divide" || c == "Mod") && isConstZero(b)) {  // R5 C10
                semErr(findLine(n), "常量除零");
            }
            bool ptrArith = (c == "Addition" || c == "Substraction") &&
                            ((lt.isPtr() && rt.isInt()) || (lt.isInt() && rt.isPtr()));
            if (!(lt.isInt() && rt.isInt()) && !ptrArith) {
                semErr(findLine(n), "算术运算操作数类型不合法（" + lt.name() + " " + c + " " + rt.name() + "）");  // R4
                return TypeInfo();
            }
            return ptrArith ? (lt.isPtr() ? lt : rt) : TypeInfo(TK_INT);
        }

        if (c == "EQ_OP" || c == "NE_OP" || c == "GT_OP" || c == "LT_OP" ||
            c == "GE_OP" || c == "LE_OP" || c == "AND" || c == "OR") {
            TypeInfo lt = checkExp(a);
            TypeInfo rt = checkExp(b);
            if (lt.kind == TK_ERR || rt.kind == TK_ERR) return TypeInfo();
            bool ok = (lt.isInt() || lt.isPtr()) && (rt.isInt() || rt.isPtr());
            if (!ok) {
                semErr(findLine(n), "比较/逻辑操作数类型不合法（" + lt.name() + " " + c + " " + rt.name() + "）");
                return TypeInfo();
            }
            return TypeInfo(TK_INT);
        }

        if (c == "NOT" || c == "Negative" || c == "Parentheses") {
            return checkExp(a);
        }

        if (c == "id[exp]") {  // R6
            AbstractAstNode* id = a;
            TypeInfo it = checkExp(b);
            (void)it;
            if (id == NULL) return TypeInfo();
            VarSym* s = lookup(id->content);
            if (s == NULL) {
                semErr(findLine(id), "变量 '" + id->content + "' 未声明");
                return TypeInfo();
            }
            if (!(s->type.isArray() || s->type.isPtr())) {  // C5
                semErr(findLine(n), "下标操作数不是数组/指针（'" + id->content + "' 是 " + s->type.name() + "）");
                return TypeInfo();
            }
            return TypeInfo(TK_INT);
        }

        if (c == "&id") {  // R7 取地址
            AbstractAstNode* id = a;
            if (id == NULL) return TypeInfo();
            VarSym* s = lookup(id->content);
            if (s == NULL) {
                semErr(findLine(id), "变量 '" + id->content + "' 未声明");
                return TypeInfo();
            }
            return TypeInfo(TK_PTR);
        }

        if (c == "*id") {  // R7 解引用
            AbstractAstNode* id = a;
            if (id == NULL) return TypeInfo();
            VarSym* s = lookup(id->content);
            if (s == NULL) {
                semErr(findLine(id), "变量 '" + id->content + "' 未声明");
                return TypeInfo();
            }
            if (!(s->type.isPtr() || s->type.isArray())) {
                semErr(findLine(n), "解引用操作数不是指针（'" + id->content + "' 是 " + s->type.name() + "）");
                return TypeInfo();
            }
            return TypeInfo(TK_INT);
        }
    }
    return TypeInfo();
}

/* ============ 语句检查（R9/R10） ============ */
static void checkCond(AbstractAstNode* condNode) {
    TypeInfo t = checkExp(condNode);
    if (t.kind == TK_ERR || t.isInt() || t.isPtr()) return;  // R10 放宽指针
    semErr(findLine(condNode), "条件表达式类型不合法（" + t.name() + "）");
}

static void checkReturn(AbstractAstNode* stmtNode, const TypeInfo& expType) {
    if (g_funcStack.empty()) return;
    TypeInfo ret = g_funcStack.back()->ret;
    if (ret.isVoid()) {
        if (!expType.isVoid() && expType.kind != TK_ERR) {
            semErr(findLine(stmtNode), "void 函数不能返回表达式");
        }
    } else {
        if (expType.isVoid()) {  // C7
            semErr(findLine(stmtNode), "返回值类型不匹配（函数返回 " + ret.name() + "，但无 return 表达式）");
        } else if (expType.kind != TK_ERR && !compatible(ret, expType)) {
            semErr(findLine(stmtNode), "返回值类型不匹配（期望 " + ret.name() + "，实际 " + expType.name() + "）");
        }
    }
}

static void checkStmtList(AbstractAstNode* body);

static void checkFor(AbstractAstNode* n, const std::string& c) {
    std::vector<AbstractAstNode*> ch;
    for (AbstractAstNode* x = n->getFirstChild(); x != NULL; x = x->getNextSibling()) ch.push_back(x);
    if (c == "For_SEMI_SEMI") {
        if (ch.size() >= 1) checkStmt(ch[0]);
    } else if (c == "For_Def_SEMI_SEMI") {
        if (ch.size() >= 2) { checkStmt(ch[0]); checkStmt(ch[1]); }
    } else if (c == "For_SEMI_Exp_SEMI") {
        if (ch.size() >= 2) { checkCond(ch[0]); checkStmt(ch[1]); }
    } else if (c == "For_SEMI_SEMI_Exp") {
        if (ch.size() >= 2) { checkExp(ch[0]); checkStmt(ch[1]); }
    } else if (c == "For_Def_SEMI_Exp_SEMI_Exp") {
        if (ch.size() >= 4) { checkStmt(ch[0]); checkCond(ch[1]); checkExp(ch[2]); checkStmt(ch[3]); }
    } else if (c == "For_Def_SEMI_Exp_SEMI") {
        if (ch.size() >= 3) { checkStmt(ch[0]); checkCond(ch[1]); checkStmt(ch[2]); }
    } else if (c == "For_Def_SEMI_SEMI_Exp") {
        if (ch.size() >= 3) { checkStmt(ch[0]); checkExp(ch[1]); checkStmt(ch[2]); }
    } else if (c == "For_SEMI_Exp_SEMI_Exp") {
        if (ch.size() >= 3) { checkCond(ch[0]); checkExp(ch[1]); checkStmt(ch[2]); }
    }
}

static void checkStmt(AbstractAstNode* n) {
    if (n == NULL) return;
    const std::string& c = n->content;

    if (c == "Exp_Stmt") {
        checkExp(n->getFirstChild());
    } else if (c == "Def_Stmt") {
        checkDef(n->getFirstChild());
    } else if (c == "Body_Stmt") {
        pushScope();
        checkStmtList(n->getFirstChild());
        popScope();
    } else if (c == "Return_Exp") {
        TypeInfo t = checkExp(n->getFirstChild());
        checkReturn(n, t);
    } else if (c == "Return_Void") {
        checkReturn(n, TypeInfo(TK_VOID));
    } else if (c == "If" || c == "If_Else") {
        AbstractAstNode* cond = n->getFirstChild();
        AbstractAstNode* s1 = cond ? cond->getNextSibling() : NULL;
        AbstractAstNode* s2 = s1 ? s1->getNextSibling() : NULL;
        checkCond(cond);
        checkStmt(s1);
        if (s2 != NULL) checkStmt(s2);
    } else if (c == "While") {
        AbstractAstNode* cond = n->getFirstChild();
        AbstractAstNode* s1 = cond ? cond->getNextSibling() : NULL;
        checkCond(cond);
        checkStmt(s1);
    } else if (c.find("For_") == 0) {
        checkFor(n, c);
    } else if (c == "Print_Exp" || c == "Print_String") {
        checkExp(n->getFirstChild());
    } else if (c == "scanf_id") {
        AbstractAstNode* id = n->getFirstChild();
        if (id != NULL && lookup(id->content) == NULL) {
            semErr(findLine(id), "变量 '" + id->content + "' 未声明");
        }
    }
    /* Break/Continue/Recover 节点：无需检查 */
}

static void checkStmtList(AbstractAstNode* body) {
    AbstractAstNode* sl = body;
    if (body != NULL && body->content == "Body") sl = body->getFirstChild();
    /* 递归展开 Some_Stmt 链 */
    AbstractAstNode* node = sl;
    if (node != NULL && node->content == "Some_Stmt") {
        AbstractAstNode* inner = node->getFirstChild();
        if (inner != NULL) {
            checkStmtList(inner);
            for (AbstractAstNode* s = inner->getNextSibling(); s != NULL; s = s->getNextSibling())
                checkStmt(s);
        }
        return;
    }
    checkStmt(node);
}

/* ============ 函数与全局块 ============ */
static void checkFunction(AbstractAstNode* desc, AbstractAstNode* func, AbstractAstNode* body) {
    TypeInfo ret = typeFromDescriptor(desc);
    std::string fname;
    std::vector<TypeInfo> params;
    AbstractAstNode* id = func ? func->getFirstChild() : NULL;
    if (func != NULL && func->content == "Func_NParam") {
        fname = id ? id->content : "";
    } else if (func != NULL && func->content == "Func_Params") {
        fname = id ? id->content : "";
        AbstractAstNode* vl = id ? id->getNextSibling() : NULL;
        collectParams(vl, params, false);
    }
    /* 记录/更新签名（定义覆盖声明） */
    FuncSym fs;
    fs.name = fname;
    fs.ret = ret;
    fs.params = params;
    fs.line = findLine(func);
    g_funcs[fname] = fs;

    /* 函数作用域：形参入符号表 + 检查函数体 */
    pushScope();
    if (func != NULL && func->content == "Func_Params" && id != NULL) {
        AbstractAstNode* vl = id->getNextSibling();
        collectParams(vl, params, true);
    }
    g_funcStack.push_back(&g_funcs[fname]);
    checkStmtList(body);  // body 为 Body 节点
    g_funcStack.pop_back();
    popScope();
}

static void checkFunctionDecl(AbstractAstNode* desc, AbstractAstNode* func) {
    TypeInfo ret = typeFromDescriptor(desc);
    std::string fname;
    std::vector<TypeInfo> params;
    AbstractAstNode* id = func ? func->getFirstChild() : NULL;
    if (func != NULL && func->content == "Func_NParam") {
        fname = id ? id->content : "";
    } else if (func != NULL && func->content == "Func_Params") {
        fname = id ? id->content : "";
        AbstractAstNode* vl = id ? id->getNextSibling() : NULL;
        collectParams(vl, params, false);
    }
    FuncSym fs;
    fs.name = fname;
    fs.ret = ret;
    fs.params = params;
    fs.line = findLine(func);
    g_funcs[fname] = fs;
}

static void checkBlock(AbstractAstNode* blockNode);

/* BlockList 展开（Single_Block / Some_Block 包装） */
static void checkBlockList(AbstractAstNode* bl) {
    if (bl == NULL) return;
    if (bl->content == "Single_Block") {
        checkBlock(bl->getFirstChild());
    } else if (bl->content == "Some_Block") {
        AbstractAstNode* c = bl->getFirstChild();            // BlockList
        checkBlockList(c);
        AbstractAstNode* b = c ? c->getNextSibling() : NULL; // Block
        if (b) checkBlock(b);
    } else {
        checkBlock(bl);
    }
}

static void checkBlock(AbstractAstNode* blockNode) {
    if (blockNode == NULL) return;
    const std::string& c = blockNode->content;
    if (c == "Def_Var_Block") {
        AbstractAstNode* desc = blockNode->getFirstChild();
        AbstractAstNode* list = desc ? desc->getNextSibling() : NULL;
        TypeInfo base = typeFromDescriptor(desc);
        checkDefList(list, base);
    } else if (c == "Def_Func_Body_Block") {
        AbstractAstNode* desc = blockNode->getFirstChild();
        AbstractAstNode* func = desc ? desc->getNextSibling() : NULL;
        AbstractAstNode* body = func ? func->getNextSibling() : NULL;
        checkFunction(desc, func, body);
    } else if (c == "Def_Func_NBody_Block") {
        AbstractAstNode* desc = blockNode->getFirstChild();
        AbstractAstNode* func = desc ? desc->getNextSibling() : NULL;
        checkFunctionDecl(desc, func);
    }
    /* error SEMI 恢复节点：跳过 */
}

/* ============ 入口 ============ */
int typeCheck(AbstractAstNode* root) {
    g_errors.clear();
    g_funcs.clear();
    g_scope.clear();
    g_funcStack.clear();

    pushScope();  // 全局作用域

    /* 预置系统函数签名 */
    FuncSym pf;
    pf.name = "print_int";
    pf.ret = TypeInfo(TK_VOID);
    pf.params.push_back(TypeInfo(TK_INT));
    pf.line = 0;
    g_funcs["print_int"] = pf;

    if (root != NULL) {
        checkBlockList(root->getFirstChild());
    }
    popScope();

    if (!g_errors.empty()) {
        for (size_t i = 0; i < g_errors.size(); ++i) {
            fprintf(stderr, "[语义错误] %s\n", g_errors[i].c_str());
        }
    }
    return (int)g_errors.size();
}
