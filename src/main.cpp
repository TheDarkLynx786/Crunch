#include <iostream>
#include "lexer/lexer.h"
#include "parser/parser.h"

int main() {
    std::string src = "src/crunch_files/arithmetic.crunch";
    
    Lexer* lexer;

    try { lexer = new Lexer(src); } 
    catch (const std::runtime_error& e) { std::cerr << e.what() << std::endl; return 1; }

    lexer->tokenize();
    lexer->toString();

    std::vector<Token*> tokens = lexer->getTokens();

    Parser* parser = new Parser(tokens);

    parser->printTree();




    delete lexer;

    return 0;
}