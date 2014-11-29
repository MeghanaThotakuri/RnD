
#include "iburg.h"
#include "parser.h"


set<string> Operators;		/* to store all the feasible operators */
map<string, Instruction> instruction_map;	/* map to store all the feasible instructions (operator <-> instruction) */ 
map<string, Terminal> terminal_map;
map<string, Nonterminal> nonterminal_map;

string begin_nt;

std::vector<string> a_terms;
std::vector<string> a_nonterms;


int non_terminal_count;
int terminal_count;

int rule_count;
vector<Rule> rules;


void add_op(string ope)
{
	Operators.insert(ope);
}


void add_ins(string t_op, string t_af, string t_am)
{
	Instruction ins;
	if(Operators.count(t_op) == 0) 
	{
		cerr << "Operator "<<t_op<<" is not a valid one"<<endl;	
	    exit(1);
	}
	ins.oper = t_op;

	if(t_af == "op") ins.af=op;
	else if(t_af == "op_r") ins.af=op_r;
	else if(t_af == "op_r_o1") ins.af=op_r_o1;
	else if(t_af == "op_r_o1_o2") ins.af=op_r_o1_o2;
	else if(t_af == "op_nsy") ins.af=op_nsy;
	else 
	{
		cerr << "assembly format "<<t_af<<" is not a valid one"<<endl;	exit(1);
	}

	ins.assembly_mnemonic = t_am;

	instruction_map.insert(make_pair(t_op,ins));
	return;
}

void add_terminal(string t_name)
{
	Terminal term;
	term.name = t_name;
	terminal_count++;
	term.number = terminal_count;
	term.arity = 0;
	terminal_map.insert(make_pair(t_name, term));
	a_terms.push_back(t_name);
	return;
}

void add_nonterminal(string t_name, std::vector<string> t_values)
{
	if(nonterminal_map.count(t_name) != 0)
		{nonterminal_map[t_name].values = t_values; return;}
	Nonterminal nonterm;
	nonterm.name = t_name;
	non_terminal_count++;
	nonterm.number = non_terminal_count;
	nonterm.values = t_values;
	nonterminal_map.insert(make_pair(t_name, nonterm));
	a_nonterms.push_back(t_name);
	return;
}

void add_begin_nonterminal(string t_name)
{
	Nonterminal nonterm;
	nonterm.name = t_name;
	non_terminal_count++;
	nonterm.number = non_terminal_count;
	nonterminal_map.insert(make_pair(t_name, nonterm));
	a_nonterms.push_back(t_name);
	begin_nt = t_name;
	return;
}

Tree * create_tree_m(string t_name, Tree * t_left, Tree * t_right)
{
	if(terminal_map.count(t_name) != 0)
	{
		Terminal &t = terminal_map[t_name];
		if(t_left && t_right) t.arity = 2;
		if(t_left || t_right) t.arity = max(t.arity,1);
		Tree * tree = new Tree();
		tree->node = t_name;
		tree->left = t_left;
		tree->right = t_right;
		tree->value = "";
		return tree;
	}
	else if(nonterminal_map.count(t_name) != 0)
	{
		if(t_left || t_right) {cout<<"a tree with nonterminal as root should have 0 arity : "<<t_name<<endl; exit(1);}
		Tree * tree = new Tree();
		tree->node = t_name;
		tree->left = t_left;
		tree->right = t_right;
		tree->value = "";

		return tree;
	}
	else
	{
		cout<<"wrong tree-node: neither terminal nor nonterminal : "<<t_name<<endl;
	}
	return NULL;
}


Tree * create_tree_m(string t_name, Tree * t_left, Tree * t_right, string t_value)
{
	if(terminal_map.count(t_name) != 0)
	{
		Terminal &t = terminal_map[t_name];
		if(t_left && t_right) t.arity = 2;
		if(t_left || t_right) t.arity = max(t.arity,1);
		Tree * tree = new Tree();
		tree->node = t_name;
		tree->left = t_left;
		tree->right = t_right;
		tree->value = t_value;
		return tree;
	}
	else if(nonterminal_map.count(t_name) != 0)
	{
		if(t_left || t_right) {cout<<"a tree with nonterminal as root should have 0 arity : "<<t_name<<endl; exit(1);}
		Tree * tree = new Tree();
		tree->node = t_name;
		tree->left = t_left;
		tree->right = t_right;
		tree->value = t_value;
		return tree;
	}
	else
	{
		cout<<"wrong tree-node: neither terminal nor nonterminal : "<<t_name<<endl;
	}
	return NULL;
}


