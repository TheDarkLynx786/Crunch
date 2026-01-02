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

    ~codegen_ctx() { delete symTable; }
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

        llvm::Value* codegen(codegen_ctx& ctx) override {

            llvm::Value* l = left->codegen(ctx);
            llvm::Value* r = right->codegen(ctx);

            if (!l || !r) {
                std::cerr << "Failed to generate code for binary expression operands." << std::endl;
                return nullptr;
            }

            // Type Promotions between operations
            
            if (op == "+") {
                
                // TODO Add String type promos
                
                if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {
                    if (l->getType()->isIntegerTy()) {
                        l = ctx.builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    if (r->getType()->isIntegerTy()) {
                        r = ctx.builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    return ctx.builder.CreateFAdd(l, r, "addtmp");
                } else {
                    return ctx.builder.CreateAdd(l, r, "addtmp");
                }
            
            } else if (op == "-") {
                
                if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {
                    if (l->getType()->isIntegerTy()) {
                        l = ctx.builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    if (r->getType()->isIntegerTy()) {
                        r = ctx.builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    return ctx.builder.CreateFSub(l, r, "subtmp");
                } else {
                    return ctx.builder.CreateSub(l, r, "subtmp");
                }

            } else if (op == "*") {
                
                if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {
                    if (l->getType()->isIntegerTy()) {
                        l = ctx.builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    if (r->getType()->isIntegerTy()) {
                        r = ctx.builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    return ctx.builder.CreateFMul(l, r, "multmp");
                } else {
                    return ctx.builder.CreateMul(l, r, "multmp");
                }

            } else if (op == "/") {
                
                if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {
                    if (l->getType()->isIntegerTy()) {
                        l = ctx.builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    if (r->getType()->isIntegerTy()) {
                        r = ctx.builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    return ctx.builder.CreateFDiv(l, r, "divtmp");
                } else {
                    // Using signed division for integers - POTENTIAL BUG
                    return ctx.builder.CreateSDiv(l, r, "divtmp");
                }
            } else if (op == "%") {
                
                if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {
                    if (l->getType()->isIntegerTy()) {
                        l = ctx.builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    if (r->getType()->isIntegerTy()) {
                        r = ctx.builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx.context), "int_to_double");
                    }
                    return ctx.builder.CreateSRem(l, r, "modtmp");
                } else {
                    return ctx.builder.CreateSRem(l, r, "modtmp");
                }
            }

            // TODO Add logical comparisons, equality, and commas

            // Unknown operator error
            std::cerr << "Unsupported binary operator: " << op << std::endl;
            return nullptr;
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

        llvm::Value* codegen(codegen_ctx& ctx) override {
            
            llvm::Value* val = operand->codegen(ctx);

            if (!val) {
                std::cerr << "Failed to generate code for unary expression operand." << std::endl;
                return nullptr;
            }

            if (op == "-") {
                
                if (val->getType()->isDoubleTy()) {
                    return ctx.builder.CreateFNeg(val, "negtmp");
                } else if (val->getType()->isIntegerTy()) {
                    return ctx.builder.CreateNeg(val, "negtmp");
                } else {
                    std::cerr << "Unsupported type for unary negation." << std::endl;
                    return nullptr;
                }

            } else if (op == "!") {
                
                // Boolean type
                if (val->getType()->isIntegerTy(1)) { 
                    return ctx.builder.CreateNot(val, "nottmp");
                } else {
                    std::cerr << "Unsupported type for logical NOT." << std::endl;
                    return nullptr;
                }

            }

            // Unknown operator error
            std::cerr << "Unsupported unary operator: " << op << std::endl;
            return nullptr;
        }
};

class LiteralExpr : public ExprNode {
    public:
        std::string value;

        LiteralExpr(const std::string& value) : value(value) {}

        ~LiteralExpr() {}

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return nullptr;
        }
};

class IdentifierExpr : public ExprNode {
    public:
        std::string name;

        IdentifierExpr(const Token& t) : name(t.getLexeme()) {}

        ~IdentifierExpr() {}

        llvm::Value* codegen(codegen_ctx& ctx) override {
            
            Symbol* sym = ctx.symTable->lookup(name);
            
            if (!sym) {
                std::cerr << "Undefined variable: " << name << std::endl;
                return nullptr;
            }

            // Variable value
            return ctx.builder.CreateLoad(sym->type, sym->llvmValue, sym->name);

        }
};

class AssignmentExpr : public ExprNode {
    public:
        std::string name;
        //TokenType type; // TODO add type detection (if variable declaration doesn't already handle it)
        ExprNode* expr;

        AssignmentExpr(ExprNode* expr, const std::string& name) : name(name), expr(expr) {}

        ~AssignmentExpr() { delete expr; }

        llvm::Value* codegen(codegen_ctx& ctx) {
            return nullptr; // TODO
        }

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

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return nullptr; // TODO
        }
};

class BoolLiteral : public ExprNode {
    public:
        bool value;
        BoolLiteral(bool v) : value(v) {}

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx.context), value);
        }
};

class IntLiteral : public ExprNode {
    public:
        int value;
        IntLiteral(const Token& t) : value(std::stoi(t.getLexeme())) {}

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), value);
        }
};

