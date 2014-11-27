#include <iostream>
#include <cstdio>
#include "iburg.h"
#include "memorylocation.h"

#include <limits.h>
#include <stdlib.h>
#define PANIC printf 

#define MAX 33333333
#define nonterms 5
#define terms 19

#define nt_stmt 1
#define nt_reg 2
#define nt_regf 3
#define nt_cndtnl_goto 4
#define nt_uncndtnl_goto 5

int begin_nt = 1;
memorylocation memloc;

Tree * create_tree(string t_name){
	Tree * tree = new Tree();
	tree->node = t_name;
	tree->left = NULL;
	tree->right = NULL;
	tree->value = "";
	return tree;
}

Tree * create_tree(string t_name, Tree * t_left, Tree * t_right){
	Tree * tree = new Tree();
	tree->node = t_name;
	tree->left = t_left;
	tree->right = t_right;
	tree->value = "";
	return tree;
}

Tree * create_tree(string t_name, Tree * t_left, Tree * t_right,string t_val){
Tree * tree = new Tree();
	tree->node = t_name;
 tree->left = t_left;
 tree->right = t_right;
 tree->value = t_val;
 return tree;
}


map<string, Instruction> instruction_map;
vector<stack<string> > nonterminal_values;
Instruction create_instruction(string t_op, assembly_format t_af, string t_am){
		Instruction ins;
		ins.oper = t_op;
		ins.af = t_af;
		ins.assembly_mnemonic = t_am;
		instruction_map.insert(make_pair(t_op,ins));
		return ins;
}
string a_nonterminal[] = 
 {
 	 "" ,
 	 "stmt" ,
 	 "reg" ,
 	 "regf" ,
 	 "cndtnl_goto" ,
 	 "uncndtnl_goto" ,
 };

string a_terminal[] = 
{
	"",
	"ASSGN",
	"PLUS",
	"MINUS",
	"MULT",
	"DIV",
	"UMINUS",
	"LT",
	"GT",
	"NE",
	"GE",
	"LE",
	"EQ",
	"CON",
	"CONF",
	"ADDR",
	"INT",
	"FLOAT",
	"IF",
	"GOTO",
};

int a_nt_values_count[]=
{
	0, 	0, 	10, 	6, 	0, 	0, 
};
void fill_nonterminals(){
	nonterminal_values.resize(nonterms+1);
	nonterminal_values[2].push("r10");
	nonterminal_values[2].push("r9");
	nonterminal_values[2].push("r8");
	nonterminal_values[2].push("r7");
	nonterminal_values[2].push("r6");
	nonterminal_values[2].push("r5");
	nonterminal_values[2].push("r4");
	nonterminal_values[2].push("r3");
	nonterminal_values[2].push("r2");
	nonterminal_values[2].push("r1");
	nonterminal_values[3].push("f6");
	nonterminal_values[3].push("f5");
	nonterminal_values[3].push("f4");
	nonterminal_values[3].push("f3");
	nonterminal_values[3].push("f2");
	nonterminal_values[3].push("f1");
}

void fill_instructions(){
	create_instruction("add", (assembly_format)3,"add");
	create_instruction("add_d", (assembly_format)3,"add.d");
	create_instruction("div", (assembly_format)3,"div");
	create_instruction("div_d", (assembly_format)3,"div.d");
	create_instruction("goto_type", (assembly_format)1,"j");
	create_instruction("if_else", (assembly_format)2,"bne");
	create_instruction("imm_load", (assembly_format)2,"li");
	create_instruction("imm_load_d", (assembly_format)2,"li.d");
	create_instruction("load", (assembly_format)2,"lw");
	create_instruction("load_d", (assembly_format)2,"lw.d");
	create_instruction("mfc1", (assembly_format)2,"mfc");
	create_instruction("move", (assembly_format)2,"mv");
	create_instruction("move_d", (assembly_format)2,"mv.d");
	create_instruction("mtc1", (assembly_format)2,"mtc");
	create_instruction("mul", (assembly_format)3,"mul");
	create_instruction("mul_d", (assembly_format)3,"mul.d");
	create_instruction("no_instruction", (assembly_format)4,"_");
	create_instruction("seq", (assembly_format)3,"seq");
	create_instruction("sge", (assembly_format)3,"sge");
	create_instruction("sgt", (assembly_format)3,"sgt");
	create_instruction("sle", (assembly_format)3,"sle");
	create_instruction("slt", (assembly_format)3,"slt");
	create_instruction("sne", (assembly_format)3,"sne");
	create_instruction("store", (assembly_format)2,"sw");
	create_instruction("store_d", (assembly_format)2,"sw.d");
	create_instruction("sub", (assembly_format)3,"sub");
	create_instruction("sub_d", (assembly_format)3,"sub.d");
	create_instruction("uminus", (assembly_format)2,"neg");
	create_instruction("uminus_d", (assembly_format)2,"neg.d");
}