void create_rule(string t_lhs, Tree * t_tree, int t_cost, std::vector<string> t_order)
{
	// cout<<"create rule called"<<endl;
	Rule rule;
	rule.lhs = t_lhs;
	rule.pattern = *t_tree;
	rule_count++;
	rule.number = rule_count;
	rule.cost = t_cost;
	rule.instr = t_order[0];
	//args_order
	int sz = t_order.size();
	// cout<<rule_count<<" "<<sz<<endl;
	switch(sz)
	{
		case 1:
		{
			rule.args_order = _none;
			break;
		}
		case 2:
		{
			if(t_order[1] == "$$") rule.args_order = _result;
			else if(t_order[1] == "$1") rule.args_order = _left;
			else if(t_order[1] == "$2") rule.args_order = _right;
			break;
		}
		case 3:
		{
			if(t_order[1] == "$$")
			{
				if(t_order[2] == "$1") rule.args_order = result_left;
				else if(t_order[2] == "$2") rule.args_order = result_right;
				else if(t_order[2] == "$0") rule.args_order = result_node;
			}
			else if(t_order[1] == "$1" && t_order[2] == "$2") rule.args_order = left_right;
			else if(t_order[1] == "$2" && t_order[2] == "$1") rule.args_order = right_left;
			else {cout<<"wrong order of arguments for machine instruction :"<<rule_count<<endl; exit(1);}
			break;
		}
		case 4:
		{
			if(t_order[1] == "$$" && t_order[2] == "$1" && t_order[3] == "$2") rule.args_order = result_left_right;
			else if(t_order[1] == "$$" && t_order[2] == "$2" && t_order[3] == "$1") rule.args_order = result_right_left;
			else {cout<<"wrong order of arguments for machine instruction :"<<rule_count<<endl; exit(1);}
			break;
		}
	}
	rules.push_back(rule);
	// cout<<rule_count<<endl;
	return;
}

void print_rule(Rule rule)
{
	cout<<rule.lhs<<"\t"<<rule.number<<"\t"<<rule.cost<<"\t"<<rule.instr<<"\t"<<rule.args_order<<endl;
}


void emitheaders()
{
	cout<<"#include <iostream>\n#include <cstdio>\n#include \"iburg.h\"\n#include \"memorylocation.h\"\n\n";
	cout<<"#include <limits.h>\n#include <stdlib.h>\n#define PANIC printf \n\n";
	cout<<"#define MAX 33333333\n";
	cout<<"#define nonterms "<<non_terminal_count<<endl;
	cout<<"#define terms "<<terminal_count<<endl;
	cout<<"\n";
	for (int i = 1; i <= non_terminal_count; ++i)
	{
		cout<<"#define nt_"<<a_nonterms[i]<<" "<<i<<endl;
	}
	cout<<endl;

	cout<<"int begin_nt = "<<nonterminal_map[begin_nt].number<<";\n";
	cout<<"memorylocation memloc;"<<endl;
	cout<<endl;
}


void tree_string(Tree * t)
{
	if(t == NULL) cout<<"NULL";
	else if(t->right==NULL && t->left == NULL) cout<<"create_tree(\""<<t->node<<"\")";
	else {
		cout<<"create_tree(\""<<t->node<<"\",";
		tree_string(t->left);
		cout<<",";
		tree_string(t->right);
		cout<<")";
	}
}


