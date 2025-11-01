#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../lexer/lexer.h"
#include "../semantics/symbol_table.h"

// Context Structure
struct codegen_ctx {
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;

    // Symbol Table
    SymbolTable* symTable = new SymbolTable();
    
    codegen_ctx(const std::string &moduleName) : builder(context) {
        module = std::make_unique<llvm::Module>(moduleName, context);
    }
};

// Base Classes
class ASTNode {
    public:
        virtual ~ASTNode() = default;

        // Optional: printing for debugging
        // virtual void print(int indent = 0) const = 0;

        // LLVM code generation
        virtual llvm::Value* codegen(codegen_ctx&) = 0;
};

class ExprNode : public ASTNode { 
    public:
        virtual ~ExprNode() = default; 

        virtual llvm::Value* codegen(codegen_ctx& ctx) override = 0;
};

class StmtNode : public ASTNode {
    public:
        virtual ~StmtNode() = default; 

        virtual llvm::Value* codegen(codegen_ctx& ctx) override = 0;
};

// Root Wrapper
class Program : public ASTNode {
    public:
        std::vector<StmtNode*> statements;

        Program() {}

        Program(const std::vector<StmtNode*>& statements) : statements(statements) {}
        
        ~Program() { for (auto stmt : statements) { delete stmt; } }

        // void print(int indent = 0) const override = 0;

        llvm::Value* codegen(codegen_ctx& ctx) override {
            
            llvm::Value* last = nullptr;
            for (auto stmt : statements) {
                last = stmt->codegen(ctx);
            }
            return last;
        }

};


// Expression Nodes
class BinaryExpr : public ExprNode {
    public:
        std::string op;
        ExprNode* left;
        ExprNode* right;

        BinaryExpr(ExprNode* left, const std::string& op, ExprNode* right) : left(left), op(op), right(right) {}
        
        ~BinaryExpr() {
            delete left;
            delete right;
        }
};

class UnaryExpr : public ExprNode {
    public:
        std::string op;
        ExprNode* operand;

        UnaryExpr(const std::string& op, ExprNode* operand) : op(op), operand(operand) {}
        
        ~UnaryExpr() {
            delete operand;
        }
};

class LiteralExpr : public ExprNode {
    public:
        std::string value;

        LiteralExpr(const std::string& value) : value(value) {}

        ~LiteralExpr() {}
};

class IdentifierExpr : public ExprNode {
    public:
        std::string name;

        IdentifierExpr(const std::string& name) : name(name) {}

        ~IdentifierExpr() {}
};

class CallExpr : public ExprNode {
    public:
        ExprNode* callee;
        std::vector<ExprNode*> args;

        CallExpr(ExprNode* callee, const std::vector<ExprNode*>& args) : callee(callee), args(args) {}

        ~CallExpr() {
            delete callee;
            for (auto arg : args) { delete arg; }
        }
};

class BoolLiteral : public ExprNode {
    public:
        bool value;
        BoolLiteral(bool v) : value(v) {}
};

class IntLiteral : public ExprNode {
    public:
        int value;
        IntLiteral(const Token& t) : value(std::stoi(t.getLexeme())) {}
};

class DoubleLiteral : public ExprNode {
    public:
        double value;
        DoubleLiteral(const Token& t) : value(std::stod(t.getLexeme())) {}
};

class StringLiteral : public ExprNode {
    public:
        std::string value;
        StringLiteral(const Token& t) : value(t.getLexeme()) {}
        
        // TODO
        llvm::Value* codegen(codegen_ctx& ctx) override {}
};

class VariableExpr : public ExprNode {
    public:
        std::string name;
        VariableExpr(const Token& t) : name(t.getLexeme()) {}
};


// Statement Nodes
class ExprStmt : public StmtNode { 
    public:
        ExprNode* expr;

        ExprStmt(ExprNode* expr) : expr(expr) {}

        ~ExprStmt() { delete expr; }

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return expr->codegen(ctx);
        }
};

class VarDeclStmt : public StmtNode { 
    public:
        TokenType type;
        std::string name;

        ExprNode* init;

        VarDeclStmt(TokenType type, const std::string& name, ExprNode* init) : type(type), name(name), init(init) {}

