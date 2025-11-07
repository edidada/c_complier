#include "symbol.h"
#include <queue>
#include "../abstract_syntax_tree/AstNode.h"

Symbol::Symbol(){
    this->idName = "";
    this->idType = SymbolType::integer;
    this->width = 4;
    this->value= "0";
    this->isUsed = false;
}

Symbol::Symbol(std::string name, SymbolType type, int width, std::string init_value){
    this->idName = name;
    this->idType = type;
    this->width = width;
    this->value = init_value;
    this->isUsed = false;
}


SymbolTable:: SymbolTable(bool isFunc){
    this->isFunctionTable = isFunc;
    this->firstChildTable = nullptr;
    this->nextSiblingTable = nullptr;
    this->parentTable = nullptr;
    this->baseTable = nullptr;
    this->total_offset = 0;
    if(isFunc){
        symbolArray = new std::vector<Symbol* >();
        argArray = new std::vector<Symbol*>();
    }
}
SymbolTable:: SymbolTable(bool isFunc, SymbolTable* parent){
    this->isFunctionTable = isFunc;
    this->firstChildTable = nullptr;
    this->nextSiblingTable = nullptr;
    this->parentTable = parent;
    this->total_offset = 0;
    SymbolTable* temp = this;
    while(!temp->isFunctionTable){
        temp = temp->parentTable;
    }
    this->baseTable = temp;
    if(isFunc){
        symbolArray = new std::vector<Symbol* >();
        argArray = new std::vector<Symbol*>();
    }

}

Symbol* SymbolTable:: findSymbolLocally(const std::string name){
    std::unordered_map<std::string, Symbol*>::iterator iter;
    iter = this->SymbolMap.find(name);
    if(iter != this->SymbolMap.end()){
        return iter->second;
    }
    return nullptr;
}

bool SymbolTable:: addSymbol(Symbol* symbol){
    if(this->findSymbolLocally(symbol->getIDName()) == nullptr){
        this->SymbolMap[symbol->getIDName()] = symbol;
        this->symbolNumber ++;
        return true;
    }
    return false;
}

Symbol* SymbolTable:: findSymbolGlobally(std::string name){
    SymbolTable* temp = this;
    while(temp != nullptr){
        Symbol* result = temp->findSymbolLocally(name);
        if(result == nullptr){
            temp = temp->parentTable;
        }else return result;
    }
    std::cout<<"\033[31m Error: Undefined ID: \033[0m"<<name<<std::endl;
    exit(1);
}

SymbolTable* SymbolTable:: addChildTable(bool isFunc){
    SymbolTable* child = new SymbolTable(isFunc);
    child->setParentTable(this);
    if (this->firstChildTable == nullptr){
        this->firstChildTable = child;
    }else {
        SymbolTable* temp = this->firstChildTable;
        while(temp->nextSiblingTable != nullptr){
            temp = temp->nextSiblingTable;
        }
        temp->addNextSiblingTable(child);
    }
    return child;
}

void SymbolTable:: addNextSiblingTable(SymbolTable* ns){
    this->nextSiblingTable = ns;
    ns->parentTable = this->parentTable;
}

