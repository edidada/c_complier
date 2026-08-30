#include "interCode.h"
#include <typeinfo>
#include <sstream>
#include <string>
#include <iostream>
#include<list>
#include <set>
/* 阶段8：递归收集多维数组声明 a[2][3] 的维度与底层名字（嵌套 AST） */
static void collectDims(AbstractAstNode* node, std::vector<int>& dims, std::string& name) {
    if (node == NULL) return;
    AbstractAstNode* first = node->getFirstChild();
    if (first != NULL && first->content == "array_id[const]") {
        collectDims(first, dims, name);   /* 内层维度先 */
    } else {
        name = first ? first->content : "";
    }
    AbstractAstNode* cn = first ? first->getNextSibling() : NULL;
    if (cn != NULL) dims.push_back(atoi(cn->content.c_str()));
}
std::vector<QuadItem*> reduceUnusedSymbol(std::vector<QuadItem* > quad_list){
    int len = quad_list.size();
    int i=0;
    while(i < len){
        QuadItem* quad = quad_list[i];
        if(quad->getOpType() == OpType::assign){
            Symbol* re = quad->result.var;
            if(re->getIsUsed() == false){
                std::cout<<re->getIDName()<<" is not used!"<<std::endl;
                quad_list.erase(quad_list.begin()+i);
                continue;
            }
        }
        i++;
    }
    return quad_list;
}
char isnumber(char ch){
    if(ch>='0' && ch<= '9'){
        return true;
    }return false;
}
bool isNumber(std::string str){
    int len = str.length();
    for(int i=0; i<len; i++){
        if(! isnumber(str[i])) return false; 
    }
    return true;
}
QuadItem::QuadItem(int result, OpType op, int arg1, int arg2)
{
    this->op = op;
    this->arg1.target = arg1;
    this->arg2.target = arg2;
    this->result.target = result;
    this->quad_item_type = 0;
}

QuadItem::QuadItem(int result, OpType op, Symbol* arg1, int arg2)
{
    this->op = op;
    this->arg1.var = arg1;
    this->arg2.target = arg2;
    this->result.target = result;
    this->quad_item_type = 1;
}

QuadItem::QuadItem(int result, OpType op, int arg1, Symbol* arg2)
{
    this->op = op;
    this->arg1.target = arg1;
    this->arg2.var = arg2;
    this->result.target = result;
    this->quad_item_type = 2;
}

QuadItem::QuadItem(int result,OpType op, Symbol* arg1, Symbol* arg2)
{
    this->op = op;
    this->arg1.var = arg1;
    this->arg2.var = arg2;
    this->result.target = result;
    this->quad_item_type = 3;
}

QuadItem:: QuadItem(Symbol* result, OpType op, int arg1, int arg2){
   this->result.var = result;
   this->op = op;
   this->arg1.target = arg1;
   this->arg2.target = arg2;
   this->quad_item_type = 4;
}

// 常量和变量运算后赋值给变量-1；
QuadItem:: QuadItem(Symbol* result, OpType op, Symbol* arg1, int arg2){
   this->result.var = result;
   this->op = op;
   this->arg1.var = arg1;
   this->arg2.target = arg2;
   this->quad_item_type = 5;
}

// 常量和变量运算后赋值给变量-2；
QuadItem:: QuadItem(Symbol* result, OpType op, int arg1, Symbol* arg2){
   this->result.var = result;
   this->op = op;
   this->arg1.target = arg1;
   this->arg2.var = arg2;
   this->quad_item_type = 6;
}

// 变量和变量运算后赋值给变量
QuadItem:: QuadItem(Symbol* result, OpType op, Symbol* arg1, Symbol* arg2){
   this->result.var = result;
   this->op = op;
   this->arg1.var = arg1;
   this->arg2.var = arg2;
   this->quad_item_type = 7;
}

/***
 * 
 * 
 * ***/
QuadItem:: QuadItem(Symbol* result, OpType op, Symbol* arg1){
    this->result.var = result;
    this->op = op;
    this->arg1.var = arg1;
    this->arg2.var = NULL;   /* 阶段8修复：一元四元式 arg2 必须初始化，否则 optimize 的 collectUse 读未初始化内存 */
    this->quad_item_type = 7;
}
QuadItem:: QuadItem(Symbol* result, OpType op, int arg1){
    this->result.var = result;
    this->op = op;
    this->arg1.target = arg1;
    this->arg2.var = NULL;   /* 阶段8修复：union 必须按 64 位清零，否则高 32 位垃圾致野指针 */
    this->quad_item_type = 6;
}
QuadItem::QuadItem(int result,OpType op)
{
    this->op = op;
    this->arg1.var = NULL;
    this->arg2.var = NULL;
    this->result.target = result;
    this->quad_item_type = 3;
}

// for print_int
QuadItem:: QuadItem(Symbol* result, OpType op){
    this->op = op;
    this->arg1.var = NULL;
    this->arg2.var = NULL;
    this->result.var = result;
    this->quad_item_type = 10;
}

