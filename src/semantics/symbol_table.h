#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <llvm/IR/Value.h>
#include "../parser/parser.h"   // for TokenType

struct Symbol {
    std::string name;
    TokenType type;                        // variable type
    llvm::AllocaInst* llvmValue = nullptr;
};

class SymbolTable {
    private:
        std::vector<std::unordered_map<std::string, Symbol>> scopes;

    public:
        SymbolTable(); // global scope

        // Enter a new scope
        void pushScope();

        // Leave current scope
        void popScope();

        // Declare a new symbol in the current scope
        // Returns false if a symbol with the same name exists in the current scope
        bool declare(const std::string& name, TokenType type, llvm::AllocaInst* llvmValue = nullptr);

        // Lookup symbol in all scopes (inner to outer)
        Symbol* lookup(const std::string& name);
};