
%scanner /scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%token NUM

%start Start

%left '+'
%left '*'

%%

Start   : Expr '\n'		{ cout << "\t= " << $1 << "\n"; }
        ;
Expr 	: Expr '+'  Expr	{ $$ = $1 + $3; }
	| Expr '*' Expr		{ $$ = $1 * $3; }
	| NUM	                { $$ = $1;	} 
	;