/**
 * 
 * 打印出一个四元式；根据op、arg1/2以及result的类型类确定输出格式；
 * 
*/
void QuadItem:: printItemInfor(int i)
{
    OpType op_type = this->op;
    int type = this->quad_item_type;
    // std::cout<<"Quad-Type: "<<type<<"  ";
    // std::cout<<"Op_Type: "<<static_cast<int>(op_type)<<std::endl;
    switch (op)
    {
    case addtion:
        if(type == 7){
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" ADD "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 6)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" ADD "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 5)
        {
             std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" ADD "
            <<this->arg2.target
            <<std::endl;
        }
        else if(type == 4){
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" ADD "
            <<this->arg2.target
            <<std::endl;
        }
        break;
    case substract:
        if(type == 7)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" SUB "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 6)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" SUB "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 5)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" SUB "
            <<this->arg2.target
            <<std::endl;
        }
        else if(type == 4)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" SUB "
            <<this->arg2.target
            <<std::endl;
        }
        break;
    case multiply:
        if(type == 7)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" MUL "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 6)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" MUL "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 5)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" MUL "
            <<this->arg2.target
            <<std::endl;
        }
        else if(type == 4)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" MUL "
            <<this->arg2.target
            <<std::endl;
        }
        break;
    case divide:
        if(type == 7)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" DIV "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 6)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" DIV "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 5)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" DIV "
            <<this->arg2.target
            <<std::endl;
        }
        else if(type == 4)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" DIV "
            <<this->arg2.target
            <<std::endl;
        }
        break;
    case mod:
        if(type == 7)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" MOD "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 6)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" MOD "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 5)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" MOD "
            <<this->arg2.target
            <<std::endl;
        }
        else if(type == 4)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" MOD "
            <<this->arg2.target
            <<std::endl;
        }
        break;
    case power:
        if(type == 7)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" POW "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 6)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" POW "
            <<this->arg2.var->getIDName()
            <<std::endl;
        }
        else if(type == 5)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.var->getIDName()
            <<" POW "
            <<this->arg2.target
            <<std::endl;
        }
        else if(type == 4)
        {
            std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<" POW "
            <<this->arg2.target
            <<std::endl;
        }
        break;
    case assign:{
        if (type == 6)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName()
            <<" := "
            <<this->arg1.target
            <<std::endl;
        }
        else if(type == 7)
        {
           std::cout<<"L"<<i<<":  "
            <<this->result.var->getIDName() <<" := "
            <<this->arg1.var->getIDName()<<std::endl;
            // std::cout<<result.var->getIDName()<<" offset: "<<result.var->getSymOffset()<<std::endl;
            // std::cout<<arg1.var->getIDName()<<" offset: "<<arg1.var->getSymOffset()<<std::endl;
        }
    }
    break;
    case JUMP_LT:
        if(type == 3)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" < "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 2)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" < "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 1)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" < "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 0)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" < "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        
    break;
    case JUMP_LE:
         if(type == 3)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" <= "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 2)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" <= "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 1)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" <= "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 0)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" <= "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        
    break;
    case JUMP_GT:
         if(type == 3)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" > "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 2)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" > "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 1)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" > "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 0)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" > "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        
    break;
    case JUMP_GE:
        if(type == 3)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" >= "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 2)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" >= "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 1)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" >="
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 0)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" >= "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        
    break;
    case JUMP_EQ:
         if(type == 3)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" == "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 2)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" == "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 1)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" == "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 0)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" == "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        
    break;
    case JUMP_NE:
        if(type == 3)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" != "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 2)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" != "
            <<this->arg2.var->getIDName()
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 1)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.var->getIDName()
            <<" != "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        else if(type == 0)
        {
           std::cout<<"L"<<i<<":  "<<"if "
            <<this->arg1.target
            <<" != "
            <<this->arg2.target
            <<" goto "
            <<"L"
            <<this->result.target
            <<std::endl;
        }
        
    break;
    case JUMP:
       std::cout<<"L"<<i<<":  "
        <<" goto "
        <<"L"
        <<this->result.target
        <<std::endl;
    break;
    case PRINT:
        std::cout<<"L"<<i<<":  "
        <<"print_int "
        <<this->result.var->getIDName()<<std::endl;
    break;
    case RETURN_OP:
        if(type == 10){
            std::cout<<"L"<<i<<":  "
            <<"RETURN "
            <<this->result.var->getIDName()<<std::endl;
        }else{
            std::cout<<"L"<<i<<":  "
            <<"RETURN"<<std::endl;
        }
    break;
    case SCAN:
        std::cout<<"L"<<i<<":  "
        <<"SCAN "
        <<this->result.var->getIDName()<<std::endl;
    break;
    case PARAM:
        std::cout<<"L"<<i<<":  "
        <<"PARAM "
        <<this->result.var->getIDName()<<std::endl;
    break;
    case CALL:
        std::cout<<"L"<<i<<":  "
        <<"CALL "
        <<this->arg1.var->getIDName()
        <<", "
        <<this->result.target<<std::endl;
    break;
    case FUNC_LABEL:
        std::cout<<"L"<<i<<":  "
        <<"FUNC_LABEL "
        <<this->arg1.var->getIDName()
        <<", "
        <<this->result.target<<std::endl;
    break;
    case FUNC_END:
        std::cout<<"L"<<i<<":  "
        <<"FUNC_END "
        <<this->result.var->getIDName()<<std::endl;
    break;
    
    default:
    //    std::cout<<"\033[31m Error! No such quad! \033[0m"<<std::endl;
       
    break;
    }
}


InterCode:: InterCode(AbstractAstNode* root){
    this->root = root;
    this->rootTable = new SymbolTable(false);
}

void InterCode:: Root_Generate(){
    std::cout<<"Gen "<<root->content<<std::endl;
    Generate(this->root, this->rootTable);
    // 阶段6：四元式级优化（常量折叠/死代码消除/不可达删除）
    this->optimize();
    int len = this->quad_list.size();
    int i=0;
    while(i < len){
        quad_list[i]->printItemInfor(i);
        i++;
    }
}

