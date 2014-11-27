%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	std::string * string_value;
	std::vector<std::string> * string_vect;
	Tree * tree;
};


%token <integer_value> NUM
%token <string_value> ID
%token PPERCENT

%type <string_vect> values
%type <string_vect> nontermvalues
%type <integer_value> cost
%type <tree> tree;


%start gramm

%%


gramm	: 	PPERCENT ops {  }
			PPERCENT instypes {  }
			PPERCENT nonterms {  }
			PPERCENT terms {  }
			PPERCENT rules {  }
			PPERCENT {  }
		;


ops : 
		|	ops ID 	{	add_op(*$2);	}
		;

instypes :
		| instypes ID '=' ID ',' ID {	add_ins(*$2, *$4, *$6); }
		;

nonterms :
		| nonterms ID '=' ID 
			{
				if((*$4) != "BEGIN")
				{
					cout<<"wrong value to nonterminal : "<<*$4<<endl;
					exit(1);
				}
				add_begin_nonterminal(*$2);
			}
		| nonterms ID '=' nontermvalues 
			{
				add_nonterminal(*$2, *$4);
			}
		;

nontermvalues :
		'{' values '}' {	$$ = $2;	}
		|
		'{' '}' 	{ $$ = new vector<string>(); }
		;


terms : ID
			{
				add_terminal(*$1);
			}
		|
		terms ',' ID
			{
				add_terminal(*$3);
			}
		;


rules : 
		| rules ID ':' tree '=' cost '{' values '}' 
			{
				create_rule(*$2, $4, $6, *$8);
			}
		;


values : ID 
			{	std::vector<string> * vs = new vector<string>();
				vs->push_back(*$1);
				$$ = vs;
			}
		| 
		values ',' ID 
			{
				$$ = $1;
				$$->push_back(*$3);
			}
		;


cost	: /* lambda */	{ $$ = 1;}
	| '(' NUM ')' { $$ = ($2)*10;}
	;


tree	: ID                      {	$$ = create_tree_m(*$1, NULL, NULL);	}
	| ID '(' tree ')'               {	$$ = create_tree_m(*$1, $3, NULL); }
	| ID '(' tree ',' tree ')'      {	$$ = create_tree_m(*$1, $3, $5); }
	;
