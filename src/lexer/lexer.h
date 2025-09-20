#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "token.h"
#include <vector>
#include <unordered_map>
#include <regex>

class Lexer {
    private: 
        std::ifstream sourceFile;
        std::vector<Token*> tokens;
        int ln = 0;
        int col = 0;

        std::unordered_map<std::string, TokenType> lex_rules = {
            
            // Keywords
            {"if", TokenType::KW_IF},
            {"else", TokenType::KW_ELSE},
            {"while", TokenType::KW_WHILE},
            {"for", TokenType::KW_FOR},
            {"break", TokenType::KW_BRK},
            {"continue", TokenType::KW_CONT},
            {"print", TokenType::KW_PRINT},
            {"true", TokenType::KW_TRUE},
            {"false", TokenType::KW_FALSE},
            
            // Constants
            {"pi", TokenType::PI},
            {"e", TokenType::EULER},

            // (Identifiers and literals done dynamically)

            // Type Keywords 
            {"int", TokenType::KW_INT},
            {"double", TokenType::KW_DBLE},
            {"string", TokenType::KW_STRING},
            {"bool", TokenType::KW_BOOL},
            {"function", TokenType::KW_FUNCTION},

            // Operators
            {"+", TokenType::PLUS},
            {"-", TokenType::MINUS},
            {"*", TokenType::MULTI},
            {"/", TokenType::DIV},
            {"%", TokenType::MOD},
            {"sin", TokenType::SIN},
            {"cos", TokenType::COS},
            {"tan", TokenType::TAN},
            {"exp", TokenType::EXP},
            {"log", TokenType::LOG},
            {"sqrt", TokenType::SQRT},
            {"deriv", TokenType::DERIV},
            {"integral", TokenType::INTEGRAL},
            
            // Assignment and Comparison
            {"=", TokenType::ASSIGN},
            {"==", TokenType::EQ},
            {"!=", TokenType::NEQ},
            {"<", TokenType::LT},
            {">", TokenType::GT},
            {"<=", TokenType::LEQ},
            {">=", TokenType::GEQ},
            {"&&", TokenType::AND},
            {"||", TokenType::OR},
            {"!", TokenType::NOT},
            
            // Delimiters
            {",", TokenType::COMMA},
            {";", TokenType::SEMICOL},
            {":", TokenType::COL},
            {".", TokenType::DOT},
            {"(", TokenType::LPAREN},         
            {")", TokenType::RPAREN},
            {"{", TokenType::LBRACE},
            {"}", TokenType::RBRACE}
        };

    public:
        Lexer();
        
        Lexer(const std::string& filename);
        
        ~Lexer();
        
        void tokenize();

        void toString() const;

        void reset();

        bool isEOF() const;
};