        ~VarDeclStmt() { delete init; }

        llvm::Value* codegen(codegen_ctx& ctx) override {
            
            llvm::Type* var_type = nullptr;
            
            // Assign var_type to token type
            switch(type) {
                
                case TokenType::KW_INT:
                    var_type = llvm::Type::getInt32Ty(ctx.context); break;
                
                case TokenType::KW_DBLE:
                    var_type = llvm::Type::getDoubleTy(ctx.context); break;
                
                case TokenType::KW_BOOL:
                    var_type = llvm::Type::getInt1Ty(ctx.context); break; // 1 bit
                
                case TokenType::KW_STRING: { 
                    
                    size_t length = 0;

                    if (typeid(init) == typeid(StringLiteral) ) {
                        
                        std::string val = dynamic_cast<StringLiteral*>(init)->value;
                        length = val.length() + 1; // +1 for null terminator

                    } else {
                        
                        std::cerr << "Expected a string literal for intializing string variable" << std::endl;
                        return nullptr;

                    }
                    
                    var_type = llvm::ArrayType::get( llvm::Type::getInt8Ty(ctx.context), length ); break;
                }

                default:
                    std::cerr << "Unsupported variable type" << std::endl;
                    return nullptr;
            }
            
            // Create allocation instance
            llvm::AllocaInst* alloca = ctx.builder.CreateAlloca(var_type, nullptr, name);

            // Add to symbol table and check if no repeated declaration in scope
            if (!ctx.symTable->declare(name, type, alloca)) {
                std::cerr << "Variable already declared in scope: " << name << std::endl;
                return nullptr;
            }

            // Handle initialization if initalizer is present
            llvm::Value* init_val = nullptr;
            if (init) { init_val = init->codegen(ctx); } 
            else {
                
                // Default storage values
                switch(var_type->getTypeID()) {
                    case llvm::Type::IntegerTyID:
                        init_val = llvm::ConstantInt::get(var_type, 0); break;
                    case llvm::Type::DoubleTyID:
                        init_val = llvm::ConstantFP::get(var_type, 0.0); break;
                    case llvm::Type::ArrayTyID:
                        init_val = llvm::ConstantAggregateZero::get(var_type); break;
                    default:
                        std::cerr << "Unsupported variable type for default initialization" << std::endl;
                        return nullptr;
                }

            }
            
            ctx.builder.CreateStore(init_val, alloca);
            return alloca;

        }
};

class BlockStmt : public StmtNode { 
    public:
        std::vector<StmtNode*> statements;

        BlockStmt(const std::vector<StmtNode*>& statements) : statements(statements) {}

        ~BlockStmt() { 
            for (auto stmt : statements) { delete stmt; }
        }
};

class IfStmt : public StmtNode { 
    public:
        ExprNode* condition;
        StmtNode* thenBranch;
        StmtNode* elseBranch; // can be nullptr

        IfStmt(ExprNode* condition, StmtNode* thenBranch, StmtNode* elseBranch = nullptr) 
            : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

        ~IfStmt() { 
            delete condition; 
            delete thenBranch; 
            if (elseBranch) delete elseBranch; 
        }
};

class PrintStmt : public StmtNode { 
    public:
        ExprNode* value;

        PrintStmt(ExprNode* value) : value(value) {}

        ~PrintStmt() { delete value; }
};

class WhileStmt :public StmtNode { 
    // Unsupported 
};

class ForStmt : public StmtNode { 
    // Unsupported
};

class BreakStmt : public StmtNode { 
    // Unsupported
};

class ContinueStmt :public StmtNode { 
    // Unsupported
};

class FunctionDeclStmt : public StmtNode { 
    // Unsupported, I'm not supposed to have function declarations lol
    /*public:
        std::string returnType;
        std::string name;
        std::vector<std::pair<std::string, std::string>> params; // pair of <type, name>
        BlockStmt* body;

        FunctionDeclStmt(const std::string& returnType, const std::string& name, 
                         const std::vector<std::pair<std::string, std::string>>& params, BlockStmt* body) 
            : returnType(returnType), name(name), params(params), body(body) {}

        ~FunctionDeclStmt() { delete body; }*/
};