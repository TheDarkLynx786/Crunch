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
    
    consume(TokenType::LBRACE, "Expected \"{\" character before block");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        StmtNode* stmt = parseStatement();
        if(stmt) stmts.push_back(stmt); //Safety Check
    }

    consume(TokenType::RBRACE,"Expected '}' after block");
    
    return new BlockStmt(stmts);
}

StmtNode* Parser::parseVarDecl() {
    Token* typeTok = advance();          
    Token* name = consume(TokenType::IDENTIFIER,"Expected variable name");
    
    ExprNode* initializer = nullptr;
    if ( peek()->getType() == TokenType::ASSIGN ) {
        advance(); // Potential Bug
        initializer = parseExpression();
    }
    consume(TokenType::SEMICOL,"Expected ';' after variable declaration");
    return new VarDeclStmt(typeTok->getType(), name->getLexeme(), initializer);
}

StmtNode* Parser::parseIfStmt() {
    consume(TokenType::KW_IF, "Expected if statement");
    
    consume(TokenType::LPAREN,"Expected '(' after if");
    ExprNode* cond = parseExpression();
    consume(TokenType::RPAREN,"Expected ')'");
    
    StmtNode* thenBranch = parseStatement();
    StmtNode* elseBranch = nullptr;
    
    if (peek()->getType() == TokenType::KW_ELSE) {
        advance(); // Potential Bug
        elseBranch = parseStatement();
    }
    return new IfStmt(cond, thenBranch, elseBranch);
}