void emitdatastructures()
{
	cout<<"string a_nonterminal[] = \n {\n ";
	for (int i = 0; i <= non_terminal_count; ++i)
	{
		cout<<"\t \""<<a_nonterms[i]<<"\" ,\n ";
	}
	cout<<"};\n\n";

	cout<<"string a_terminal[] = \n{\n";
	for (int i = 0; i <= terminal_count; ++i)
	{
		cout<<"\t\""<<a_terms[i]<<"\",\n";
	}
	cout<<"};\n\n";

	cout<<"int a_nt_values_count[]=\n{\n";
	for (int i = 0; i <= non_terminal_count; ++i)
	{
		cout<<"\t"<<nonterminal_map[a_nonterms[i]].values.size()<<", ";
	}
	cout<<"\n};\n";

	cout<<"void fill_nonterminals(){\n\tnonterminal_values.resize(nonterms+1);\n";
	for (int i = 1; i <= non_terminal_count; ++i)
	{
		for (int j =  nonterminal_map[a_nonterms[i]].values.size()-1; j >= 0; j--)
		{
			cout<<"\tnonterminal_values["<<i<<"].push(\""<<nonterminal_map[a_nonterms[i]].values[j]<<"\");\n";
		}
	}
	cout<<"}\n\n";
	cout<<"void fill_instructions(){\n";
	for (std::map<string, Instruction>::iterator i = instruction_map.begin(); i != instruction_map.end(); ++i)
	{
		cout<<"\tcreate_instruction(\""<<i->first<<"\", (assembly_format)"<<i->second.af<<",\""<<i->second.assembly_mnemonic<<"\");\n";
	}
	cout<<"}\n\n";

	cout<<"set<string> nonterminal;\nset<string> terminal;\n";
	cout<<"map<string, int> nonterminal_map;\n";
	cout<<"bool isterminal(string s){	return (terminal.count(s)==1); }\n";
	cout<<"bool isnonterminal(string s){	return (nonterminal.count(s)==1); }\n\n";

	cout<<"std::vector<Rule> rules;"<<endl;
	cout<<"Rule create_rule(string t_lhs, Tree t_tree, int t_no, int t_cost, string t_instr, Order t_ao){\n";
	cout<<"Rule r;\nr.lhs=t_lhs;\nr.pattern = t_tree;\nr.number = t_no;\n";
	cout<<"r.cost = t_cost;\nr.instr = t_instr;\nr.args_order = t_ao;\nreturn r;\n}\n\n";

	cout<<"void fill_rules(){\n\tRule strt;\n\trules.push_back(strt);\n";

	for (int i = 0; i < rule_count; ++i)
	{
		cout<<"\trules.push_back(create_rule(\""<<rules[i].lhs<<"\",*";
		tree_string(&rules[i].pattern);
		cout<<","<<rules[i].number<<","<<rules[i].cost<<",\""<<rules[i].instr<<"\",";
		cout<<"(Order)"<<rules[i].args_order<<"));\n";
	}
	cout<<"}\n\n";

	cout<<"void initialize(){\n";
	cout<<"\tnonterminal.insert(a_nonterminal, a_nonterminal+nonterms+1);\n";
	cout<<"for (int i = 1; i <= nonterms; ++i)\tnonterminal_map.insert(make_pair(a_nonterminal[i],i));\n";
	cout<<"\tterminal.insert(a_terminal, a_terminal+terms+1);\n\tfill_rules();\n\tfill_nonterminals();\n\tfill_instructions();\n}\n\n";
}


std::vector<bool> closureexists(non_terminal_count+2,false);

void emitclosures()
{
	std::vector<bool> closurerule(rule_count,false);
	std::vector<int> closureruleofnt(rule_count,-1);
	
	for (int i = 0; i < rule_count; ++i)
	{
		Rule r = rules[i];
		if(nonterminal_map.count(r.pattern.node) !=0 && r.pattern.left==NULL && r.pattern.right==NULL)
		{
			closurerule[i]=true;
			closureruleofnt[i]=nonterminal_map[r.pattern.node].number;
			closureexists[closureruleofnt[i]]=true;
		}
	}
	for (int i = 1; i <= non_terminal_count; ++i)
	{
		if(closureexists[i])
			cout<<"void closure_"<<a_nonterms[i]<<"(State *);"<<endl;
	}
	cout<<"\n";

	for (int i = 1; i <= non_terminal_count; ++i)
	{
		if(closureexists[i]){
			cout<<"void closure_"<<a_nonterms[i]<<"(State * st){"<<endl;
			for (int j = 0; j < rule_count; ++j)
			{
				if(closureruleofnt[j]==i)
				{
					cout<<"\t if(st->cost[nt_"<<a_nonterms[i]<<"]+"<<rules[j].cost<<" < "<<"st->cost[nt_"<<rules[j].lhs<<"]){\n";
					cout<<"\t\t st->cost[nt_"<<rules[j].lhs<<"] = st->cost[nt_"<<a_nonterms[i]<<"]+"<<rules[j].cost<<";\n";
					cout<<"\t\t st->rule[nt_"<<rules[j].lhs<<"] = "<<j+1<<";"<<endl;
					if(closureexists[nonterminal_map[rules[j].lhs].number])
						cout<<"\t\t closure_"<<rules[j].lhs<<"(st);"<<endl;
					cout<<"\t}\n";
				}
			}
			cout<<"}\n\n";
		}
	}
	cout<<"\n";
}


