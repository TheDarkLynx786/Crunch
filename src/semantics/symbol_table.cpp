#include "symbol_table.h"

SymbolTable::SymbolTable() { pushScope(); } // global scope

// Enter a new scope
void SymbolTable::pushScope() { scopes.push_back({}); }

// Leave current scope
void SymbolTable::popScope() { 
    if (!scopes.empty()) scopes.pop_back(); 
}

// New symbol, returns false if one already exists
bool SymbolTable::declare(const std::string& name, TokenType type, llvm::AllocaInst* llvmValue = nullptr) {
    if (scopes.empty()) pushScope();

    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        return false; // duplicate declaration in same scope
    }

    currentScope[name] = Symbol{name, type, llvmValue};
    return true;
}

// Lookup symbol in all scopes (inner to outer)
Symbol* SymbolTable::lookup(const std::string& name) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) return &it->second;
    }
    return nullptr; // not found
}