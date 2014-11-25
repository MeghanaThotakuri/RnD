#ifndef BURG_INCLUDED
#define BURG_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <list>
#include <stack>


using namespace std;

enum assembly_format
{
	op,				/* Only the operator, no operand */
	op_r,			/* No operands, eg. goto L */
	op_r_o1,		/* One operand, eg. op r, o1 */
	op_r_o1_o2,		/* Two operands, eg. op r, o1, o2 */ 
	op_nsy			/* not specified yet */
};

extern void add_op(string ope);

class Instruction
{
public:
	string oper;
	assembly_format af;
	string assembly_mnemonic;
};


extern void add_ins(string t_op, string t_af, string t_am);

class Terminal
{
public:
	string name;		/* terminal name */
	int number;		/* identifying number */
	int arity;		/* operator arity */				
					/**** dont know whether i should add this or not as there is a terminal which has 2 arity's (GOTO) ****/
};


extern void add_terminal(string t_name);


class Nonterminal
{
public:
	string name;		/* terminal name */
	int number;		/* identifying number */
	vector<string> values;  /*values it can take */
};


extern void add_nonterminal(string t_name, std::vector<string> t_values);
extern void add_begin_nonterminal(string t_name);


class State
{
public:
	std::vector<int> cost;
	std::vector<int> rule;
};

class Tree 
{
public:
	string node;		/* non-terminal or terminal which is the root of this tree */
	Tree* left;			/* operands */
	Tree* right;		/* operands */
	string value;		/* the value this node takes. applicable only in case of contant/variable nodes */
	State state;		/* state of automaton */
	list<int> picked_instruction;	/* the instructions picked to generate code at this node */
	vector<int> nt_reqd;			/* the number of non-terminals of each kind required in the tree rooted here*/
	vector<bool> store_reqd;			/* whether this type of nonterminal need to be stored after evaluation of this node*/
	string result;						/* result of the tree evaluated here */
	bool result_computed_by_store;
	bool code_generated;
};

extern Tree * create_tree_m(string t_name, Tree * t_left, Tree * t_right);
extern Tree * create_tree_m(string t_name, Tree * t_left, Tree * t_right, string t_value);

enum Order
{
	result_left_right,
	result_right_left,
	result_left,
	result_right,
	left_right,
	right_left,
	result_node,
	_left,
	_right,
	_result,
	_none
};

class Rule {		/* rules: */
public:
	string lhs;		/* lefthand side non-terminal */
	Tree pattern;	/* rule pattern */
	int number;		/* rule number */
	int cost;		/* associated cost */
	string instr;	/* correspondng machine instruction operator */
	Order args_order;	/* order of arguments of the machine instruction to be generated */
};

extern void create_rule(string t_lhs, Tree * t_tree, int t_cost, std::vector<string> t_order);


#endif
