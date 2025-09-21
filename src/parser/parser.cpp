#include "parser.h"

Parser::Parser() {
    current = 0;
    ast_root = new Program();
    
}

Parser::Parser(std::vector<Token*> tokens) {
    this->tokens = tokens;
    current = 0;
    ast_root = parseProgram();
}

Parser::~Parser() {
    for (auto tok : tokens) { delete tok; }
    delete ast_root;
}

// Grammar rule based parsing functions

Program* Parser::parseProgram() {
    Program* program = new Program();
    while(!isAtEnd()) {
        StmtNode* stmt = parseStatement();
        
        if (stmt != nullptr) { program->statements.push_back(stmt); } 
        else advance();
    }
    return program;
}


// Statement returns
StmtNode* Parser::parseStatement() {
    Token* token = peek();
    switch (token->getType()) {
        case TokenType::LBRACE: return parseBlock(); break;
        
        case TokenType::KW_INT: return parseVarDecl(); break;
        case TokenType::KW_DBLE: return parseVarDecl(); break;
        case TokenType::KW_STRING: return parseVarDecl(); break;
        case TokenType::KW_BOOL: return parseVarDecl(); break;
        case TokenType::KW_FUNCTION: return parseVarDecl(); break; // Temp Unsupported
        
        case TokenType::KW_IF: return parseIfStmt(); break;
        case TokenType::KW_PRINT: return parsePrintStmt(); break;
        case TokenType::KW_BRK: return parseBreakStmt(); break; // Temp Unsupported
        case TokenType::KW_CONT: return parseContinueStmt(); break; // Temp Unsupported
        
        default: return parseExprStmt(); break;
    }
}

StmtNode* Parser::parseBlock() {
    std::vector<StmtNode*> stmts;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        stmts.push_back(parseStatement());
    }

    consume(TokenType::RBRACE,"Expected '}' after block");
    
    return new BlockStmt(stmts);
}

StmtNode* Parser::parseVarDecl() {
    Token* typeTok = advance();          
    Token* name = consume(TokenType::IDENTIFIER,"Expected variable name");
    
    ExprNode* initializer = nullptr;
    if ( peek()->getType() == TokenType::ASSIGN ) initializer = parseExpression();
    consume(TokenType::SEMICOL,"Expected ';' after variable declaration");
    return new VarDeclStmt(typeTok->getType(), name->getLexeme(), initializer);
}

StmtNode* Parser::parseIfStmt() {
    consume(TokenType::LPAREN,"Expected '(' after if");
    ExprNode* cond = parseExpression();
    consume(TokenType::RPAREN,"Expected ')'");
    
    StmtNode* thenBranch = parseStatement();
    StmtNode* elseBranch = nullptr;
    
    if (peek()->getType() == TokenType::KW_ELSE) elseBranch = parseStatement();
    return new IfStmt(cond, thenBranch, elseBranch);
}

StmtNode* Parser::parsePrintStmt() {
    ExprNode* value = parseExpression();
    consume(TokenType::SEMICOL,"Expected ';' after print value");
    return new PrintStmt(value);
}

StmtNode* Parser::parseExprStmt() {
    ExprNode* expr = parseExpression();
    consume(TokenType::SEMICOL,"Expected ';' after expression");
    return new ExprStmt(expr);
}

// Expression returns

ExprNode* Parser::parseExpression() { return parseLogicalOr(); }

ExprNode* Parser::parseLogicalOr() {
    ExprNode* expr = parseLogicalAnd();
    while (peek()->getType() == TokenType::OR) {
        Token* op = previous();
        ExprNode* right = parseLogicalAnd();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseLogicalAnd() {
    ExprNode* expr = parseEquality();
    while ( peek()->getType() == TokenType::AND) {
        Token* op = previous();
        ExprNode* right = parseEquality();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseEquality() {
    ExprNode* expr = parseComparison();
    while (peek()->getType() == TokenType::EQ || peek()->getType() == TokenType::NEQ) {
        Token* op = previous();
        ExprNode* right = parseComparison();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseComparison() {
    ExprNode* expr = parseTerm();
    while 
    (
        peek()->getType() == TokenType::LT ||
        peek()->getType() == TokenType::GT ||
        peek()->getType() == TokenType::LEQ ||
        peek()->getType() == TokenType::GEQ
    ) {
        Token* op = previous();
        ExprNode* right = parseTerm();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseTerm() {
    ExprNode* expr = parseFactor();
    while 
    (
        peek()->getType() == TokenType::MINUS ||
        peek()->getType() == TokenType::PLUS
    ) {
        Token* op = previous();
        ExprNode* right = parseFactor();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseFactor() {
    ExprNode* expr = parseUnary();
    while 
    (
        peek()->getType() == TokenType::MULTI ||
        peek()->getType() == TokenType::DIV ||
        peek()->getType() == TokenType::MOD 
    ) {
        Token* op = previous();
        ExprNode* right = parseUnary();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseUnary() {
    if 
    (
        peek()->getType() == TokenType::MINUS ||
        peek()->getType() == TokenType::NOT ||
        peek()->getType() == TokenType::SIN ||
        peek()->getType() == TokenType::COS ||
        peek()->getType() == TokenType::TAN ||
        peek()->getType() == TokenType::LOG ||
        peek()->getType() == TokenType::EXP ||
        peek()->getType() == TokenType::SQRT
    ) {
        Token* op = previous();
        ExprNode* right = parseUnary();
        return new UnaryExpr(op->getLexeme(), right);
    }
    return parsePrimary();
}

ExprNode* Parser::parsePrimary() {
    TokenType tok_type = peek()->getType();
    if (tok_type == TokenType::KW_TRUE)  return new BoolLiteral(true);
    if (tok_type == TokenType::KW_FALSE) return new BoolLiteral(false);
    if (tok_type == TokenType::INT_LIT)  return new IntLiteral(*previous()); // we do a lil dereferencing
    if (tok_type == TokenType::DBLE_LIT) return new DoubleLiteral(*previous());
    if (tok_type == TokenType::STR_LIT)  return new StringLiteral(*previous());
    if (tok_type == TokenType::BOOL_LIT) return new VariableExpr(*previous());
    if (tok_type == TokenType::LPAREN) {
        ExprNode* expr = parseExpression();
        consume(TokenType::RPAREN,"Expected ')'");
        return expr;
    }
    
    // Add at what token later
    throw std::runtime_error("Expected expression");
}