class DoubleLiteral : public ExprNode {
    public:
        double value;
        DoubleLiteral(const Token& t) : value(std::stod(t.getLexeme())) {}

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx.context), value);
        }
};

class StringLiteral : public ExprNode {
    public:
        std::string value;
        StringLiteral(const Token& t) : value(t.getLexeme()) {}
        
        llvm::Value* codegen(codegen_ctx& ctx) override {
            return llvm::ConstantDataArray::getString(ctx.context, value, true);
        }
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
                    
                    var_type = llvm::Type::getInt8PtrTy(ctx.context);
                    break;
                }

                default:
                    std::cerr << "Unsupported variable type" << std::endl;
                    return nullptr;
            }
            
            // Create allocation instance
            llvm::AllocaInst* alloca = ctx.builder.CreateAlloca(var_type, nullptr, name);

            // Add to symbol table and check if no repeated declaration in scope
            if (!ctx.symTable->declare(name, var_type, alloca)) {
                std::cerr << "Variable already declared in scope: " << name << std::endl;
                return nullptr;
            }

            // Handle initialization if initalizer is present
            llvm::Value* init_val = nullptr;
            if (init) {
                
                init_val = init->codegen(ctx);
                
                // Type Checking and Promotion
                if (init_val && init_val->getType() != var_type) {

                    // Int to Double Promotion
                    if ( var_type->isDoubleTy() && init_val->getType()->isIntegerTy() ) {
                        init_val = ctx.builder.CreateSIToFP(init_val, var_type, "int_to_double");
                    } else if ( var_type->isIntegerTy() && init_val->getType()->isDoubleTy() ) {
                        init_val = ctx.builder.CreateFPToSI(init_val, var_type, "double_to_int");
                    }
                    else {
                        std::cerr << "Type mismatch in variable initialization for variable: " << name << std::endl;
                        return nullptr;
                    }
                }

            } 
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

        llvm::Value* codegen(codegen_ctx& ctx) override {
            
            // Push scope
            ctx.symTable->pushScope();

            llvm::Value* last = nullptr;
            for (auto stmt: statements) {
                last = stmt->codegen(ctx);
            }

            // Pop scope 
            ctx.symTable->popScope();

            return last;
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

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return nullptr; // TODO
        }
};

class PrintStmt : public StmtNode { 
    public:
        ExprNode* value;

        PrintStmt(ExprNode* value) : value(value) {}

        ~PrintStmt() { delete value; }

        llvm::Value* codegen(codegen_ctx& ctx) override {
            return nullptr; // TODO
        }
};

class WhileStmt :public StmtNode { 
    // Unsupported for now
};

class ForStmt : public StmtNode { 
    // Unsupported for now
};

class BreakStmt : public StmtNode { 
    // Unsupported for now
};

class ContinueStmt :public StmtNode { 
    // Unsupported for now
};

class FunctionDeclStmt : public StmtNode { 
    // Unsupported, I'm not supposed to have function declarations lol
    // Though in the off chance I do add them
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