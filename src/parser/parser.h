#pragma once

#include <string>
#include <vector>
#include "lexer/lexer.h"
#include "ast/ast.h"

class Parser {
    private:
        std::vector<Token*> tokens;
        Program* ast_root = nullptr;

    public:
        Parser();
        
        Parser(std::vector<Token*> tokens);
        
        ~Parser();

        // Grammar rule based parsing functions

        Program* parseProgram();


        // Statement returns
        StmtNode* parseStatement();

        StmtNode* parseBlock();

        StmtNode* parseVarDecl();

        StmtNode* parseIfStmt();

        StmtNode* parsePrintStmt();

        StmtNode* parseBreakStmt() { return nullptr; } // Unsupported

        StmtNode* parseContinueStmt() { return nullptr; } // Unsupported

        StmtNode* parseExprStmt();

        // Expression returns

        ExprNode* parseExpression();

        ExprNode* parseLogicalOr();

        ExprNode* parseLogicalAnd();

        ExprNode* parseEquality();

        ExprNode* parseComparison();

        ExprNode* parseTerm();

        ExprNode* parseFactor();

        ExprNode* parseUnary();

        ExprNode* parsePrimary();

};