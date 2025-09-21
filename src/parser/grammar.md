program         -> statement* EOF

statement       -> varDecl,
                   ifStmt,
                   printStmt,
                   breakStmt,
                   continueStmt,
                   exprStmt,
                   block ;

block           -> LBRACE statement* RBRACE

varDecl         -> type IDENTIFIER (ASSIGN expression)? SEMICOL ;

type            -> KW_INT | KW_DBLE | KW_STRING | KW_BOOL ;

ifStmt          -> KW_IF LPAREN expression RPAREN statement (KW_ELSE statement)? ;

printStmt       -> KW_PRINT LPAREN expression RPAREN SEMICOL ;

breakStmt       -> KW_BRK SEMICOL ;
continueStmt    -> KW_CONT SEMICOL ;

exprStmt        -> expression SEMICOL ;

expression      -> logicalOr ;

logicalOr       -> logicalAnd (OR logicalAnd)* ;

logicalAnd      -> equality (AND equality)* ;

equality        -> comparison ((EQ | NEQ) comparison)* ;

comparison      -> term ((LT | GT | LEQ | GEQ) term)* ;

term            -> factor ((PLUS | MINUS) factor)* ;

factor          -> unary ((MULTI | DIV | MOD) unary)* ;

unary           -> (MINUS | NOT | SIN | COS | TAN | LOG | EXP | SQRT) unary| primary ;

primary         ->   INT_LIT
                   | DBLE_LIT
                   | STR_LIT
                   | BOOL_LIT
                   | PI
                   | EULER
                   | IDENTIFIER
                   | LPAREN expression RPAREN ;