void emitautomata(){
	cout<<"void state_label(Tree * p){\n";
	cout<<"(p->state).cost.resize(nonterms+1,MAX);\n";
	cout<<"(p->state).rule.resize(nonterms+1,-1);\n";
	cout<<"string op = p->node;\n";
	cout<<"int c;\n";
	cout<<"\n";

	for (int i = 0; i < rule_count; ++i)
	{
		Rule r = rules[i];
		cout<<"if(op==\""<<r.pattern.node<<"\"){\n";
		cout<<"\tc=";
		if(r.pattern.left==NULL || terminal_map.count((r.pattern.left)->node)!=0) cout<<"0 + ";
		else cout<<"(p->left->state).cost[nt_"<<r.pattern.left->node<<"] + ";
		if(r.pattern.right==NULL || terminal_map.count((r.pattern.right)->node)!=0) cout<<"0 + ";
		else cout<<"(p->right->state).cost[nt_"<<r.pattern.right->node<<"] + ";
		cout<<r.cost<<";\n";

		cout<<"\tif(";
		if(r.pattern.left != NULL && terminal_map.count((r.pattern.left)->node)!=0) cout<<"p->left->node == \""<<(r.pattern.left)->node<<"\" && ";
		if(r.pattern.right != NULL && terminal_map.count((r.pattern.right)->node)!=0) cout<<"p->right->node == \""<<(r.pattern.right)->node<<"\" && ";
		cout<<"c < (p->state).cost[nt_"<<r.lhs<<"]){\n";
		cout<<"\t\t(p->state).cost[nt_"<<r.lhs<<"] = c;\n";
		cout<<"\t\t(p->state).rule[nt_"<<r.lhs<<"] = "<<r.number<<";\n";
		if(closureexists[nonterminal_map[r.lhs].number])
			cout<<"\t\tclosure_"<<r.lhs<<"(&p->state);\n";
		cout<<"\t}\n";
		cout<<"}\n";
	}
	cout<<"}\n\n";

	cout<<"void label(Tree * p){\n";
	cout<<"\tif(p == NULL) PANIC(\"null tree\");\n";
	cout<<"\tif(p->left != NULL) label(p->left);\n";
	cout<<"\tif(p->right != NULL) label(p->right);\n";
	cout<<"\tstate_label(p);\n}\n";
}


void emithelpers()
{

	cout<<"Tree * create_tree(string t_name){\n";
	cout<<"\tTree * tree = new Tree();\n\ttree->node = t_name;\n\ttree->left = NULL;\n";
	cout<<"\ttree->right = NULL;\n\ttree->value = \"\";\n\treturn tree;\n}\n\n";


	cout<<"Tree * create_tree(string t_name, Tree * t_left, Tree * t_right){\n";
	cout<<"\tTree * tree = new Tree();\n\ttree->node = t_name;\n\ttree->left = t_left;\n";
	cout<<"\ttree->right = t_right;\n\ttree->value = \"\";\n\treturn tree;\n}\n\n";

	cout<<"Tree * create_tree(string t_name, Tree * t_left, Tree * t_right,string t_val){\n";
	cout<<"Tree * tree = new Tree();\n	tree->node = t_name;\n tree->left = t_left;\n tree->right = t_right;\n ";
	cout<<"tree->value = t_val;\n return tree;\n}\n\n\n";

	cout<<"map<string, Instruction> instruction_map;"<<endl;
	cout<<"vector<stack<string> > nonterminal_values;"<<endl;
	cout<<"Instruction create_instruction(string t_op, assembly_format t_af, string t_am){"<<endl;
	cout<<"\t	Instruction ins;"<<endl;
	cout<<"\t	ins.oper = t_op;"<<endl;
	cout<<"\t	ins.af = t_af;"<<endl;
	cout<<"\t	ins.assembly_mnemonic = t_am;"<<endl;
	cout<<"\t	instruction_map.insert(make_pair(t_op,ins));"<<endl;
	cout<<"\t	return ins;"<<endl;
	cout<<"}"<<endl;
}

