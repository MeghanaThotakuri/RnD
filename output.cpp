#include <iostream>
#include <cstdio>

#include "iburg.h"

#include <limits.h>
#include <stdlib.h>

#define PANIC printf
#define MAX 33333333
#define nonterms 2
#define terms 7

#define begin_nt stmt

string a_nonterminal[] = 
	{
		"stmt",
		"reg"
	};

string a_terminal[] = 
	{
		"ASSGN", 
		"PLUS",
		"MINUS", 
		"MULT", 
		"DIV", 
		"CON", 
		"ADDR"
	};

int nt_values[] =
{
	
}

set<string> nonterminal;

set<string> terminal;

bool isterminal(string s)
{
	return (terminal.count(s)==1);
}

bool isnonterminal(string s)
{
	return (nonterminal.count(s)==1);
}

std::vector<Rule> rules;

Rule create_rule(string t_lhs, Tree t_tree, int t_no, int t_cost, string t_instr, Order t_ao){
	Rule r;
	r.lhs=t_lhs;
	r.pattern = t_tree;
	r.number = t_no;
	r.cost = t_cost;
	r.instr = t_instr;
	r.args_order = t_ao;
	return r;
}

void fill_rules()
{
	Rule strt;
	rules.push_back(strt);
	rules.push_back("stmt",*create_tree("ASSGN",create_tree("ADDR"),create_tree("reg")),1,1,"store",_right_left);
	rules.push_back("reg",*create_tree("PLUS",create_tree("reg"),create_tree("reg")),2,1,"add",_right_left);
	rules.push_back("reg",*create_tree("CON"),5,1,"imm_load",result_left);
}


void state_label(Tree * p)
{
	(p->state).cost.resize(nonterms,MAX);
	(p->state).rule.resize(nonterms,-1);

	string op = p->node;

	int c;

	if(op == "ASSIGN")
	{				/*	stmt: ASSGN(ADDR,reg)	*/
		if(p->left == NULL || p->right == NULL) PANIC("something wrong");
		c = 0 + (p->right->state).cost[1] + 1;
		if(p->left->node == "ADDR"  && c < (p->state).cost[0])
		{
			(p->state).cost[0] = c;
			(p->state).rule[0] = 1;
			closure(&p->state, 0);
		}
	}
	if(op == "PLUS")
	{			/*  reg: PLUS(reg,reg)  */
		if(p->left == NULL || p->right == NULL) PANIC("something wrong");
		c = (p->left->state).cost[1] + (p->right->state).cost[1] + 1;
		if(c < (p->state).cost[1])
		{
			(p->state).cost[1] = c;
			(p->state).rule[1] = 2;
			closure(&p->state, 1);
		}
	}
	if(op == "PLUS")
	{			/*  reg: PLUS(reg,CON)  */
		if(p->left == NULL || p->right == NULL) PANIC("something wrong");
		c = (p->left->state).cost[1] + 0 + 1;
		if(p->right->node=="CON" && c < (p->state).cost[1])
		{
			(p->state).cost[1] = c;
			(p->state).rule[1] = 4;
			closure(&p->state, 1);
		}
	}
	if(op == "MINUS")
	{			/*  reg: MINUS(reg,reg)  */
		if(p->left == NULL || p->right == NULL) PANIC("something wrong");
		c = (p->left->state).cost[1] + (p->right->state).cost[1] + 1;
		if(c < (p->state).cost[1])
		{
			(p->state).cost[1] = c;
			(p->state).rule[1] = 3;
			closure(&p->state, 1);
		}
	}
	if(op == "CON")
	{			/*  reg: CON  */
	
		c = 0 + 0 + 1;
		if(c < (p->state).cost[1])
		{
			(p->state).cost[1] = c;
			(p->state).rule[1] = 5;
			closure(&p->state, 1);
		}
	}
	if(op == "ADDR")
	{			/*  reg: ADDR  */
	
		c = 0 + 0 + 1;
		if(c < (p->state).cost[1])
		{
			(p->state).cost[1] = c;
			(p->state).rule[1] = 6;
			closure(&p->state, 1);
		}
	}
}

void label(Tree * p)
{
	if(p == NULL) PANIC("null tree");
	if(p->left != NULL)	label(p->left);
	if(p->right != NULL) label(p->right);

	state_label(p);
}


void initialize()
{
	nonterminal.insert(a_nonterminal, a_nonterminal+nonterms);
	terminal.insert(a_terminal, a_terminal+terms);
}


Tree * create_tree(string t_name)
{
	Tree * tree = new Tree();
	tree->node = t_name;
	tree->left = NULL;
	tree->right = NULL;
	tree->value = "";
	return tree;
}

Tree * create_tree(string t_name, Tree * t_left, Tree * t_right)
{
	Tree * tree = new Tree();
	tree->node = t_name;
	tree->left = t_left;
	tree->right = t_right;
	tree->value = "";
	return tree;
}


Tree * create_tree(string t_name, Tree * t_left, Tree * t_right,string t_val)
{
	Tree * tree = new Tree();
	tree->node = t_name;
	tree->left = t_left;
	tree->right = t_right;
	tree->value = t_val;
	return tree;
}

void print(Tree * p)
{
	if(p==NULL) return;
	cout<<(p->state).cost[0]<<" "<<(p->state).cost[1]<<endl;
	print(p->left);
	print(p->right);
}

void pick_instr(Tree * p, int result_nt)
{
	int rno = (p->state).rule[result_nt];
	p->picked_instruction = rno;
	Tree t = rules[rno].pattern;
	if(t.left != NULL && isnonterminal(t.left.node)){
		string nt = t.left.node;
		for (int i = 1; i <= nonterms; ++i)
		{
			if(a_nonterminal[i]==nt)
			{
				pick_instr(p->left, i);
			}
		}
	}
	if(t.right != NULL && isnonterminal(t.right.node)){
		string nt = t.right.node;
		for (int i = 1; i <= nonterms; ++i)
		{
			if(a_nonterminal[i]==nt)
			{
				pick_instr(p->right, i);
			}
		}
	}
}

int main() {
	initialize();
	
	Tree * p;

	p = create_tree("ASSIGN",
			create_tree("ADDR", NULL,NULL,"a"),
			create_tree("PLUS", 
				create_tree("CON", NULL,NULL,"b"), 
				create_tree("CON", NULL,NULL,"c")));
				

	label(p);
	print(p);
	pick_instr(p,begin_nt);
	return 0;
}
