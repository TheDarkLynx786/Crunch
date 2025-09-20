#include "lexer.h"

Lexer::Lexer() {
    this->ln = 0;
    this->col = 0;
}


Lexer::Lexer(const std::string& filename) {
    if(filename.substr(filename.find_last_of(".") + 1) != "crunch") {
        throw std::runtime_error(
            "Invalid file type: \"" +
            filename +
            "\" is not of file type \".crunch\"."
        );
    }
    
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

void Lexer::tokenize() {
    std::cout << "Tokenizing..." << std::endl;

    std::string line;
    this->ln = 0; // ln reset
    
    // Dyanmic token ruleset (using regex)
    std::regex identifiers( R"(\b[a-zA-Z][a-zA-Z0-9_-]*\b)" );
    std::regex int_lit( R"(\b(\d+)\b)" );
    std::regex dble_lit( R"(\b(\d+\.\d+)\b)" );
    std::regex str_lit( R"("([^"\\]|\\.)*")" );
    std::regex bool_lit( R"(\b(true|false)\b)" );
    //std::regex function_lit(R"()"); Custom function data types are a whole other beast to tackle, not yet

    while(std::getline(sourceFile, line)) {
        
        std::istringstream iss(line);
        std::string word;
        this->col = 0; // col reset

        std::cout << "Line " << ln+1 << std::endl;

        while( iss >> word ) {
            
            std::cout << "Col " << col+1 << " | Word: \"" << word << "\"" << std::endl;

            if (word == "#") break; // Ignore rest of line; comment
            
            // Convert word to token from ruleset and append to tokens vector

            
            TokenType tok;
            
            // Literal checks
            if (std::regex_match(word, int_lit)) tok = TokenType::INT_LIT;
            else if (std::regex_match(word, dble_lit)) tok = TokenType::DBLE_LIT;
            else if (std::regex_match(word, str_lit)) tok = TokenType::STR_LIT;
            else if (std::regex_match(word, bool_lit)) tok = TokenType::BOOL_LIT;
            // ADD FUNCTION CHECK HERE LATER

            // Identifier check
            else if (std::regex_match(word, identifiers) && lex_rules.find(word) == lex_rules.end()) tok = TokenType::IDENTIFIER;
            
            // Finally index lex_rules
            else if (lex_rules.find(word) != lex_rules.end()) tok = lex_rules[word];
            
            //Unknown check
            else tok = TokenType::UNKNOWN;

            
            Token* new_tok = new Token(tok, ln, col);
            std::cout << "Token: " << new_tok->getLexeme() << std::endl;
            tokens.push_back( new_tok );
            

            // Increase col (1 for space)
            this->col += word.length() + 1; 
        }

        this->ln++;

    }

    tokens.push_back( new Token(TokenType::END_OF_FILE, ln, col) );
}

void Lexer::reset() { 
    if (sourceFile.is_open()) {
        sourceFile.clear();
        sourceFile.seekg(0, std::ios::beg);
    }
    ln = 0;
    col = 0;
    tokens.clear();
}

void Lexer::toString() const {
    for (const auto& token : tokens) {
        std::cout << token->getLexeme() << " ";
    }
    std::cout << std::endl;
}

bool Lexer::isEOF() const { return sourceFile.eof(); }