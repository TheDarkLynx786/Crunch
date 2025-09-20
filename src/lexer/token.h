#pragma once

#include <string>

enum class TokenType {
    // Keywords
    KW_IF, KW_ELSE, KW_WHILE, KW_FOR, KW_BRK, KW_CONT, KW_PRINT,
    KW_TRUE, KW_FALSE,

    // Identifiers and Literals
    IDENTIFIER,
    INT_LIT,
    DBLE_LIT,
    STR_LIT,
    BOOL_LIT,
    FUNCTION_MATH, // My custom "function" data type

    // Operators
    PLUS, MINUS, MULTI, DIV, MOD,
    SIN, COS, TAN, EXP, LOG, SQRT,
    DERIV, INTEGRAL,

    // =, ==, !=, <, >, <=, >=, &&, ||, !
    ASSIGN, EQ, NEQ, LT, GT, LEQ, GEQ, AND, OR, NOT,

    // Delims
    COMMA, SEMICOL, COL, DOT, LPAREN, RPAREN, LBRACE, RBRACE,

    // Constants
    PI, EULER,
    
    // Other
    END_OF_FILE, UNKNOWN
};



class Token {
    private:
        TokenType type;
        std::string lexeme;
        int ln, col;
    
    public:
        Token() {
            this->type = TokenType::UNKNOWN;
            this->lexeme = "";
            this->ln = -1;
            this->col = -1;
        }
    
        Token(TokenType type, std::string& lexeme, int ln, int col) {
            this->type = type;
            this->lexeme = this->tokenToString(type);
            this->ln = ln;
            this->col = col;
        }

        std::string tokenToString(TokenType type) {
            switch(type) {
                case TokenType::KW_IF: {return "if"; break;}
                case TokenType::KW_ELSE: {return "else"; break;}
                case TokenType::KW_WHILE: {return "while"; break;}
                case TokenType::KW_FOR: {return "for"; break;}
                case TokenType::KW_BRK: {return "break"; break;}
                case TokenType::KW_CONT: {return "continue"; break;}
                case TokenType::KW_PRINT: {return "print"; break;}
                case TokenType::KW_TRUE: {return "true"; break;}
                case TokenType::KW_FALSE: {return "false"; break;}
                case TokenType::IDENTIFIER: {return "identifier"; break;}
                case TokenType::INT_LIT: {return "int"; break;}
                case TokenType::DBLE_LIT: {return "double"; break;}
                case TokenType::STR_LIT: {return "string"; break;}
                case TokenType::BOOL_LIT: {return "bool"; break;}
                case TokenType::FUNCTION_MATH: {return "function"; break;}
                case TokenType::PLUS: {return "+"; break;};
                case TokenType::MINUS: {return "-"; break;}
                case TokenType::MULTI: {return "*"; break;}
                case TokenType::DIV: {return "/"; break;}
                case TokenType::MOD: {return "%"; break;}
                case TokenType::SIN: {return "sin"; break;}
                case TokenType::COS: {return "cos"; break;}
                case TokenType::TAN: {return "tan"; break;}
                case TokenType::EXP: {return "exp"; break;}
                case TokenType::LOG: {return "log"; break;}
                case TokenType::SQRT: {return "sqrt"; break;}
                case TokenType::DERIV: {return "deriv"; break;}
                case TokenType::INTEGRAL: {return "integral"; break;}
                case TokenType::ASSIGN: {return "="; break;}
                case TokenType::EQ: {return "=="; break;}
                case TokenType::NEQ: {return "!="; break;}
                case TokenType::LT: {return "<"; break;}
                case TokenType::GT: {return ">"; break;}
                case TokenType::LEQ: {return "<="; break;}
                case TokenType::GEQ: {return ">="; break;}
                case TokenType::AND: {return "&&"; break;}
                case TokenType::OR: {return "||"; break;}
                case TokenType::NOT: {return "!"; break;}
                case TokenType::COMMA: {return ","; break;}
                case TokenType::SEMICOL: {return ";"; break;}
                case TokenType::COL: {return ":"; break;}
                case TokenType::DOT: {return "."; break;}
                case TokenType::LPAREN: {return "("; break;}
                case TokenType::RPAREN: {return ")"; break;}
                case TokenType::LBRACE: {return "{"; break;}
                case TokenType::RBRACE: {return "}"; break;}
                case TokenType::PI: {return "pi"; break;}
                case TokenType::EULER: {return "e"; break;}
                case TokenType::END_OF_FILE: {return "EOF"; break;}
                case TokenType::UNKNOWN: {return "unknown"; break;}
                default: {return "unknown"; break;}
            }
        }

        // Getters
        TokenType getType() const { return type; }
        std::string getLexeme() const { return lexeme; }
        int getLine() const { return ln; }
        int getColumn() const { return col; }

        // Setters
        void setType(TokenType type) { this->type = type; }
        void setLineCol(int ln, int col) { this->ln = ln; this->col = col;}

};