set<string> nonterminal;
set<string> terminal;
map<string, int> nonterminal_map;
bool isterminal(string s){	return (terminal.count(s)==1); }
bool isnonterminal(string s){	return (nonterminal.count(s)==1); }

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

void fill_rules(){
	Rule strt;
	rules.push_back(strt);
	rules.push_back(create_rule("stmt",*create_tree("IF",create_tree("cndtnl_goto"),create_tree("uncndtnl_goto")),1,10,"no_instruction",(Order)10));
	rules.push_back(create_rule("cndtnl_goto",*create_tree("GOTO",create_tree("reg"),create_tree("CON")),2,10,"if_else",(Order)4));
	rules.push_back(create_rule("uncndtnl_goto",*create_tree("GOTO",create_tree("CON"),NULL),3,10,"goto_type",(Order)7));
	rules.push_back(create_rule("stmt",*create_tree("ASSGN",create_tree("ADDR"),create_tree("reg")),4,10,"store",(Order)5));
	rules.push_back(create_rule("stmt",*create_tree("ASSGN",create_tree("ADDR"),create_tree("regf")),5,10,"store_d",(Order)5));
	rules.push_back(create_rule("reg",*create_tree("PLUS",create_tree("reg"),create_tree("reg")),6,10,"add",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("MINUS",create_tree("reg"),create_tree("reg")),7,10,"sub",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("MULT",create_tree("reg"),create_tree("reg")),8,10,"mul",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("DIV",create_tree("reg"),create_tree("reg")),9,10,"div",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("UMINUS",create_tree("reg"),NULL),10,10,"uminus",(Order)2));
	rules.push_back(create_rule("regf",*create_tree("PLUS",create_tree("regf"),create_tree("regf")),11,10,"add_d",(Order)0));
	rules.push_back(create_rule("regf",*create_tree("MINUS",create_tree("regf"),create_tree("regf")),12,10,"sub_d",(Order)0));
	rules.push_back(create_rule("regf",*create_tree("MULT",create_tree("regf"),create_tree("regf")),13,10,"mul_d",(Order)0));
	rules.push_back(create_rule("regf",*create_tree("DIV",create_tree("regf"),create_tree("regf")),14,10,"div_d",(Order)0));
	rules.push_back(create_rule("regf",*create_tree("UMINUS",create_tree("regf"),NULL),15,10,"uminus_d",(Order)2));
	rules.push_back(create_rule("reg",*create_tree("LT",create_tree("reg"),create_tree("reg")),16,10,"slt",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("GT",create_tree("reg"),create_tree("reg")),17,10,"sgt",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("LE",create_tree("reg"),create_tree("reg")),18,10,"sle",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("GE",create_tree("reg"),create_tree("reg")),19,10,"sge",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("NE",create_tree("reg"),create_tree("reg")),20,10,"sne",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("EQ",create_tree("reg"),create_tree("reg")),21,10,"seq",(Order)0));
	rules.push_back(create_rule("reg",*create_tree("CON"),22,10,"imm_load",(Order)6));
	rules.push_back(create_rule("regf",*create_tree("CONF"),23,10,"load_d",(Order)6));
	rules.push_back(create_rule("regf",*create_tree("FLOAT",create_tree("reg"),NULL),24,10,"mtc1",(Order)2));
	rules.push_back(create_rule("reg",*create_tree("INT",create_tree("regf"),NULL),25,10,"mfc1",(Order)2));
	rules.push_back(create_rule("reg",*create_tree("ADDR"),26,10,"load",(Order)6));
	rules.push_back(create_rule("regf",*create_tree("ADDR"),27,10,"load_d",(Order)0));
}

