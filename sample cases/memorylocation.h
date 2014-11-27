#include <iostream>
#include <string>
#include <vector>

using namespace std;

class memorylocation
{
private:
	 vector<string> memlocations;
	 vector<bool> availablity;
public:
	 string get_memory_location(int type);
	 void free_memory_location(string t_mem_loc);

	 memorylocation();
};


memorylocation::memorylocation() 
{
	string a[7] = {"m1","m2","m3","m4","m5","m6","m7"};
	memlocations = *new vector<string>( a, a+7);
	availablity.resize(memlocations.size(),true);
}

string memorylocation::get_memory_location(int type)
{
	for (int i = 0; i < memlocations.size(); ++i)
	{
		if(availablity[i]) {availablity[i] = false; return memlocations[i];}
	}
	return "";
}

void memorylocation::free_memory_location(string t_mem_loc)
{
	for (int i = 0; i < memlocations.size(); ++i)
	{
		if(memlocations[i] == t_mem_loc) availablity[i] = true;
	}
}
