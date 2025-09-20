#pragma once

#include <iostream>
#include <fstream>
#include "token.h"

class Lexer {
    private: 
        std::ifstream sourceFile;
    public:
        Lexer(const std::string& filename);
        
        ~Lexer();
        
        Token getNextToken();

        void reset();

        bool isEOF() const;

        
};