StmtNode* Parser::parsePrintStmt() {
    consume(TokenType::KW_PRINT, "Expected \"print\" statement.");
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

ExprNode* Parser::parseExpression() { return parseComma(); }

ExprNode* Parser::parseComma() {
    ExprNode* expr = parseAssignment();
    while ( peek()->getType() == TokenType::COMMA) {
        Token* op = advance();
        ExprNode* right = parseAssignment();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseAssignment() {
    ExprNode* expr = parseLogicalOr();

    // Note: using while makes operator left-associative
    // Assignment is right-associative, so this uses if
    if (peek()->getType() == TokenType::ASSIGN) {
        Token* op = advance();
        ExprNode* value = parseAssignment(); // right-associative

        // Ensure the LHS is a valid assignment target
        if (auto var = dynamic_cast<IdentifierExpr*>(expr)) {
            return new AssignmentExpr(value, var->name);
        } else {
            throw std::runtime_error("Invalid assignment target.");
        }
    }

    return expr;
}

ExprNode* Parser::parseLogicalOr() {
    ExprNode* expr = parseLogicalAnd();
    while (peek()->getType() == TokenType::OR) {
        Token* op = advance();
        ExprNode* right = parseLogicalAnd();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseLogicalAnd() {
    ExprNode* expr = parseEquality();
    while ( peek()->getType() == TokenType::AND) {
        Token* op = advance(); //Token* op = previous(); | Potential Bug
        ExprNode* right = parseEquality();
        expr = new BinaryExpr(expr, op->getLexeme(), right);
    }
    return expr;
}

ExprNode* Parser::parseEquality() {
    ExprNode* expr = parseComparison();
    while (peek()->getType() == TokenType::EQ || peek()->getType() == TokenType::NEQ) {
        Token* op = advance(); //Token* op = previous(); | Potential Bug
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
        Token* op = advance(); //Token* op = previous(); | Potential Bug
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
        Token* op = advance(); //Token* op = previous(); | Potential Bug
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
        Token* op = advance(); //Token* op = previous(); | Potential Bug
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
        Token* op = advance(); //Token* op = previous(); | Potential Bug
        ExprNode* right = parseUnary();
        return new UnaryExpr(op->getLexeme(), right);
    }
    return parsePrimary();
}

ExprNode* Parser::parsePrimary() {
    TokenType tok_type = peek()->getType();
    if (tok_type == TokenType::KW_TRUE)  return new BoolLiteral(true);
    if (tok_type == TokenType::KW_FALSE) return new BoolLiteral(false);
    if (tok_type == TokenType::INT_LIT)  return new IntLiteral(*advance()); // we do a lil dereferencing | USED TO BE "previous()," Potential Bug
    if (tok_type == TokenType::DBLE_LIT) return new DoubleLiteral(*advance());
    if (tok_type == TokenType::STR_LIT)  return new StringLiteral(*advance());
    if (tok_type == TokenType::BOOL_LIT) return new BoolLiteral(true); // placeholder handling for BOOL_LIT
    if (tok_type == TokenType::IDENTIFIER) return new IdentifierExpr(*advance());

    if (tok_type == TokenType::LPAREN) {
        advance(); // Potential Bug
        ExprNode* expr = parseExpression();
        consume(TokenType::RPAREN,"Expected ')'");
        return expr;
    }
    
    // Add at what token later
    throw std::runtime_error("Expected expression");
}

// --- AST printing helpers (file-local) ---
namespace {
    void printIndent(int indent) {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    }

    void printExprNode(ExprNode* expr, int indent);
    void printStmtNode(StmtNode* stmt, int indent);

    void printExprNode(ExprNode* expr, int indent) {
        if (!expr) { printIndent(indent); std::cout << "<null expr>\n"; return; }

        if (auto b = dynamic_cast<BinaryExpr*>(expr)) {
            printIndent(indent); std::cout << "BinaryExpr op='" << b->op << "'\n";
            printExprNode(b->left, indent + 1);
            printExprNode(b->right, indent + 1);
            return;
        }
        if (auto u = dynamic_cast<UnaryExpr*>(expr)) {
            printIndent(indent); std::cout << "UnaryExpr op='" << u->op << "'\n";
            printExprNode(u->operand, indent + 1);
            return;
        }
        if (auto lit = dynamic_cast<LiteralExpr*>(expr)) {
            printIndent(indent); std::cout << "LiteralExpr value='" << lit->value << "'\n";
            return;
        }
        if (auto id = dynamic_cast<IdentifierExpr*>(expr)) {
            printIndent(indent); std::cout << "IdentifierExpr name='" << id->name << "'\n";
            return;
        }
        if (auto c = dynamic_cast<CallExpr*>(expr)) {
            printIndent(indent); std::cout << "CallExpr\n";
            printIndent(indent+1); std::cout << "Callee:\n";
            printExprNode(c->callee, indent + 2);
            printIndent(indent+1); std::cout << "Args:\n";
            for (auto a : c->args) printExprNode(a, indent + 2);
            return;
        }
        if (auto b = dynamic_cast<BoolLiteral*>(expr)) {
            printIndent(indent); std::cout << "BoolLiteral " << (b->value ? "true" : "false") << "\n"; return;
        }
        if (auto i = dynamic_cast<IntLiteral*>(expr)) {
            printIndent(indent); std::cout << "IntLiteral " << i->value << "\n"; return;
        }
        if (auto d = dynamic_cast<DoubleLiteral*>(expr)) {
            printIndent(indent); std::cout << "DoubleLiteral " << d->value << "\n"; return;
        }
        if (auto s = dynamic_cast<StringLiteral*>(expr)) {
            printIndent(indent); std::cout << "StringLiteral '" << s->value << "'\n"; return;
        }
        if (auto a = dynamic_cast<AssignmentExpr*>(expr)) {
            
            std::string value = "default";

            if (auto b = dynamic_cast<BoolLiteral*>(a->expr)) {
                value = std::to_string(b->value);
            }
            if (auto i = dynamic_cast<IntLiteral*>(a->expr)) {
                value = std::to_string(i->value);
            }
            if (auto d = dynamic_cast<DoubleLiteral*>(a->expr)) {
                value = std::to_string(d->value);
            }
            if (auto s = dynamic_cast<StringLiteral*>(a->expr)) {
                value = s->value;
            }
            
            printIndent(indent); std::cout << "AssignmentExpr '" << a->name << "' to '" << value << "'\n"; return;
        }

        printIndent(indent); std::cout << "<unknown ExprNode>\n";
    }

    void printStmtNode(StmtNode* stmt, int indent) {
        if (!stmt) { printIndent(indent); std::cout << "<null stmt>\n"; return; }

        if (auto es = dynamic_cast<ExprStmt*>(stmt)) {
            printIndent(indent); std::cout << "ExprStmt\n";
            printExprNode(es->expr, indent + 1);
            return;
        }
        if (auto vd = dynamic_cast<VarDeclStmt*>(stmt)) {
            printIndent(indent); std::cout << "VarDeclStmt type=" << static_cast<int>(vd->type) << " name='" << vd->name << "'\n";
            if (vd->init) printExprNode(vd->init, indent + 1);
            return;
        }
        if (auto bs = dynamic_cast<BlockStmt*>(stmt)) {
            printIndent(indent); std::cout << "BlockStmt\n";
            for (auto s : bs->statements) printStmtNode(s, indent + 1);
            return;
        }
        if (auto ifs = dynamic_cast<IfStmt*>(stmt)) {
            printIndent(indent); std::cout << "IfStmt\n";
            printIndent(indent+1); std::cout << "Condition:\n";
            printExprNode(ifs->condition, indent + 2);
            printIndent(indent+1); std::cout << "Then:\n";
            printStmtNode(ifs->thenBranch, indent + 2);
            if (ifs->elseBranch) {
                printIndent(indent+1); std::cout << "Else:\n";
                printStmtNode(ifs->elseBranch, indent + 2);
            }
            return;
        }
        if (auto ps = dynamic_cast<PrintStmt*>(stmt)) {
            printIndent(indent); std::cout << "PrintStmt\n";
            printExprNode(ps->value, indent + 1);
            return;
        }

        printIndent(indent); std::cout << "<unknown StmtNode>\n";
    }

    void printProgram(Program* prog, int indent) {
        if (!prog) { printIndent(indent); std::cout << "<null program>\n"; return; }
        printIndent(indent); std::cout << "Program\n";
        for (auto s : prog->statements) printStmtNode(s, indent + 1);
    }
}

// Implement Parser::printTree to use the helpers
void Parser::printTree() {
    printProgram(ast_root, 0);
}