void initialize(){
	nonterminal.insert(a_nonterminal, a_nonterminal+nonterms+1);
for (int i = 1; i <= nonterms; ++i)	nonterminal_map.insert(make_pair(a_nonterminal[i],i));
	terminal.insert(a_terminal, a_terminal+terms+1);
	fill_rules();
	fill_nonterminals();
	fill_instructions();
}



void state_label(Tree * p){
(p->state).cost.resize(nonterms+1,MAX);
(p->state).rule.resize(nonterms+1,-1);
string op = p->node;
int c;

if(op=="IF"){
	c=(p->left->state).cost[nt_cndtnl_goto] + (p->right->state).cost[nt_uncndtnl_goto] + 10;
	if(c < (p->state).cost[nt_stmt]){
		(p->state).cost[nt_stmt] = c;
		(p->state).rule[nt_stmt] = 1;
	}
}
if(op=="GOTO"){
	c=(p->left->state).cost[nt_reg] + 0 + 10;
	if(p->right->node == "CON" && c < (p->state).cost[nt_cndtnl_goto]){
		(p->state).cost[nt_cndtnl_goto] = c;
		(p->state).rule[nt_cndtnl_goto] = 2;
	}
}
if(op=="GOTO"){
	c=0 + 0 + 10;
	if(p->left->node == "CON" && c < (p->state).cost[nt_uncndtnl_goto]){
		(p->state).cost[nt_uncndtnl_goto] = c;
		(p->state).rule[nt_uncndtnl_goto] = 3;
	}
}
if(op=="ASSGN"){
	c=0 + (p->right->state).cost[nt_reg] + 10;
	if(p->left->node == "ADDR" && c < (p->state).cost[nt_stmt]){
		(p->state).cost[nt_stmt] = c;
		(p->state).rule[nt_stmt] = 4;
	}
}
if(op=="ASSGN"){
	c=0 + (p->right->state).cost[nt_regf] + 10;
	if(p->left->node == "ADDR" && c < (p->state).cost[nt_stmt]){
		(p->state).cost[nt_stmt] = c;
		(p->state).rule[nt_stmt] = 5;
	}
}
if(op=="PLUS"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 6;
	}
}
if(op=="MINUS"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 7;
	}
}
if(op=="MULT"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 8;
	}
}
if(op=="DIV"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 9;
	}
}
if(op=="UMINUS"){
	c=(p->left->state).cost[nt_reg] + 0 + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 10;
	}
}
if(op=="PLUS"){
	c=(p->left->state).cost[nt_regf] + (p->right->state).cost[nt_regf] + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 11;
	}
}
if(op=="MINUS"){
	c=(p->left->state).cost[nt_regf] + (p->right->state).cost[nt_regf] + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 12;
	}
}
if(op=="MULT"){
	c=(p->left->state).cost[nt_regf] + (p->right->state).cost[nt_regf] + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 13;
	}
}
if(op=="DIV"){
	c=(p->left->state).cost[nt_regf] + (p->right->state).cost[nt_regf] + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 14;
	}
}
if(op=="UMINUS"){
	c=(p->left->state).cost[nt_regf] + 0 + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 15;
	}
}
if(op=="LT"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 16;
	}
}
if(op=="GT"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 17;
	}
}
if(op=="LE"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 18;
	}
}
if(op=="GE"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 19;
	}
}
if(op=="NE"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 20;
	}
}
if(op=="EQ"){
	c=(p->left->state).cost[nt_reg] + (p->right->state).cost[nt_reg] + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 21;
	}
}
if(op=="CON"){
	c=0 + 0 + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 22;
	}
}
if(op=="CONF"){
	c=0 + 0 + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 23;
	}
}
if(op=="FLOAT"){
	c=(p->left->state).cost[nt_reg] + 0 + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 24;
	}
}
if(op=="INT"){
	c=(p->left->state).cost[nt_regf] + 0 + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 25;
	}
}
if(op=="ADDR"){
	c=0 + 0 + 10;
	if(c < (p->state).cost[nt_reg]){
		(p->state).cost[nt_reg] = c;
		(p->state).rule[nt_reg] = 26;
	}
}
if(op=="ADDR"){
	c=0 + 0 + 10;
	if(c < (p->state).cost[nt_regf]){
		(p->state).cost[nt_regf] = c;
		(p->state).rule[nt_regf] = 27;
	}
}
}

