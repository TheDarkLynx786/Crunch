#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../lexer/lexer.h"

// Base Classes
class ExprNode { public: virtual ~ExprNode() = default; };
class StmtNode { public: virtual ~StmtNode() = default; };

// Root Wrapper
class Program {
    public:
        std::vector<StmtNode*> statements;

        Program() {}

        Program(const std::vector<StmtNode*>& statements) : statements(statements) {}
        
        ~Program() { for (auto stmt : statements) { delete stmt; } }

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
};

class VarDeclStmt : public StmtNode { 
    public:
        TokenType type;
        std::string name;

        ExprNode* init;

        VarDeclStmt(TokenType type, const std::string& name, ExprNode* init) : type(type), name(name), init(init) {}

        ~VarDeclStmt() { delete init; }
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