void emitdecode()
{
	cout<<"void pick_instr(Tree * p, int result_nt){\n";
	cout<<"\tint rno = (p->state).rule[result_nt];\n\tp->picked_instruction.push_back(rno);\n\tTree t = rules[rno].pattern;\n";
	cout<<"if(t.node == p->node){\n";
	cout<<"\tif(t.left != NULL && isnonterminal(t.left->node)){\n\tstring nt = t.left->node;\n";
	cout<<"\tfor (int i = 1; i <= nonterms; ++i){\n\t\tif(a_nonterminal[i]==nt){ pick_instr(p->left, i); break;}\n\t}\n\t}\n";
	cout<<"\tif(t.right != NULL && isnonterminal(t.right->node)){\n";
	cout<<"\tstring nt = t.right->node;\n\tfor (int i = 1; i <= nonterms; ++i){\n";
	cout<<"\t\tif(a_nonterminal[i]==nt){pick_instr(p->right, i); break;}\n";
	cout<<"\t}\n\t}\n}";
	cout<<"else{\n\tfor (int i = 1; i <= nonterms; ++i){\n\t\tif(t.node == a_nonterminal[i]) {pick_instr(p,i); return;}";
	cout<<"\n\t}\n}";
	cout<<"\n}\n\n";
}


void emitcodegen()
{
	cout<<"void su_number(Tree * p)\n";
cout<<"{\n";
cout<<"\tp->nt_reqd.resize(nonterms+1,0);\n";
cout<<"\tp->store_reqd.resize(nonterms+1,false);\n";
cout<<"\tint rno=0;\n";
cout<<"    for (std::list<int>::iterator i = p->picked_instruction.begin(); i != p->picked_instruction.end(); ++i) rno = *i;\n";
cout<<"    if(rno == 0) return;\n";
cout<<"\tTree t = rules[rno].pattern;\n";
cout<<"\tif(t.left==NULL && t.right==NULL){\n";
cout<<"\t\tfor (int i = 1; i <= nonterms; ++i){\n";
cout<<"\t\t\tif(a_nonterminal[i] == rules[rno].lhs) p->nt_reqd[i]=1;\n";
cout<<"\t\t\telse if(t.node == a_nonterminal[i]) p->nt_reqd[i]=1;\n";
cout<<"\t\t}\n";
cout<<"\t}\n";
cout<<"\telse if(t.right==NULL){\n";
cout<<"\t\tsu_number(p->left);\n";
cout<<"\t\tfor (int i = 1; i <= nonterms; ++i){\n";
cout<<"\t\t\tp->nt_reqd[i]=p->left->nt_reqd[i];\n";
cout<<"\t\t\tif(rules[rno].lhs == a_nonterminal[i] && p->nt_reqd[i]==0) p->nt_reqd[i]++; \n";
cout<<"\t\t}\n";
cout<<"\t}\n";
cout<<"\telse{\n";
cout<<"\t\tsu_number(p->left);\n";
cout<<"\t\tsu_number(p->right);\n";
cout<<"\t\tfor (int i = 1; i <= nonterms; ++i){\n";
cout<<"\t\t\tp->nt_reqd[i] = max(p->left->nt_reqd[i], p->right->nt_reqd[i]);\n";
cout<<"\t\t\tif(p->left->nt_reqd[i] == p->right->nt_reqd[i] && p->left->nt_reqd[i] != 0) {\n";
cout<<"\t\t\t\tp->nt_reqd[i]++;\n";
cout<<"\t\t\t\tif(a_nt_values_count[i] < p->nt_reqd[i]) {p->left->store_reqd[i] = true; p->nt_reqd[i]--; p->left->nt_reqd[i]=1;}\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\tif(rules[rno].lhs == a_nonterminal[i] && p->nt_reqd[i]==0) p->nt_reqd[i]++;\n";
cout<<"\t\t}\n";
cout<<"\t}\n";
cout<<"\n";
cout<<"\tfor (std::list<int>::iterator i = p->picked_instruction.begin(); i != p->picked_instruction.end(); ++i)\t{\n";
cout<<"   \t\tif(*i == rno) break;\n";
cout<<"   \t\tfor (int j = 1; j <= nonterms; ++j){\n";
cout<<"   \t\t\tif(a_nonterminal[j]==rules[*i].lhs && p->nt_reqd[j]==0) p->nt_reqd[j]++;\n";
cout<<"   \t\t}\n";
cout<<"   \t}\n";
cout<<"}\n";
cout<<"\n";
cout<<"string get_val(string nt_type)\n";
cout<<"{\n";
cout<<"\tif(nonterminal_values[nonterminal_map[nt_type]].empty()) {cout<<\"errr : no available nonterminals of type \"<<nt_type<<endl; exit(0);}\n";
cout<<"\tstring ret = nonterminal_values[nonterminal_map[nt_type]].top();\n";
cout<<"\tnonterminal_values[nonterminal_map[nt_type]].pop();\n";
cout<<"\treturn ret;\n";
cout<<"}\n";
cout<<"\n";
cout<<"void release_nt_val(string val, string nt_type)\n";
cout<<"{\n";
cout<<"\tif(val == \"\" || !isnonterminal(nt_type)) return;\n";
cout<<"\tnonterminal_values[nonterminal_map[nt_type]].push(val);\n";
cout<<"}\n";
cout<<"\n";
cout<<"void gencode1(Tree * p)\n";
cout<<"{\n";
cout<<"\tbool startone = true;\n";
cout<<"\tint rno=0;\n";
cout<<"\tif(p==NULL) return;\n";
cout<<"\telse if(p->picked_instruction.empty()) {p->result = p->value; p->code_generated=true; return;}\n";
cout<<"\telse if(p->result_computed_by_store == true){\n";
cout<<"\t\tstring res_val = get_val(rules[*(p->picked_instruction.begin())].lhs);\n";
cout<<"\t\tcout<<\"mv \"<<res_val<<\", \"<<p->result<<endl;\n";
cout<<"\t\tmemloc.free_memory_location(p->result);\n";
cout<<"\t\tp->result = res_val;\n";
cout<<"\t\tp->result_computed_by_store = false;\n";
cout<<"\t\treturn;\n";
cout<<"\t}\n";
cout<<"\tfor (std::list<int>::reverse_iterator rit=p->picked_instruction.rbegin(); rit!=p->picked_instruction.rend(); ++rit){\n";
cout<<"\t\tRule r = rules[*rit];\n";
cout<<"\t\tif(startone == true){\n";
cout<<"\t\t\tstartone = false;\n";
cout<<"\t\t\tif(r.args_order==_none)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\t\tif(r.instr == \"no_instruction\"){ p->result = p->value; return;}\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<endl;\n";
cout<<"\t\t\t\tp->result=\"\";\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==_result)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\tp->result = get_val(r.lhs);\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->result<<endl;\t\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==_left)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tp->result = \"\";\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->left->result<<endl;\t\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==_right)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\tp->result = \"\";\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->right->result<<endl;\t\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==result_node)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\tp->result = get_val(r.lhs);\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->result<<\", \"<<p->value<<endl;\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==result_left)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tp->result = get_val(r.lhs);\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->result<<\", \"<<p->left->result<<endl;\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==result_right)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\tp->result = get_val(r.lhs);\n";
cout<<"\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->result<<\", \"<<p->right->result<<endl;\n";
cout<<"\t\t\t}\n";
cout<<"\t\t\telse if(r.args_order==right_left || r.args_order==left_right || r.args_order==result_left_right || r.args_order==result_right_left)\n";
cout<<"\t\t\t{\n";
cout<<"\t\t\t\tif(r.pattern.left->node == r.pattern.right->node)\n";
cout<<"\t\t\t\t{\n";
cout<<"\t\t\t\t\tint nt_index = nonterminal_map[r.pattern.left->node];\n";
cout<<"\t\t\t\t\tif(p->left->nt_reqd[nt_index] < p->right->nt_reqd[nt_index]){\n";
cout<<"\t\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node);\n";
cout<<"\t\t\t\t\t}\n";
cout<<"\t\t\t\t\telse{\n";
cout<<"\t\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\t\t}\n";
cout<<"\t\t\t\t}\n";
cout<<"\t\t\t\telse\n";
cout<<"\t\t\t\t{\n";
cout<<"\t\t\t\t\tgencode1(p->left);\n";
cout<<"\t\t\t\t\tgencode1(p->right);\n";
cout<<"\t\t\t\t\tif(r.pattern.right != NULL && isnonterminal(r.pattern.right->node))\n";
cout<<"\t\t\t\t\t\trelease_nt_val(p->right->result, r.pattern.right->node); \n";
cout<<"\t\t\t\t\tif(r.pattern.left != NULL && isnonterminal(r.pattern.left->node))\n";
cout<<"\t\t\t\t\t\trelease_nt_val(p->left->result, r.pattern.left->node); \n";
cout<<"\t\t\t\t}\n";
cout<<"\n";
cout<<"\t\t\t\tif(r.args_order == right_left){\n";
cout<<"\t\t\t\t\tp->result = \"\";\n";
cout<<"\t\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->right->result<<\", \"<<p->left->result<<endl;\n";
cout<<"\t\t\t\t}\n";
cout<<"\t\t\t\telse if(r.args_order==left_right)\n";
cout<<"\t\t\t\t{\n";
cout<<"\t\t\t\t\tp->result = \"\";\n";
cout<<"\t\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->left->result<<\", \"<<p->right->result<<endl;\n";
cout<<"\t\t\t\t}\n";
cout<<"\t\t\t\telse if(r.args_order==result_left_right)\n";
cout<<"\t\t\t\t{\n";
cout<<"\t\t\t\t\tp->result = get_val(r.lhs);\n";
cout<<"\t\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->result<<\", \"<<p->left->result<<\", \"<<p->right->result<<endl;\n";
cout<<"\t\t\t\t}\n";
cout<<"\t\t\t\telse if(r.args_order==result_right_left)\n";
cout<<"\t\t\t\t{\n";
cout<<"\t\t\t\t\tp->result = get_val(r.lhs);\n";
cout<<"\t\t\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<p->result<<\", \"<<p->right->result<<\", \"<<p->left->result<<endl;\n";
cout<<"\t\t\t\t}\n";
cout<<"\t\t\t}\n";
cout<<"\t\t}\n";
cout<<"\t\telse{\n";
cout<<"\t\t\tstring res_val = get_val(r.lhs);\n";
cout<<"\t\t\tcout<<instruction_map[r.instr].assembly_mnemonic<<\" \"<<res_val<<\", \"<<p->result<<endl;\n";
cout<<"\t\t\trelease_nt_val(p->result, r.pattern.node); \n";
cout<<"\t\t\tp->result = res_val;\n";
cout<<"\t\t}\n";
cout<<"\t}\n";
cout<<"}\n";
cout<<"\n";
cout<<"void gencode_store(Tree * p)\n";
cout<<"{\n";
cout<<"\tif(p->result_computed_by_store == false) {\n";
cout<<"\t\tgencode1(p);\n";
cout<<"\t\tstring res_mem_val = memloc.get_memory_location(nonterminal_map[rules[*(p->picked_instruction.begin())].lhs]);\n";
cout<<"\t\tcout<<\"mv \"<<res_mem_val<<\", \"<<p->result<<endl;\n";
cout<<"\t\trelease_nt_val(p->result, rules[*(p->picked_instruction.begin())].lhs); \n";
cout<<"\t\tp->result = res_mem_val;\n";
cout<<"\t\tp->result_computed_by_store = true;\n";
cout<<"\t}\n";
cout<<"}\n";
cout<<"\n";
cout<<"void check_and_generate_stores(Tree * p)\n";
cout<<"{\n";
cout<<"\tif(p==NULL) return;\n";
cout<<"\tp->result_computed_by_store = false;\n";
cout<<"\tp->code_generated = false;\n";
cout<<"\tcheck_and_generate_stores(p->left);\n";
cout<<"\tcheck_and_generate_stores(p->right);\n";
cout<<"\tfor (int i = 1; i <= nonterms; ++i)\n";
cout<<"\t\tif(p->store_reqd[i]) gencode_store(p);\n";
cout<<"}\n";
cout<<"\n";
cout<<"void gencode(Tree * p)\n";
cout<<"{\n";
cout<<"\tcheck_and_generate_stores(p);\n";
cout<<"\tgencode1(p);\n";
cout<<"}\n\n\n";
}


void emitmain()
{
	cout<<"int main() {\n\t	initialize();\n\tTree * p;\n\t p = create_tree(... , ... , ...);";
	cout<<"\n\tlabel(p);\n\tpick_instr(p,begin_nt);\n\tsu_number(p);\n\tgencode(p);\n\treturn 0;\n}\n";
}


int main()
{
	non_terminal_count = 0;
	terminal_count = 0;
	rule_count = 0;

	Parser simcalc;

	a_nonterms.push_back("");
	a_terms.push_back("");

	simcalc.parse();

	emitheaders();
	emithelpers();
	emitdatastructures();
	emitclosures();
	emitautomata();
	emitdecode();
	emitcodegen();
	emitmain();
	
} 