void label(Tree * p){
	if(p == NULL) PANIC("null tree");
	if(p->left != NULL) label(p->left);
	if(p->right != NULL) label(p->right);
	state_label(p);
}
void pick_instr(Tree * p, int result_nt){
	int rno = (p->state).rule[result_nt];
	p->picked_instruction.push_back(rno);
	Tree t = rules[rno].pattern;
if(t.node == p->node){
	if(t.left != NULL && isnonterminal(t.left->node)){
	string nt = t.left->node;
	for (int i = 1; i <= nonterms; ++i){
		if(a_nonterminal[i]==nt){ pick_instr(p->left, i); break;}
	}
	}
	if(t.right != NULL && isnonterminal(t.right->node)){
	string nt = t.right->node;
	for (int i = 1; i <= nonterms; ++i){
		if(a_nonterminal[i]==nt){pick_instr(p->right, i); break;}
	}
	}
}else{
	for (int i = 1; i <= nonterms; ++i){
		if(t.node == a_nonterminal[i]) {pick_instr(p,i); return;}
	}
}
}

void su_number(Tree * p)
{
	p->nt_reqd.resize(nonterms+1,0);
	p->store_reqd.resize(nonterms+1,false);
	int rno=0;
    for (std::list<int>::iterator i = p->picked_instruction.begin(); i != p->picked_instruction.end(); ++i) rno = *i;
    if(rno == 0) return;
	Tree t = rules[rno].pattern;
	if(t.left==NULL && t.right==NULL){
		for (int i = 1; i <= nonterms; ++i){
			if(a_nonterminal[i] == rules[rno].lhs) p->nt_reqd[i]=1;
			else if(t.node == a_nonterminal[i]) p->nt_reqd[i]=1;
		}
	}
	else if(t.right==NULL){
		su_number(p->left);
		for (int i = 1; i <= nonterms; ++i){
			p->nt_reqd[i]=p->left->nt_reqd[i];
			if(rules[rno].lhs == a_nonterminal[i] && p->nt_reqd[i]==0) p->nt_reqd[i]++; 
		}
	}
	else{
		su_number(p->left);
		su_number(p->right);
		for (int i = 1; i <= nonterms; ++i){
			p->nt_reqd[i] = max(p->left->nt_reqd[i], p->right->nt_reqd[i]);
			if(p->left->nt_reqd[i] == p->right->nt_reqd[i] && p->left->nt_reqd[i] != 0) {
				p->nt_reqd[i]++;
				if(a_nt_values_count[i] < p->nt_reqd[i]) {p->left->store_reqd[i] = true; p->nt_reqd[i]--; p->left->nt_reqd[i]=1;}
			}
			if(rules[rno].lhs == a_nonterminal[i] && p->nt_reqd[i]==0) p->nt_reqd[i]++;
		}
	}

	for (std::list<int>::iterator i = p->picked_instruction.begin(); i != p->picked_instruction.end(); ++i)	{
   		if(*i == rno) break;
   		for (int j = 1; j <= nonterms; ++j){
   			if(a_nonterminal[j]==rules[*i].lhs && p->nt_reqd[j]==0) p->nt_reqd[j]++;
   		}
   	}
}

