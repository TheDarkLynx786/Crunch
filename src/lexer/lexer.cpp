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
    
    //_____________________   + - * / %  = == != < > <= >= &&  ||  ! , ; :  .  (  )  {  }
    std::regex specials( R"([\+\-\*\/\%]|=|==|!=|<|>|<=|>=|&&|\|\||!|,|;|:|\.|\(|\)|\{|\})" );

    while(std::getline(sourceFile, line)) {   
            
        this->col = 0; // col reset

        std::cout << "Line " << ln+1 << std::endl;
        
        // Convert line to tokens from ruleset and append to tokens vector
        TokenType tok;

        auto finalize_buffer = [&](std::string &buf) {
            
            if (buf.empty()) return;
            
            TokenType t;
            
            // Literal Check
            if (std::regex_match(buf, int_lit)) t = TokenType::INT_LIT;
            else if (std::regex_match(buf, dble_lit)) t = TokenType::DBLE_LIT;
            else if (std::regex_match(buf, str_lit)) t = TokenType::STR_LIT;
            else if (std::regex_match(buf, bool_lit)) t = TokenType::BOOL_LIT;

            // Identifier Check
            else if (std::regex_match(buf, identifiers) && lex_rules.find(buf) == lex_rules.end()) t = TokenType::IDENTIFIER;
            
            // Final Lex_Rules Check
            else {
                auto it = lex_rules.find(buf);
                t = (it != lex_rules.end()) ? it->second : TokenType::UNKNOWN;
            }

            // Push token to token stream and clear buffer
            Token* new_tok = new Token(t, buf, ln, col);
            std::cout << "Token: " << new_tok->getTypeString() << " | Name: " << new_tok->getLexeme() << std::endl;
            tokens.push_back(new_tok);
            buf.clear();
        };

        std::string buffer;
        bool str_lit_parse = false;
        for (std::size_t i = 0; i < line.size() - 1; ++i) {
            
            //std::cout << "Col " << col+1 << " | Word: \"" << word << "\"" << std::endl;

            char c = line[i];
            std::cout << "Char \"" << c << "\"" << std::endl;

            if (c == '#') {
                // Comment, can safely ignore and break for the line
                break;
            }

            else if (c == '\"') {
                
                if(!str_lit_parse) {
                    str_lit_parse = true;
                    buffer.push_back(c);
                    continue;
                } else if (str_lit_parse) {
                    str_lit_parse = false;
                    buffer.push_back(c);
                    std::cout << "Current buffer " << buffer << std::endl;
                    finalize_buffer(buffer);
                    continue;
                }

            }

            else if (str_lit_parse) {
                buffer.push_back(c);
                continue;
            }

            // Whitespace after string literals
            else if (c == ' ' || c == '\n' || c == '\t') {
                finalize_buffer(buffer);
                continue;
            }

            // single-char operator group that should always be a token by itself
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
                c == ',' || c == ';' || c == ':' || c == '.') 
            {
                finalize_buffer(buffer);
                std::string s(1, c);
                auto it = lex_rules.find(s);
                TokenType t = (it != lex_rules.end()) ? it->second : TokenType::UNKNOWN;
                Token* new_tok = new Token(t, s, ln, col);
                std::cout << "Token: " << new_tok->getTypeString() << " | Name: " << new_tok->getLexeme() << std::endl;
                tokens.push_back(new_tok);
                continue;
            }

            // characters that can start two-char tokens (==, !=, <=, >=, &&, ||, etc.)
            else if (c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|' ||
                c == '(' || c == ')' || c == '{' || c == '}')
            {
                // try two-char lexeme first (if we have room)
                if (i + 1 < line.size()) {
                    std::string two = std::string(1, c);
                    two += line[i + 1];
                    auto it2 = lex_rules.find(two);
                    if (it2 != lex_rules.end()) {
                        // found two-char operator
                        finalize_buffer(buffer);
                        Token* new_tok = new Token(it2->second, two, ln, col);
                        std::cout << "Token: " << new_tok->getTypeString() << " | Name: " << new_tok->getLexeme() << std::endl;
                        tokens.push_back(new_tok);
                        ++i; // consume the second char
                        continue;
                    }
                }

                // fallback to single-character token
                finalize_buffer(buffer);
                std::string s(1, c);
                auto it1 = lex_rules.find(s);
                TokenType t = (it1 != lex_rules.end()) ? it1->second : TokenType::UNKNOWN;
                Token* new_tok = new Token(t, s, ln, col);
                std::cout << "Token: " << new_tok->getTypeString() << " | Name: " << new_tok->getLexeme() << std::endl;
                tokens.push_back(new_tok);
                continue;
            }

            // default: part of an identifier/number/etc.
            buffer.push_back(c);
        
        } // end for

        // final flush of buffer after finishing the line
        if ( !buffer.empty() ) finalize_buffer(buffer);

        this->ln++;
    }

    tokens.push_back( new Token(TokenType::END_OF_FILE, "", ln, col) );
    
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
        std::cout << token->getTypeString() << " ";
    }
    std::cout << std::endl;
}

bool Lexer::isEOF() const { return sourceFile.eof(); }