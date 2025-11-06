#ifndef ASMGENERATOR_H
#define ASMGENERATOR_H
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include "./AsmCode.h"
#include "../symbol_table/symbol.h"
#include "../intermidiate_code/interCode.h"
//需要加上funcsymbol
#include "../symbol_table/FuncSymbol.h"
using namespace std;
enum class asmRegister {
    unset = -1,
    eax = 1,
    ebx = 2,
    ecx = 3,
    edx = 4,
    esp = 5,
    ebp = 6
};

class AsmCode {
private:
    std::string codeBuffer;
    inline std::string transRegister(asmRegister reg);
public:
    inline AsmCode();
    inline std::string generateVar(int offset);
    inline std::string generateInstanceNumber(int value);
    inline void generateBinaryInstructor(std::string instructor, asmRegister reg1, asmRegister reg2);
    inline void generateBinaryInstructor(std::string instructor, asmRegister reg, std::string var);
    inline void generateBinaryInstructor(std::string instructor, std::string var, asmRegister reg);
    inline void generateBinaryInstructor(std::string instructor, std::string var1, std::string var2);
    inline void generateUnaryInstructor(std::string instructor, std::string var);
    inline void generateUnaryInstructor(std::string instructor, asmRegister reg);
    inline std::string findValueByAddress(asmRegister reg);
    // ADD instructor, add reg1, reg2
    inline void add(asmRegister reg1, asmRegister reg2);
    // ADD instructor, add reg, var
    inline void add(asmRegister reg, std::string var);
    inline void sub(asmRegister reg1, asmRegister reg2);
    inline void sub(asmRegister reg, std::string var);
    inline void mov(asmRegister reg1, asmRegister reg2);
    inline void mov(asmRegister reg, std::string var);
    inline void mov(std::string var, asmRegister reg);
    inline void mov(std::string var1, std::string var2);
    inline void mul(asmRegister reg1, asmRegister reg2);
    inline void mul(asmRegister reg, std::string var);
    inline void mul(std::string var1, std::string var2);
    inline void div(asmRegister reg1, asmRegister reg2);
    inline void div(asmRegister reg, std::string var);
    inline void div(std::string var, asmRegister reg);
    inline void div(std::string var1, std::string var2);
    inline void asmXor(asmRegister reg1, asmRegister reg2);
    inline void push(asmRegister reg);
    inline void push(std::string var);
    inline void pop(asmRegister reg);
    inline void label(std::string label);
    inline void addCode(std::string code);
    
    friend inline std::ostream& operator<<(std::ostream& os, const AsmCode& asmcode);
    std::string getcodeBuffer(){return codeBuffer;}

};

class AsmGenerate
{
private:
    int eax;
    int ebx;
    int ecx;
    int edx;
    AsmCode asmcode;
    std::string registerUsedVar[6];
    //std::vector<Symbol*> tempVar;
    std::vector<QuadItem*> quad_list;
    std::map<int, int> labelMap;
    SymbolTable* rootTable;
    SymbolTable* currentTable;
    //FuncTable funcTable;
    inline void releaseRegister(asmRegister reg);
    inline asmRegister getRegister(std::string var);
    inline asmRegister findRegister(std::string var);
    inline void generateSetArg(QuadItem q);
    inline void generateArithmetic(QuadItem q);
    inline void generateJump(QuadItem q);
    inline void generateNeg(QuadItem q);
    inline void generatePower(QuadItem q);
    inline void preSetLabel();
    inline bool isJumpQuad(OpType code);
    inline void generateprint(QuadItem q);
public:
    inline Symbol* getoffsetofarray(Symbol* arg);
    inline AsmCode& getAsmCode() { return this->asmcode; }
    //AsmGenerate(std::vector<QuadItem*>quads, std::vector<Symbol*> tempVar, SymbolTable* rootTable, FuncTable funcTable);
    inline AsmGenerate(std::vector<QuadItem*>quads, SymbolTable* rootTable);
    inline void generate();
    void printbuffer(){
        std::cout<<"buffer:"<<this->asmcode.getcodeBuffer()<<"\n\n";
    }
};

#endif