void InterCode:: Generate(AbstractAstNode* node, SymbolTable* symbol_table) {
    if (node == NULL ){
        std::cout<<"Warning: Generate received null node, skipped."<<std::endl;
        return;
    }
    std::string node_content = node->content;
    int type = static_cast<int>(node->nodeType);
    switch (type){
        case static_cast<int>(AstNodeType::ROOT):
                {
                    AbstractAstNode* child = node->getFirstChild();
                    while(child != NULL){
                        std::cout<<"Gen "<<child->content<<std::endl;
                        Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
                }
        break;
        case static_cast<int>(AstNodeType::STATEMENT ):
        {
            if(node_content == "Single_Block" || "Some_Block")
            {
                AbstractAstNode* child = node->getFirstChild();
                    while(child != NULL){
                        std::cout<<"Gen "<<child->content<<std::endl;
                        Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
            }
        }
        break;
        case static_cast<int>(AstNodeType::DEFINITION ):{
            if (node_content == "Def_Func_Body_Block"){
                AbstractAstNode* child = node->getFirstChild();
                    while(child != NULL){
                        std::cout<<"Gen "<<child->content<<std::endl;
                        Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
            }else if(node_content == "Func_NParam"){
                AbstractAstNode* child = node->getFirstChild();
                    while(child != NULL){
                        std::cout<<"Gen "<<child->content<<std::endl;
                        Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
            }else if(node_content == "Def_Var_Block"){
                AbstractAstNode* child = node->getFirstChild();
                bool is_modify_int = false;
                if(child->content == "INT_TYPE") is_modify_int = true;
                while(child != NULL){
                    std::cout<<"Gen "<<child->content<<std::endl;
                    Body_Generate(child, symbol_table);
                    child = child->getNextSibling();
                }


            }
        }
        break;
        case static_cast<int>(AstNodeType::MODIFY):
                {
                    AbstractAstNode* child = node->getFirstChild();
                    while(child != NULL){
                        std::cout<<"Gen "<<child->content<<std::endl;
                        Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
                }
        break;
        case static_cast<int>(AstNodeType::ID):
                {
                    AbstractAstNode* child = node->getFirstChild();
                    // child->printNodeInfo();
                    while(child != NULL){
                        std::cout<<"Gen "<<child->content<<std::endl;
                        Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
                }
        break;
        case static_cast<int>(AstNodeType::BODY):
                {
                    // 每个函数统一生成 FUNC_LABEL + 函数体 + FUNC_END（阶段5）
                    AbstractAstNode* funcNode = node->getParent();
                    std::string fname = funcNode->getFirstChild()->getNextSibling()->getFirstChild()->content;
                    Symbol* funcSym = new Symbol(fname, SymbolType::var, 0);
                    if(fname == "main"){
                        // main 表挂 rootTable 子链（AsmGenerate 取 firstChildTable）
                        SymbolTable* main_symbol_table = symbol_table->addChildTable(false);
                        // 阶段8修复：局部变量从全局段之后分配 offset，避免与全局变量（ebp-4..）
                        // 重叠；FUNC_LABEL 栈大小 = 最终 total_offset（已含全局基数）
                        main_symbol_table->setOffset(symbol_table->getOffset());
                        QuadItem* begin = new QuadItem(0, OpType::FUNC_LABEL, funcSym, 0);
                        quad_list.push_back(begin);
                        Body_Generate(node, main_symbol_table);
                        begin->result.target = main_symbol_table->getOffset();
                        QuadItem* end = new QuadItem(funcSym, OpType::FUNC_END);
                        quad_list.push_back(end);
                    }
                    else{
                        // 非 main 函数：独立函数体表（不挂 rootTable 子链），形参负偏移 [ebp+8] 起
                        SymbolTable* func_table = new SymbolTable(false);
                        func_table->setParentTable(symbol_table);
                        // 阶段8修复：局部变量从全局段之后分配 offset（形参为硬编码负偏移，不受影响）
                        func_table->setOffset(symbol_table->getOffset());
                        AbstractAstNode* fp = funcNode->getFirstChild()->getNextSibling();
                        if(fp->content == "Func_Params"){
                            AbstractAstNode* vl = fp->getFirstChild()->getNextSibling();
                            int idx = 0;
                            GenParams(vl, func_table, idx);
                        }
                        QuadItem* begin = new QuadItem(0, OpType::FUNC_LABEL, funcSym, 0);
                        quad_list.push_back(begin);
                        Body_Generate(node, func_table);
                        begin->result.target = func_table->getOffset();
                        QuadItem* end = new QuadItem(funcSym, OpType::FUNC_END);
                        quad_list.push_back(end);
                    }
                }
        break;

        default:
            std::cout<<"Warning: Generate unknown AstNodeType: "<<node_content<<", skipped."<<std::endl; 
        break;
    }
}

/* 阶段8：数组元素/指针解引用符号作为"取值"操作数时，先取出为临时变量
   （t := a[i][j]），汇编算术/比较/打印/实参/返回分支仅按普通变量读取操作数 */
Symbol* InterCode:: normalizeOperand(Symbol* s){
    if(s == NULL) return s;
    std::string name = s->getIDName();
    bool needExtract = (name.find('[') != std::string::npos) ||
                       (!name.empty() && name[0]=='*');
    // 注意：&x 是"地址值"而非"取值"，汇编层 PARAM/赋值可直接计算地址，不在此抽取
    if(needExtract){
        Symbol* re = new Symbol("t"+std::to_string(temp_list.size()), SymbolType::temp_var, 4);
        temp_list.push_back(re);
        QuadItem* quad = new QuadItem(re, OpType::assign, s);  // t := a[i][j]（读路径已支持）
        this->quad_list.push_back(quad);
        return re;
    }
    return s;
}

Symbol* InterCode:: Exp_Stmt_Generate(AbstractAstNode* node, SymbolTable* symbol_table){
    if(node == NULL){
        std::cout<<"Warning: Exp_Stmt_Generate received null node, skipped."<<std::endl;
        return nullptr;
    }
    std::string node_content = node->content;
    int type = static_cast<int>(node->nodeType);
    switch(type){
        case static_cast<int>(AstNodeType::DEFINITION):{
            if(node_content == "Single_ID"){                
                std::string ID_name = node->getFirstChild()->content;
                Symbol* re = symbol_table->findSymbolLocally(ID_name);
                if(re == NULL){
                    // 未定义使用在findSymbolGlobally中定义；
                    re = symbol_table->findSymbolGlobally(ID_name);
                }
                return re;
            }
        }
            break;
        case static_cast<int>(AstNodeType::OPERATION):{
            if(node_content == "Parentheses")
            {
                Symbol* re = Exp_Stmt_Generate(node->getFirstChild(), symbol_table);
                re->setIsUsed();
                return re;
            }
            else if(node_content == "Addition" ||
                node_content == "Substraction" ||
                node_content == "Multiply" || 
                node_content == "Divide" || 
                node_content == "Mod" || 
                node_content == "Power")
            {
                        // 注意，先generate完子节点， 再生成result的tempVar；
                Symbol* arg1 = Exp_Stmt_Generate(node->getFirstChild(), symbol_table); arg1->setIsUsed();
                Symbol* arg2 = Exp_Stmt_Generate(node->getFirstChild()->getNextSibling(), symbol_table); arg2->setIsUsed();
                // 阶段8：数组元素/指针解引用作为算术操作数时先取出（汇编算术分支不支持此类符号名）
                arg1 = normalizeOperand(arg1);
                arg2 = normalizeOperand(arg2);
                Symbol* re = new Symbol("t"+std::to_string(temp_list.size()), SymbolType::temp_var, 4);
                temp_list.push_back(re);
                OpType op;
                if (node_content == "Addition"){
                    op = addtion;
                }else if (node_content == "Substraction"){
                    op = substract;
                }else if (node_content == "Multiply"){
                    op = multiply;
                }else if (node_content == "Divide"){
                    op = divide;
                }else if (node_content == "Mod"){
                    op = mod;
                }else if (node_content == "Power"){
                    op = power;
                }
                QuadItem* quad;
                if (isNumber(arg1->getIDName()) && isNumber(arg2->getIDName())){
                    quad = new QuadItem(re, op, atoi(arg1->getIDName().c_str()), atoi(arg2->getIDName().c_str()) );
                }else if(isNumber(arg1->getIDName()) && !isNumber(arg2->getIDName())){
                    quad = new QuadItem(re, op, atoi(arg1->getIDName().c_str()), arg2 );
                }else if(!isNumber(arg1->getIDName()) && isNumber(arg2->getIDName())){
                    quad = new QuadItem(re, op, arg1, atoi(arg2->getIDName().c_str()) );
                }else if(!isNumber(arg1->getIDName()) && !isNumber(arg2->getIDName())){
                    quad = new QuadItem(re, op, arg1, arg2);
                }
                this->quad_list.push_back(quad);
                return re;
            } 
            else if(node_content == "Assign")
            {
                OpType op = assign;
                Symbol* re = Exp_Stmt_Generate(node->getFirstChild(), symbol_table); re->setIsUsed();
                Symbol* arg1 = Exp_Stmt_Generate(node->getFirstChild()->getNextSibling(), symbol_table); arg1->setIsUsed();
                // 进行类型检查；type的值是枚举类型symbolType决定的；
                int re_symbol_type = static_cast<int>(re->getSymbolType());
                int arg1_symbol_type = static_cast<int>(arg1->getSymbolType());
                // 阶段5修复：删除原 setSymOffset 传播（p:=&a 时会把 p 的偏移改写为
                // a 的偏移，导致指针与目标变量共享栈槽、相互覆盖；指针声明已分配独立栈槽）
                if(arg1_symbol_type== 2 && re_symbol_type == 5 || 
                    arg1_symbol_type== 2 && re_symbol_type == 4)
                {
                    std::cout<<"\033[31m warning: incompatible integer to pointer conversion assigning to 'int *' from 'int'; take the address with & \033[0m"<<std::endl;
                }
                else if(arg1_symbol_type== 5 && re_symbol_type == 2 ||
                        arg1_symbol_type== 4 && re_symbol_type == 2)
                {
                    std::cout<<"\033[31m warning: incompatible pointer to integer conversion assigning to 'int' from 'int *'; dereference with *\n\033[0m"<<std::endl;
                }
                QuadItem* quad;
                if(isNumber(arg1->getIDName())){
                    std::cout<<arg1->getIDName()<<" is a number!!"<<std::endl;
                    quad = new QuadItem(re, op, atoi(arg1->getIDName().c_str()));
                }else {
                    quad = new QuadItem(re, op, arg1);
                }
                this->quad_list.push_back(quad);
                // std::cout<<"quad_list Size: "<<quad_list.size()
                // <<"quad type: "<<quad->getItemType()<<std::endl;
                return re;
            } 
            else if(node_content == "EQ_OP" ||
                node_content == "NE_OP" ||
                node_content == "GT_OP" ||
                node_content == "LT_OP" ||
                node_content == "GE_OP" ||
                node_content == "LE_OP" )
            {
                Symbol* arg1 = Exp_Stmt_Generate(node->getFirstChild(), symbol_table);
                Symbol* arg2 = Exp_Stmt_Generate(node->getFirstChild()->getNextSibling(), symbol_table); 
                // 阶段8：数组元素/解引用作为比较操作数先取出
                arg1 = normalizeOperand(arg1);
                arg2 = normalizeOperand(arg2);
                OpType op;
                if (node_content == "EQ_OP"){
                    op = JUMP_EQ;
                }else if (node_content == "NE_OP"){
                    op = JUMP_NE;
                }else if (node_content == "GT_OP"){
                    op = JUMP_GT;
                }else if (node_content == "LT_OP"){
                    op = JUMP_LT;
                }else if (node_content == "GE_OP"){
                    op = JUMP_GE;
                }else if (node_content == "LE_OP"){
                    op = JUMP_LE;
                }
                QuadItem* T;
                if (isNumber(arg1->getIDName())&& isNumber(arg2->getIDName())){
                   T = new QuadItem(int(NULL), op, atoi(arg1->getIDName().c_str()), atoi(arg2->getIDName().c_str()) );
                }else if (!isNumber(arg1->getIDName())&& isNumber(arg2->getIDName())){
                   T = new QuadItem(int(NULL), op, arg1, atoi(arg2->getIDName().c_str()) );
                }else if (isNumber(arg1->getIDName())&& !isNumber(arg2->getIDName())){
                   T = new QuadItem(int(NULL), op, atoi(arg1->getIDName().c_str()), arg2 );
                }else if (!isNumber(arg1->getIDName())&& !isNumber(arg2->getIDName())){
                   T = new QuadItem(int(NULL), op, arg1, arg2);
                }
                
                QuadItem *F = new QuadItem(int(NULL),OpType::JUMP);
                std::list<int> trueL; 
                int len = quad_list.size();
                trueL.push_back(len);
                this->quad_list.push_back(T);
                std::list<int> falseL; 
                len = quad_list.size();
                falseL.push_back(len);
                this->quad_list.push_back(F);
                trueList.push(trueL);
                falseList.push(falseL);

            }
            else if(node_content == "AND" || node_content == "OR" || node_content == "NOT")
            {
                if (node_content == "AND"){
                    AbstractAstNode *child = node->getFirstChild();
                    Exp_Stmt_Generate(child, symbol_table);
                    logic.push(quad_list.size());
                    Exp_Stmt_Generate(child->getNextSibling(), symbol_table);
                    std::list<int> leftTrue, rightTrue, leftFalse, rightFalse;
                    rightTrue = trueList.top();
                    trueList.pop();
                    leftTrue = trueList.top();
                    trueList.pop();
                    rightFalse = falseList.top();
                    falseList.pop();
                    leftFalse = falseList.top();
                    falseList.pop();
                    leftFalse.merge(rightFalse);
                    falseList.push(leftFalse);
                    trueList.push(rightTrue);
                    backpatch(&leftTrue, logic.top());
                    logic.pop();
                    break;
                }else if (node_content == "OR"){
                    AbstractAstNode *child = node->getFirstChild();
                    Exp_Stmt_Generate(child, symbol_table);
                    logic.push(quad_list.size());
                    Exp_Stmt_Generate(child->getNextSibling(), symbol_table);
                    std::list<int> leftTrue, rightTrue, leftFalse, rightFalse;
                    rightTrue = trueList.top();
                    trueList.pop();
                    leftTrue = trueList.top();
                    trueList.pop();
                    rightFalse = falseList.top();
                    falseList.pop();
                    leftFalse = falseList.top();
                    falseList.pop();
                    leftTrue.merge(rightTrue);
                    trueList.push(leftTrue);
                    falseList.push(rightFalse);
                    backpatch(&leftFalse, logic.top());
                    logic.pop();
                    break;                    
                }else if (node_content == "NOT"){
                    AbstractAstNode *child = node->getFirstChild();
                    while (child != NULL){
                        Exp_Stmt_Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
                    std::list<int> trueL, falseL;
                    trueL = trueList.top();
                    trueList.pop();
                    falseL = falseList.top();
                    falseList.pop();
                    trueList.push(falseL);
                    falseList.push(trueL);
                    break;                   
                }
            }
            else if(node_content == "id[exp]")// a[i] 或 a[i][j]
            {   
                // * quads for b = a[i]      b is int && a is array(int)
                // * t1 = i*4
                // * t2 = a[t1]
                // * b = t2
                // * 
                // 阶段8：第一子节点是 Exp（ID_Exp 或嵌套 id[exp]），递归拼接符号名
                std::string array_name;
                AbstractAstNode* first = node->getFirstChild();
                if (first->content == "ID_Exp") {
                    Symbol* base_sym = Exp_Stmt_Generate(first, symbol_table);
                    array_name = base_sym->getIDName();
                } else {
                    Symbol* inner = Exp_Stmt_Generate(first, symbol_table);
                    array_name = inner->getIDName();
                }
                AbstractAstNode* index_exp_astNode = node->getFirstChild()->getNextSibling();
                // index_symbol 在Exp_Stmt_Generate时候已经进入符号表中了；
                Symbol* index_symbol = Exp_Stmt_Generate(index_exp_astNode, symbol_table);
                std::string index_content = index_symbol->getIDName();
                Symbol* re = new Symbol(array_name+"["+index_content+"]", SymbolType::var, 4);
                return re;
            }
            else if(node_content == "&id")
            {
                // &b是类型为pointer的Symbol；它不进入符号表和temp_list,只是作为offset的传递者以及中间代生成的工具symbol；
                // &b的偏移量就是b的偏移量；
                std::string var_name = node->getFirstChild()->content;
                Symbol* var = symbol_table->findSymbolLocally(var_name);
                if(var == NULL){
                    var = symbol_table->findSymbolGlobally(var_name);
                }
                if(var == NULL){
                    std::cout<<"\033[31m Error: Undefined variable in &: \033[0m"<<var_name<<std::endl;
                    return nullptr;
                }
                int offset = var->getSymOffset();
                Symbol* addr_var = new Symbol("&"+var_name, SymbolType::pointer);
                addr_var->setSymOffset(offset);
                // std::cout<<"in &id, "<<addr_var->getIDName()<<" offset is "<<addr_var->getSymOffset();
                return addr_var;
            }
            else if(node_content == "*id")
            {
                std::string pointer_name = node->getFirstChild()->content;
                Symbol* pointer = symbol_table->findSymbolLocally(pointer_name);
                if(pointer == NULL){
                    pointer = symbol_table->findSymbolGlobally(pointer_name);
                }
                if(pointer == NULL){
                    std::cout<<"\033[31m Error: Undefined pointer in *: \033[0m"<<pointer_name<<std::endl;
                    return nullptr;
                }
                int offset = pointer->getSymOffset();
                Symbol* star_pointer = new Symbol("*"+pointer_name, SymbolType::var);
                star_pointer->setSymOffset(offset);
                return star_pointer;
            }
            else if(node_content == "**id")// 阶段8：多重指针解引用 **p
            {
                std::string pointer_name = node->getFirstChild()->content;
                Symbol* pointer = symbol_table->findSymbolLocally(pointer_name);
                if(pointer == NULL){
                    pointer = symbol_table->findSymbolGlobally(pointer_name);
                }
                if(pointer == NULL){
                    std::cout<<"\033[31m Error: Undefined pointer in **: \033[0m"<<pointer_name<<std::endl;
                    return nullptr;
                }
                int offset = pointer->getSymOffset();
                Symbol* star_pointer = new Symbol("**"+pointer_name, SymbolType::var);
                star_pointer->setSymOffset(offset);
                return star_pointer;
            }
        }
            break;
        case static_cast<int>(AstNodeType::CALL):
        {
            if(node_content == "Call_Args_Func"){
                AbstractAstNode* child = node->getFirstChild();
                if(child->content == "print_int"){
                    // 实参可为任意表达式（变量/数组访问/数字），统一求值后打印（阶段5增强）
                    AbstractAstNode* argNode = child->getNextSibling();
                    Symbol* argVal = NULL;
                    if(argNode != NULL && argNode->content == "Func_Single_Arg"){
                        argVal = Exp_Stmt_Generate(argNode->getFirstChild(), symbol_table);
                    }
                    if(argVal == NULL && argNode != NULL){
                        AbstractAstNode* id = argNode->getFirstChild()->getFirstChild()->getFirstChild();
                        if(id != NULL){
                            argVal = symbol_table->findSymbolLocally(id->content);
                            if(argVal == NULL){
                                argVal = symbol_table->findSymbolGlobally(id->content);
                            }
                        }
                    }
                    if(argVal == NULL){
                        std::cout<<"\033[31m Error: Undefined variable in print_int \033[0m"<<std::endl;
                    }
                    else
                    {
                        // 阶段8：print 数组元素/解引用时先取出（汇编打印分支按普通变量读取）
                        argVal = normalizeOperand(argVal);
                        QuadItem* quad = new QuadItem(argVal, OpType::PRINT);
                        this->quad_list.push_back(quad);
                    }
                }
                else{
                    // 通用函数调用：cdecl 实参逆序压栈（PARAM），再生成 CALL f, n（阶段4/5）
                    std::string fname = child->content;
                    Symbol* func_sym = new Symbol(fname, SymbolType::var, 0);
                    std::vector<Symbol*> args;
                    AbstractAstNode* argNode = child->getNextSibling();
                    GenCallArgs(argNode, symbol_table, args);
                    for(int i = (int)args.size() - 1; i >= 0; --i){
                        // 阶段8：实参为数组元素/解引用时先取出（汇编 PARAM 按普通变量读取）
                        args[i] = normalizeOperand(args[i]);
                        QuadItem* p = new QuadItem(args[i], OpType::PARAM);
                        this->quad_list.push_back(p);
                    }
                    QuadItem* call_quad = new QuadItem((int)args.size(), OpType::CALL, func_sym, 0);
                    this->quad_list.push_back(call_quad);
                    // 返回值约定：函数返回值在 eax；CALL 后立即存入新临时变量
                    // （eax 别名仅用于这条紧邻的赋值，避免返回值直接参与后续算术）
                    Symbol* eax_alias = new Symbol("eax", SymbolType::var, 0);
                    Symbol* retTemp = new Symbol("t" + std::to_string(temp_list.size()), SymbolType::temp_var, 4);
                    temp_list.push_back(retTemp);
                    QuadItem* save = new QuadItem(retTemp, OpType::assign, eax_alias);
                    this->quad_list.push_back(save);
                    return retTemp;
                }
            }
        }
            break;
        case static_cast<int>(AstNodeType::EXPRESSION): // EXPRESSION
        {
            if(node_content == "Const_Exp")
            {
                Symbol* re = new Symbol(node->getFirstChild()->content);
                return re;
            }
            else if(node_content == "ID_Exp")
            {
                Symbol* re = Exp_Stmt_Generate(node->getFirstChild(), symbol_table);
                return re;
            }
            else if(node_content == "For_Exp")
            {
                Exp_Stmt_Generate(node->getFirstChild(),symbol_table);
            }
        }
            break;

        default:
            std::cout<<"Warning: Exp_Stmt_Generate unknown AstNodeType: "<<node_content<<", skipped."<<std::endl; 
        break;
    }
    return nullptr;
}

// 递归展开实参链（Func_Single_Arg / Func_Some_Args），按源顺序求值收集到 args
void InterCode:: GenCallArgs(AbstractAstNode* node, SymbolTable* symbol_table, std::vector<Symbol*>& args){
    if(node == NULL) return;
    if(node->content == "Func_Single_Arg"){
        Symbol* param = Exp_Stmt_Generate(node->getFirstChild(), symbol_table);
        if(param != NULL){
            args.push_back(param);
        }
    }
    else if(node->content == "Func_Some_Args"){
        AbstractAstNode* inner = node->getFirstChild();                     // 内层 Args 链
        GenCallArgs(inner, symbol_table, args);                             // 先内层（源顺序）
        AbstractAstNode* tail = inner ? inner->getNextSibling() : NULL;     // 尾部实参
        if(tail != NULL){
            if(tail->content == "Func_Single_Arg" || tail->content == "Func_Some_Args"){
                GenCallArgs(tail, symbol_table, args);
            } else {
                // 尾部实参是裸表达式（文法：Args COMMA Exp），直接求值入参（阶段5修复多实参丢失）
                Symbol* param = Exp_Stmt_Generate(tail, symbol_table);
                if(param != NULL){
                    args.push_back(param);
                }
            }
        }
    }
}

// 递归展开形参链（Single_Param / Some_Param），按 cdecl 约定以负偏移（[ebp+8], [ebp+12]...）入函数体表
void InterCode:: GenParams(AbstractAstNode* node, SymbolTable* table, int& idx){
    if(node == NULL) return;
    if(node->content == "Single_Param"){
        AbstractAstNode* param = node->getFirstChild();                     // Param_ID / array_*id 等
        AbstractAstNode* desc = param->getFirstChild();
        AbstractAstNode* id = desc ? desc->getNextSibling() : NULL;
        if(id != NULL && id->nodeType == AstNodeType::ID){
            Symbol* s = new Symbol(id->content, SymbolType::var, 4);
            s->setSymOffset(-(8 + 4 * idx));
            table->addSymbol(s);
            idx++;
        }
    }
    else if(node->content == "Some_Param"){
        AbstractAstNode* sub = node->getFirstChild();                       // 内层 VarList
        GenParams(sub, table, idx);
        AbstractAstNode* last = sub ? sub->getNextSibling() : NULL;         // 尾部 Param
        if(last != NULL){
            AbstractAstNode* desc = last->getFirstChild();
            AbstractAstNode* id = desc ? desc->getNextSibling() : NULL;
            if(id != NULL && id->nodeType == AstNodeType::ID){
                Symbol* s = new Symbol(id->content, SymbolType::var, 4);
                s->setSymOffset(-(8 + 4 * idx));
                table->addSymbol(s);
                idx++;
            }
        }
    }
}

SymbolTable* InterCode:: Body_Generate(AbstractAstNode* node, SymbolTable* symbol_table){
    if (node == NULL ){
        std::cout<<"Warning: Body_Generate received null node, skipped."<<std::endl;
        return symbol_table;
    }
    std::string node_content = node->content;
    int type = static_cast<int>(node->nodeType);
    switch(type){
        case static_cast<int>(AstNodeType:: BODY):{
            AbstractAstNode* child = node->getFirstChild();
                    // child->printNodeInfo();
                    int i=0;
                    while(child != NULL){
                        std::cout<<"Body_Gen "<<child->content<<std::endl;
                        Body_Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
        }
        break;

        case static_cast<int>(AstNodeType:: STATEMENT):{
            if(node_content == "Exp_Stmt")
            {
                Exp_Stmt_Generate(node->getFirstChild(), symbol_table);
            }
            else if(node_content == "Some_Stmt" || 
                node_content == "Def_Stmt" || 
                node_content == "Body_Stmt")
            {
                AbstractAstNode* child = node->getFirstChild();
                while(child != NULL){
                        std::cout<<"Body_Gen "<<child->content<<std::endl;
                        Body_Generate(child, symbol_table);
                        child = child->getNextSibling();
                    }
            }
            else if(node_content == "If")
            {
                Exp_Stmt_Generate(node->getFirstChild(), symbol_table); 
                AbstractAstNode* child = node->getFirstChild(); 
                int start = quad_list.size();
                std::list<int> JudgeTrue = trueList.top();
                std::list<int> JudgeFalse = falseList.top();
                trueList.pop(); 
                falseList.pop();              
                backpatch(&JudgeTrue, start);
                SymbolTable* if_symbol_table = new SymbolTable(false);
                if_symbol_table->setParentTable(symbol_table);
                if (symbol_table->firstChildTable == NULL){
                    symbol_table->firstChildTable = if_symbol_table;
                }else {
                    SymbolTable* temp = symbol_table->firstChildTable;
                    while(temp->nextSiblingTable != NULL){
                        temp = temp->nextSiblingTable;
                    }
                temp->addNextSiblingTable(symbol_table);
                }
                Body_Generate(child ->getNextSibling(), if_symbol_table);                 
                int end = quad_list.size();        
                backpatch(&JudgeFalse, end);         
            }
            else if(node_content == "If_Else")
            {                 
                Exp_Stmt_Generate(node->getFirstChild(), symbol_table);                 
                AbstractAstNode* child = node->getFirstChild()->getNextSibling();                 
                int start = quad_list.size();                 
                std::list<int> JudgeTrue = trueList.top();                 
                std::list<int> JudgeFalse = falseList.top();                 
                trueList.pop();                 
                falseList.pop();                 
                backpatch(&JudgeTrue, start);     

                SymbolTable* if_symbol_table = new SymbolTable(false);
                if_symbol_table->setParentTable(symbol_table);
                if (symbol_table->firstChildTable == NULL){
                    symbol_table->firstChildTable = if_symbol_table;
                }else {
                    SymbolTable* temp = symbol_table->firstChildTable;
                    while(temp->nextSiblingTable != NULL){
                        temp = temp->nextSiblingTable;
                    }
                temp->addNextSiblingTable(symbol_table);
                }          

                Body_Generate(child, if_symbol_table);                 
                child = child->getNextSibling();        
                QuadItem *temp =new QuadItem( (int)NULL,OpType::JUMP);                 
                this->quad_list.push_back(temp);                 
                temp = quad_list.back();                 
                int elseStart = quad_list.size();

                SymbolTable* else_symbol_table = new SymbolTable(false);
                else_symbol_table->setParentTable(symbol_table);
                if (symbol_table->firstChildTable == NULL){
                    symbol_table->firstChildTable = else_symbol_table;
                }else {
                    SymbolTable* temp = symbol_table->firstChildTable;
                    while(temp->nextSiblingTable != NULL){
                        temp = temp->nextSiblingTable;
                    }
                temp->addNextSiblingTable(symbol_table);
                }          

                Body_Generate(child, else_symbol_table);                 
                backpatch(&JudgeFalse, elseStart);                 
                int end = quad_list.size();                 
                temp->backpatch(end);             
            }
            else if(node_content == "While"){
                AbstractAstNode *child=node->getFirstChild();
                int start = quad_list.size();
                Exp_Stmt_Generate(child, symbol_table);
                std::list<int> JudgeTrue = trueList.top();                
                std::list<int> JudgeFalse = falseList.top(); 
                trueList.pop();
                falseList.pop(); 
                backpatch(&JudgeTrue, JudgeTrue.back() + 2);
                child=child->getNextSibling();
                while (child != NULL){ 
                    SymbolTable* while_symbol_table = symbol_table->addChildTable(false);
                    Body_Generate(child, while_symbol_table);
                    child = child->getNextSibling();
                }
                int end = quad_list.size();
                QuadItem *temp = new QuadItem(start,OpType::JUMP);
                this->quad_list.push_back(temp);                
                backpatch(&JudgeFalse, end+1);
            }
            else if(node_content == "For_Def_SEMI_Exp_SEMI_Exp"){
                // std::cout<<"======FOR FROM HERE!=============="<<std::endl;
                AbstractAstNode *child=node->getFirstChild();
                AbstractAstNode *for_body=child->getNextSibling()->getNextSibling()->getNextSibling();                
                SymbolTable *for_symbol_table = symbol_table->addChildTable(false);
                Exp_Stmt_Generate(child, for_symbol_table);
                int start = quad_list.size();
                //first operation
                child = child->getNextSibling();
                Exp_Stmt_Generate(child, for_symbol_table);
                std::list<int> JudgeTrue = trueList.top();
                std::list<int> JudgeFalse = falseList.top();
                trueList.pop();
                falseList.pop();
                backpatch(&JudgeTrue, JudgeTrue.back() + 2);                
                Body_Generate(for_body, for_symbol_table);
                Exp_Stmt_Generate(child->getNextSibling(), for_symbol_table);
                int end = quad_list.size();
                QuadItem *temp = new QuadItem(start,OpType::JUMP);
                this->quad_list.push_back(temp);                
                backpatch(&JudgeFalse, end+1);
            }
            else if(node_content == "Return_Exp"){
                AbstractAstNode *child=node->getFirstChild();
                Symbol* ret_val = Exp_Stmt_Generate(child,symbol_table);
                if(ret_val != NULL){
                    // 阶段8：返回数组元素/解引用时先取出（汇编 RETURN 按普通变量读取）
                    ret_val = normalizeOperand(ret_val);
                    QuadItem* quad = new QuadItem(ret_val, OpType::RETURN_OP);
                    this->quad_list.push_back(quad);
                }
            }
            else if(node_content == "Return_Void"){
                QuadItem* quad = new QuadItem((int)0, OpType::RETURN_OP);
                this->quad_list.push_back(quad);
            }
            else if(node_content == "scanf_id"){
                AbstractAstNode* idNode = node->getFirstChild();
                if(idNode != NULL){
                    Symbol* var = symbol_table->findSymbolLocally(idNode->content);
                    if(var == NULL){
                        var = symbol_table->findSymbolGlobally(idNode->content);
                    }
                    if(var != NULL){
                        QuadItem* quad = new QuadItem(var, OpType::SCAN);
                        this->quad_list.push_back(quad);
                    }
                }
            }

        }
        break;

        case static_cast<int>(AstNodeType:: DEFINITION): {
            if(node_content == "Def_Var" || node_content == "Def_Var_Block"){
                // 默认类型声明的类型为 int；
                int varType = static_cast<int>(SymbolType:: integer);
                AbstractAstNode* mod_node = node->getFirstChild();
                if(mod_node->content == "INT_TYPE"){
                    // varType不变;
                }else {
                    // varType 不是int，则需要重新定义之；
                }
                AbstractAstNode* child = mod_node->getNextSibling();
                while(child != NULL){
                    std::cout<<"Body_Gen "<<child->content<<std::endl;
                    Body_Generate(child, symbol_table);
                    child = child->getNextSibling();
                }
            }else if(node_content == "Def_Single_Var" || node_content == "Def_Some_Var"){
                AbstractAstNode* child = node->getFirstChild();
                AbstractAstNode* mod_node = node->getParent()->getFirstChild();
                if (child->content == "Var_ASSIGN"){
                    if(child->getFirstChild()->content == "Block_Single_Vardef"){
                        std::string var_name = child->getFirstChild()->getFirstChild()->content;
                        Symbol* dup_check = symbol_table->findSymbolLocally(var_name);
                        if(dup_check!= NULL && static_cast<int>(dup_check->getSymbolType()) == 2 )
                        {
                            std::cout<<"\033[31m Error!  Duplicate defination for Variable_name  \033[0m"<<var_name<<std::endl;
                        }
                        else
                        {
                            Symbol* arg1;
                            AbstractAstNode* arg1_astNode = child->getFirstChild()->getNextSibling();
                            arg1 = Exp_Stmt_Generate(arg1_astNode, symbol_table);
                            std::string arg1_content = arg1->getIDName();
                            
                            Symbol* var = new Symbol(var_name, SymbolType:: var, 4);
                            int var_symbol_type = static_cast<int>(var->getSymbolType());
                            int arg1_symbol_type = static_cast<int>(arg1->getSymbolType());
                            if(var_symbol_type == 2 && arg1_symbol_type == 2){
                                symbol_table->setOffset(symbol_table->getOffset()+var->getWidth()); 
                                var->setSymOffset(symbol_table->getOffset());
                                symbol_table->addSymbol(var);
                            }
                            QuadItem* quad;
                            if(isNumber(arg1_content))
                            {
                                // std::cout<<"arg1 is a number: "<< atoi(arg1->getIDName().c_str())<<std::endl;
                                quad = new QuadItem(var, assign, atoi(arg1->getIDName().c_str()));
                            }
                            else 
                            {
                                quad = new QuadItem(var, assign, arg1);
                            }
                            this->quad_list.push_back(quad);
                        }

                    }
                    else if(child->getFirstChild()->content == "array_*id"){
                        // int* a = &b;
                        
                        std::string var_name = child->getFirstChild()->getFirstChild()->content;
                        Symbol* dup_check = symbol_table->findSymbolLocally(var_name);
                        if(dup_check!= NULL && static_cast<int>(dup_check->getSymbolType()) == 4 )
                        {
                            std::cout<<"\033[31m  Error! Duplicate defination for Variable_name \033[0m"<<var_name<<std::endl;
                        }
                        else
                        {
                            Symbol* var = new Symbol(var_name, SymbolType::pointer, 4);
                            Symbol* addr_var = Exp_Stmt_Generate( child->getFirstChild()->getNextSibling(),symbol_table);
                            // 阶段5修复：指针与普通变量一致分配独立栈槽，避免与目标变量共享偏移
                            symbol_table->setOffset(symbol_table->getOffset()+var->getWidth());
                            var->setSymOffset(symbol_table->getOffset());
                            symbol_table->addSymbol(var);
                            QuadItem* quad = new QuadItem(var, assign, addr_var);
                            quad_list.push_back(quad);
                        }
                    }
                }
                else if(child->content == "Var_ONLY"){
                    if(child->getFirstChild()->content == "array_id[const]")
                    {
                        /* 阶段8：支持多维数组 a[2][3]，递归收集各维大小 */
                        std::string var_name;
                        std::vector<int> dims;
                        collectDims(child->getFirstChild(), dims, var_name);
                        Symbol* dup_check = symbol_table->findSymbolLocally(var_name);
                        if(dup_check != NULL && static_cast<int>(dup_check->getSymbolType()) == 5)
                        {
                            std::cout<<"\033[31m Error! Duplicate defination for Array_name \033[0m"<<var_name<<std::endl;
                        }
                        else
                        {
                            int total = 1;
                            for (size_t d = 0; d < dims.size(); ++d) total *= dims[d];
                            Symbol* var = new Symbol(var_name, SymbolType::array, total*4);
                            var->setDims(dims);
                            symbol_table->setOffset(symbol_table->getOffset()+var->getWidth());
                            var->setSymOffset(symbol_table->getOffset());
                            symbol_table->addSymbol(var);
                        }
                    }
                    else if(child->getFirstChild()->content == "array_*id" || child->getFirstChild()->content == "array_**id"){
                        std::string pointer_name = child->getFirstChild()->getFirstChild()->content;
                        Symbol* pointer_var = new Symbol(pointer_name, SymbolType::pointer, 4);
                        // 阶段5修复：指针与普通变量一致分配独立栈槽（原设计偏移恒为 -1 且
                        // 不占 total_offset，导致指针与相邻变量偏移冲突、相互覆盖）
                        symbol_table->setOffset(symbol_table->getOffset()+pointer_var->getWidth());
                        pointer_var->setSymOffset(symbol_table->getOffset());
                        symbol_table->addSymbol(pointer_var); 
                    }
                    else if(child->getFirstChild()->content == "Block_Single_Vardef"){
                        std::string var_name;
                        var_name = child->getFirstChild()->getFirstChild()->content;
                        Symbol* dup_check = symbol_table->findSymbolLocally(var_name);
                        if(dup_check!= NULL && static_cast<int>(dup_check->getSymbolType()) == 2 )
                        {
                            std::cout<<"\033[31m Error! Duplicate defination for Variable_name \033[0m"<<var_name<<std::endl;
                        }
                        else
                        {
                            Symbol* var = new Symbol(var_name, SymbolType:: var, 4);
                            symbol_table->setOffset(symbol_table->getOffset()+var->getWidth()); 
                            var->setSymOffset(symbol_table->getOffset());
                            symbol_table->addSymbol(var);
                        }
                    }
                   
                }
                if(node_content == "Def_Some_Var"){
                    AbstractAstNode* child = node->getFirstChild()->getNextSibling();
                        Body_Generate(child, symbol_table);
                }
            }
        }
        break;
        case static_cast<int>(AstNodeType:: OPERATION):{
            // std::cout<<"+++++++++Here!!!!++++++++++++"<<std::endl; 
            Exp_Stmt_Generate(node,symbol_table);
        }
        break;
        default:
            std::cout<<"Warning: Body_Generate unknown AstNodeType: "<<node_content<<", skipped."<<std::endl; 
        break;
    }
    return symbol_table;

}


// 3 function for 3backpatch
std::list<int> *InterCode::makelist(int index)
{
    std::list<int> *jumpList = new std::list<int>();
    jumpList->push_back(index);
    return jumpList;
}
std::list<int> *InterCode::merge(std::list<int> *list1, std::list<int> *list2)
{
    list1->merge(*list2);
    return list1;
}
void InterCode::backpatch(std::list<int> *backList, int target)
{
    std::list<int>::iterator it;
    std::cout<<"====backlist_begin====="<<*(backList->begin())<<"=========="<<std::endl;
    std::cout<<"====backlist_end====="<<*(backList->end())<<"=========="<<std::endl;
    std::cout<<"====target====="<<target<<"=========="<<std::endl;
    for (it = backList->begin(); it != backList->end(); it++)
    {
        quad_list[*it]->backpatch(target);
    }
    return;
}

// ==================== 阶段6：代码优化 ====================
// 四元式级优化（Root_Generate 生成后、打印与汇编生成前调用）：
//   1. 常量折叠：t := c1 op c2（纯常量算术）→ t := c
//   2. 死代码消除：结果变量（含临时变量）后续无引用的赋值/运算删除（迭代）
//   3. 不可达删除：RETURN/JUMP 之后、未被任何跳转引用的指令删除（迭代）
// 每轮删除后统一重映射 JUMP 系列四元式的 result.target（四元式行号索引）
static bool isJumpOp(OpType op){
    return op==OpType::JUMP||op==OpType::JUMP_LT||op==OpType::JUMP_LE||
           op==OpType::JUMP_GT||op==OpType::JUMP_GE||op==OpType::JUMP_EQ||
           op==OpType::JUMP_NE;
}
static bool isDeletableDefOp(OpType op){
    return op==OpType::assign||op==OpType::addtion||op==OpType::substract||
           op==OpType::multiply||op==OpType::divide||op==OpType::mod||
           op==OpType::power||op==OpType::uminus||op==OpType::logic_and||
           op==OpType::logic_or||op==OpType::logic_not;
}
// 该四元式是否为“可删除的赋值/运算”（写目标不是解引用/数组元素/取地址等副作用形式）
static bool isDeletableDef(QuadItem* q){
    if(q->result.var == NULL) return false;
    if(!isDeletableDefOp(q->op)) return false;
    std::string rn = q->result.var->getIDName();
    if(rn.empty()) return false;
    if(rn[0]=='*' || rn[0]=='&') return false;
    if(rn.find('[') != std::string::npos) return false;
    return true;
}
// 收集该四元式“引用”的符号名（use，含内嵌引用如 a[t0] / &a / *p）
static void collectUse(QuadItem* q, std::vector<std::string>& used){
    OpType op = q->op;
    int t = q->quad_item_type;
    if(isJumpOp(op)){
        if(t==1 || t==3){ if(q->arg1.var) used.push_back(q->arg1.var->getIDName()); }
        if(t==2 || t==3){ if(q->arg2.var) used.push_back(q->arg2.var->getIDName()); }
        return;
    }
    if(op==OpType::PRINT || op==OpType::PARAM || op==OpType::RETURN_OP){
        if(q->result.var) used.push_back(q->result.var->getIDName());
        return;
    }
    if(op==OpType::CALL || op==OpType::SCAN ||
       op==OpType::FUNC_LABEL || op==OpType::FUNC_END) return;
    // 副作用写目标（解引用/数组元素等，不可删除）：其符号名内嵌的变量属于使用
    if(q->result.var){
        std::string rn = q->result.var->getIDName();
        if(!rn.empty() && (rn[0]=='*' || rn[0]=='&' || rn.find('[')!=std::string::npos)){
            used.push_back(rn);
        }
    }
    switch(t){
        case 5: case 7: if(q->arg1.var) used.push_back(q->arg1.var->getIDName()); break;
        default: break;
    }
    switch(t){
        case 6: case 7: if(q->arg2.var) used.push_back(q->arg2.var->getIDName()); break;
        default: break;
    }
}
// 变量名 v 是否被任一引用符号名包含（含匹配；误报只会少删，安全）
static bool isNameUsed(const std::vector<std::string>& used, const std::string& v){
    if(v.empty()) return true;
    for(size_t k=0;k<used.size();k++){
        if(used[k].find(v) != std::string::npos) return true;
    }
    return false;
}
void InterCode::optimize(){
    // ---------- pass 1：常量折叠 ----------
    for(size_t i=0;i<quad_list.size();i++){
        QuadItem* q = quad_list[i];
        if(q->quad_item_type != 4) continue;
        OpType op = q->op;
        if(op==OpType::addtion||op==OpType::substract||op==OpType::multiply||
           op==OpType::divide||op==OpType::mod||op==OpType::power){
            int c1 = q->arg1.target, c2 = q->arg2.target;
            if((op==OpType::divide||op==OpType::mod) && c2==0) continue; // 除零不折叠
            if(op==OpType::power && c2<1) continue;  // 汇编 pow_i_i 对指数0语义特殊，跳过
            long long v=0;
            switch(op){
                case OpType::addtion:  v=(long long)c1+c2; break;
                case OpType::substract:v=(long long)c1-c2; break;
                case OpType::multiply: v=(long long)c1*c2; break;
                case OpType::divide:   v=c1/c2; break;
                case OpType::mod:      v=c1%c2; break;
                case OpType::power:    { long long r=1; for(int k=0;k<c2;k++) r*=(long long)c1; v=r; break; }
                default: break;
            }
            Symbol* re = q->result.var;
            quad_list[i] = new QuadItem(re, OpType::assign, (int)v); // t := c（type 6）
        }
    }
    // ---------- pass 2/3：死代码消除 + 不可达删除（迭代至稳定） ----------
    while(true){
        // 跳转目标集合：被引用的行号指令绝不删除
        std::set<int> targets;
        for(size_t i=0;i<quad_list.size();i++){
            if(isJumpOp(quad_list[i]->op)) targets.insert(quad_list[i]->result.target);
        }
        // 可达性分析（从每个 FUNC_LABEL 函数入口开始）
        std::vector<bool> reachable(quad_list.size(), false);
        std::stack<int> work;
        for(size_t i=0;i<quad_list.size();i++){
            if(quad_list[i]->op==OpType::FUNC_LABEL) work.push((int)i);
        }
        while(!work.empty()){
            int i=work.top(); work.pop();
            if(i<0||(size_t)i>=quad_list.size()||reachable[i]) continue;
            reachable[i]=true;
            OpType op = quad_list[i]->op;
            if(op==OpType::JUMP){
                int t=quad_list[i]->result.target;
                if(t>=0 && (size_t)t<quad_list.size() && !reachable[t]) work.push(t);
            } else if(op==OpType::JUMP_LT||op==OpType::JUMP_LE||op==OpType::JUMP_GT||
                      op==OpType::JUMP_GE||op==OpType::JUMP_EQ||op==OpType::JUMP_NE){
                if(i+1<(int)quad_list.size() && !reachable[i+1]) work.push(i+1);
                int t=quad_list[i]->result.target;
                if(t>=0 && (size_t)t<quad_list.size() && !reachable[t]) work.push(t);
            } else if(op==OpType::RETURN_OP){
                // return 后不继续顺序执行
            } else {
                if(i+1<(int)quad_list.size() && !reachable[i+1]) work.push(i+1);
            }
        }
        // 收集可达指令的 use 符号名（不可达指令的 use 不计）
        std::vector<std::string> used;
        for(size_t i=0;i<quad_list.size();i++){
            if(!reachable[i]) continue;
            collectUse(quad_list[i], used);
        }
        // 标记删除
        std::vector<bool> del(quad_list.size(), false);
        bool changed=false;
        for(size_t i=0;i<quad_list.size();i++){
            QuadItem* q = quad_list[i];
            if(!reachable[i]){
                // 不可达：保留函数边界与跳转目标，其余删除
                if(q->op!=OpType::FUNC_LABEL && q->op!=OpType::FUNC_END &&
                   targets.count((int)i)==0){
                    del[i]=true; changed=true;
                }
                continue;
            }
            if(targets.count((int)i)) continue;   // 跳转目标保护
            if(!isDeletableDef(q)) continue;
            std::string dn = q->result.var->getIDName();
            if(!isNameUsed(used, dn)){            // 结果无任何引用 → 死代码
                del[i]=true; changed=true;
            }
        }
        if(!changed) break;
        // 重建列表 + 重映射跳转目标
        std::vector<int> remap(quad_list.size(), -1);
        std::vector<QuadItem*> newList;
        for(size_t i=0;i<quad_list.size();i++){
            if(!del[i]){ remap[i]=(int)newList.size(); newList.push_back(quad_list[i]); }
        }
        for(size_t i=0;i<newList.size();i++){
            QuadItem* q = newList[i];
            if(isJumpOp(q->op)){
                int t=q->result.target;
                if(t>=0 && (size_t)t<remap.size() && remap[t]>=0) q->result.target=remap[t];
            }
        }
        quad_list = newList;
    }
}