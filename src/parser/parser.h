#pragma once

#include <string>
#include <vector>
#include "../lexer/lexer.h"
#include "../ast/ast.h"

class Parser {
    private:
        std::vector<Token*> tokens;
        size_t current;
        Program* ast_root = nullptr;

    public:
        Parser();
        
        Parser(std::vector<Token*> tokens);
        
        ~Parser();

        // Helper functions
        
        bool isAtEnd() { return peek()->getType() == TokenType::END_OF_FILE; }
        Token* peek() { return tokens.at(current); }
        Token* previous() const { return tokens.at(current - 1); }
        Token* advance() { if (!isAtEnd()) current++; return previous(); }
        bool check(TokenType type) { return !isAtEnd() && peek()->getType() == type; }

        Token* consume(TokenType type, const std::string& message) {
            if (check(type)) return advance();
            throw std::runtime_error(message);
        }


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

        // Print Tree
        void printTree();

};