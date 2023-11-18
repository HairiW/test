#include<iostream>
#include"Lexical Analyzer.h"
#include"Grammar Analyzer.h"
using namespace std;
int main()
{
	Lexical_Analyzer la;
	//la.GetPath("right_code.txt");
	la.GetPath("wrong_code.txt");
	la.Proc();
	Grammar_Analyzer ga;
	ga.OpenFile("result_file.txt");
	ga.Prog();
	return 0;
}