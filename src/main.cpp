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
    
    printf("Get Root\n");

    // Get AST Root
    Program* rootNode = parser->getASTRoot();

    // 1. LLVM Setup
    const std::string moduleName = "CrunchModule";

    codegen_ctx ctx(moduleName);

    if (rootNode) {
        rootNode->codegen(ctx);
    }

    // Print IR Code
    printf("Generated IR Code:\n");
    ctx.module->print(llvm::outs(), nullptr);

    delete lexer;
    delete parser;

    return 0;
}