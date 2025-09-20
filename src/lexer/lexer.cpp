#include "lexer.h"

Lexer::Lexer(const std::string& filename) {
    sourceFile.open(filename);
    if (!sourceFile.is_open()) {
        throw std::runtime_error(
            "Could not open source file: \"" + 
            filename + 
            "\", please make sure if the file is of type \".crunch\".");
    }

    /* 
        //Rulset generation (and Grammar)

        // Keywords
        std::regex keywords( R"(\b(if|else|while|for|break|continue|print|true|false)\b)" );

        // Identifiers
        std::regex identifiers( R"(\b[a-zA-Z][a-zA-Z0-9_-]*\b)" );
        
        // Literals
        std::regex lits( R"(\b(int|double|string|bool)\b)" );

        // Operators
        std::regex ops( R"(\+|\-|\*|\/|%|sin|cos|tan|exp|log|sqrt|deriv|integral)" );

        // Assignment and Comparison
        std::regex assign_comp( "(=|==|!=|<|>|<=|>=|&&|\|\||!)" );

        // Delimiters
        std::regex delims( R"(,|;|:|\.|\(|\)|\{|\})" );

        // Constants
        std::regex consts( R"(\b(pi|euler)\b)" );
    */

};

Lexer::~Lexer() { if (sourceFile.is_open()) sourceFile.close(); }

Token Lexer::tokenize() {
    std::string line;
    while(std::getline(sourceFile, line)) {


        this->ln++;

    }
}

bool Lexer::isEOF() const { return sourceFile.eof(); }