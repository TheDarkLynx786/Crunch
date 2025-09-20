#include <string>

enum class TokenType {
    // Keywords
    KW_IF, KW_ELSE, KW_WHILE, KW_FOR, KW_BRK, KW_CONT, KW_print,
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
    COMMENT, WHITESPACE, END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int ln, col;
};