string get_val(string nt_type)
{
	if(nonterminal_values[nonterminal_map[nt_type]].empty()) {cout<<"errr : no available nonterminals of type "<<nt_type<<endl; exit(0);}
	string ret = nonterminal_values[nonterminal_map[nt_type]].top();
	nonterminal_values[nonterminal_map[nt_type]].pop();
	return ret;
}

void release_nt_val(string val, string nt_type)
{
	if(val == "" || !isnonterminal(nt_type)) return;
	nonterminal_values[nonterminal_map[nt_type]].push(val);
}

void gencode1(Tree * p)
{
	if(p==NULL) return;
	bool startone = true;
	int rno=0;
	if(p==NULL) return;
	else if(p->picked_instruction.empty()) {p->result = p->value; p->code_generated=true; return;}
	else if(p->result_computed_by_store == true){
		string res_val = get_val(rules[*(p->picked_instruction.begin())].lhs);
		cout<<"mv "<<res_val<<", "<<p->result<<endl;
		memloc.free_memory_location(p->result);
		p->result = res_val;
		p->result_computed_by_store = false;
		return;
	}
	for (std::list<int>::reverse_iterator rit=p->picked_instruction.rbegin(); rit!=p->picked_instruction.rend(); ++rit){
		Rule r = rules[*rit];
		if(startone == true){
			startone = false;
			if(r.args_order==_none)
			{
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				cout<<instruction_map[r.instr].assembly_mnemonic<<endl;
				p->result="";
			}
			else if(r.args_order==_result)
			{
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				p->result = get_val(r.lhs);
				cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->result<<endl;	
			}
			else if(r.args_order==_left)
			{
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				p->result = "";
				cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->left->result<<endl;	
			}
			else if(r.args_order==_right)
			{
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				p->result = "";
				cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->right->result<<endl;	
			}
			else if(r.args_order==result_node)
			{
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				p->result = get_val(r.lhs);
				cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->result<<", "<<p->value<<endl;
			}
			else if(r.args_order==result_left)
			{
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				p->result = get_val(r.lhs);
				cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->result<<", "<<p->left->result<<endl;
			}
			else if(r.args_order==result_right)
			{
				gencode1(p->left);
				if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
					release_nt_val(p->left->result, r.pattern.left->node); 
				gencode1(p->right);
				if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
					release_nt_val(p->right->result, r.pattern.right->node); 
				p->result = get_val(r.lhs);
				cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->result<<", "<<p->right->result<<endl;
			}
			else if(r.args_order==right_left || r.args_order==left_right || r.args_order==result_left_right || r.args_order==result_right_left)
			{
				if(r.pattern.left->node == r.pattern.right->node)
				{
					int nt_index = nonterminal_map[r.pattern.left->node];
					if(p->left->nt_reqd[nt_index] < p->right->nt_reqd[nt_index]){
					gencode1(p->right);
					gencode1(p->left);
					if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
						release_nt_val(p->left->result, r.pattern.left->node); 
					if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
						release_nt_val(p->right->result, r.pattern.right->node);
					}
					else{
					gencode1(p->left);
					gencode1(p->right);
					if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
						release_nt_val(p->right->result, r.pattern.right->node); 
					if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
						release_nt_val(p->left->result, r.pattern.left->node); 
					}
				}
				else
				{
					gencode1(p->left);
					gencode1(p->right);
					if(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))
						release_nt_val(p->right->result, r.pattern.right->node); 
					if(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))
						release_nt_val(p->left->result, r.pattern.left->node); 
				}

				if(r.args_order == right_left){
					p->result = "";
					cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->right->result<<", "<<p->left->result<<endl;
				}
				else if(r.args_order==left_right)
				{
					p->result = "";
					cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->left->result<<", "<<p->right->result<<endl;
				}
				else if(r.args_order==result_left_right)
				{
					p->result = get_val(r.lhs);
					cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->result<<", "<<p->left->result<<", "<<p->right->result<<endl;
				}
				else if(r.args_order==result_right_left)
				{
					p->result = get_val(r.lhs);
					cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<p->result<<", "<<p->right->result<<", "<<p->left->result<<endl;
				}
			}
		}
		else{
			string res_val = get_val(r.lhs);
			cout<<instruction_map[r.instr].assembly_mnemonic<<" "<<res_val<<", "<<p->result<<endl;
			release_nt_val(p->result, r.pattern.node); 
			p->result = res_val;
		}
	}
}

void gencode_store(Tree * p)
{
	if(p->result_computed_by_store == false) {
		gencode1(p);
		string res_mem_val = memloc.get_memory_location(nonterminal_map[rules[*(p->picked_instruction.begin())].lhs]);
		cout<<"mv "<<res_mem_val<<", "<<p->result<<endl;
		release_nt_val(p->result, rules[*(p->picked_instruction.begin())].lhs); 
		p->result = res_mem_val;
		p->result_computed_by_store = true;
	}
}

void check_and_generate_stores(Tree * p)
{
	if(p==NULL) return;
	p->result_computed_by_store = false;
	p->code_generated = false;
	check_and_generate_stores(p->left);
	check_and_generate_stores(p->right);
	for (int i = 1; i <= nonterms; ++i)
		if(p->store_reqd[i]) gencode_store(p);
}

void gencode(Tree * p)
{
	check_and_generate_stores(p);
	gencode1(p);
}


void print(Tree * p)
{
	if(p==NULL) return;
	cout<<(p->state).cost[0]<<" "<<(p->state).cost[1]<<" "<<(p->state).cost[2];
	cout<<"\t"<<(p->state).rule[0]<<" "<<(p->state).rule[1]<<" "<<(p->state).rule[2]<<endl;
	print(p->left);
	print(p->right);
}


void printin(Tree * p)
{
	if(p==NULL) return;
	cout<<p->node<<"\t";
	for (std::list<int>::iterator i = p->picked_instruction.begin(); i != p->picked_instruction.end(); ++i)
	{
		cout<<*i<<" ";
	}
	cout<<endl;
	printin(p->left);
	printin(p->right);
}

void printsu(Tree * p)
{
	if(p==NULL) return;
	cout<<p->node<<"\t";
	for (int i = 1; i <= nonterms; ++i)
	{
		cout<<" "<<p->nt_reqd[i];
	}
	cout<<"\t\t";
	for (int i = 1; i <= nonterms; ++i)
	{
		cout<<" "<<p->store_reqd[i];
	}
	cout<<endl;
	printsu(p->left);
	printsu(p->right);
}

int main() {
	initialize();
	Tree * p;
	
	p = create_tree("ASSGN",
			create_tree("ADDR",NULL,NULL,"fp[10]"), 
			create_tree("PLUS", 
				create_tree("PLUS", 
					create_tree("PLUS", 
						create_tree("CON", NULL,NULL,"3"), 
						create_tree("CON", NULL,NULL,"4")), 
					create_tree("PLUS", 
						create_tree("ADDR", NULL,NULL,"a"), 
						create_tree("CON", NULL,NULL,"6"))),
				create_tree("UMINUS",
					create_tree("INT",
						create_tree("FLOAT",
							create_tree("ADDR",NULL,NULL,"af"),
							NULL),
						NULL),
					NULL)));

	cout<<"The numbering of nodes for instruction selection"<<endl;
	label(p);
	print(p);
	cout<<endl;

	cout<<"Instructions selected at each node"<<endl;
	pick_instr(p,begin_nt);
	printin(p);
	cout<<endl;

	cout<<"Sethi Ullman numbering of each node along with the requirement of store(rightnost numbers)"<<endl;
	su_number(p);
	printsu(p);
	cout<<endl;

	cout<<"Code generated is "<<endl;
	gencode(p);
	cout<<endl;

	return 0;
}