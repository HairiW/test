#include"Lexical Analyzer.h"
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>
#include<Utility>
#include<algorithm>
#include<iomanip>
using namespace std;
void Lexical_Analyzer::GetPath(string str)
{
	filepath = str;
}
void Lexical_Analyzer::OpenFile()
{
	file.open(filepath);
	if (!file)
	{
		cout << "文件打开失败！\n" << endl;
		exit(0);
	}
	result_file.open(result_filepath, ios::out);
	if (!result_file)
	{
		cout << "写入文件打开失败！\n" << endl;
		exit(0);
	}
}
void Lexical_Analyzer::Scanner()
{
	Flush();
	GetChar();
	GetBC();
	if (ch == '\n' || ch == ' ')
		return;
	if (IsLetter())
	{
		while (IsLetter() or IsDigit())//当ch是字母或数字时
		{
			Concat();
			GetChar();
		}
		if (index <= line.length())
			Retract();
		code = Reserve();
		if (code)//如果是保留字
			Display();
		else//否则为标识符
		{
			code = reserve_word["id"];
			Display();
		}
	}
	else if (IsDigit())
	{
		while (IsDigit())
		{
			Concat();
			GetChar();
		}
		if (IsLetter())
		{
			//ProcError(1);
			while (IsLetter())
				GetChar();
			Retract();
		}
		else
		{
			Retract();
			code = reserve_word["const"];
			Display();
		}
	}
	else if (ch == ',')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == ';')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == ':')
	{
		Concat();
		GetChar();
		if (ch == '=')
		{
			Concat();
			code = Reserve();
			Display();
		}
		else
		{
			//ProcError(2);
			Retract();
		}
	}
	else if (ch == '=')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == '<')
	{
		Concat();
		GetChar();
		if (ch == '>' || ch == '=')
			Concat();
		else
			Retract();
		code = Reserve();
		Display();
	}
	else if (ch == '+')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == '-')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == '*')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == '/')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == '>')
	{
		Concat();
		GetChar();
		if (ch == '=')
			Concat();
		else
			Retract();
		code = Reserve();
		Display();
	}
	else if (ch == '(')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else if (ch == ')')
	{
		Concat();
		code = Reserve();
		Display();
	}
	else
		//ProcError(3);
		return;
}
void Lexical_Analyzer::GetChar()
{
	if (index + 1 <= line.length())
		ch = line[index++];
	else
	{
		ch = ' ';
		index++;
	}
}
void Lexical_Analyzer::GetBC()
{
	while (ch == ' ' || ch == '\n' || ch == '\t')
	{
		if (index + 1 <= line.length())
			ch = line[index++];
		else
		{
			ch = '\n';
			break;
		}
	}
}
void Lexical_Analyzer::Concat()
{
	strToken = strToken + ch;
}
int Lexical_Analyzer::Reserve()
{
	if (reserve_word[strToken])
		return reserve_word[strToken];
	else
		return 0;

}
bool Lexical_Analyzer::IsLetter()
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	else
		return false;
}
bool Lexical_Analyzer::IsDigit()
{
	if ((ch >= '0' && ch <= '9'))
		return true;
	else
		return false;
}
void Lexical_Analyzer::Retract()
{
	index--;
}
int Lexical_Analyzer::InsertId()
{
	id_list.push_back(strToken);
	id_cnt++;
	return id_cnt;
}
int Lexical_Analyzer::InsertConst()
{
	const_list.push_back(strToken);
	const_cnt++;
	return const_cnt;
}
void Lexical_Analyzer::ProcError(int type)
{
	if (type == 1)
		cout << "标识符不能以数字开头！";
	else if (type == 2)
		cout << "':'后缺少'='！";
	else
		cout << "请勿输入非法字符！";
}
void Lexical_Analyzer::Proc()
{
	OpenFile();
	row = 1;
	while (getline(file, line))
	{
		index = 0;
		while (index < line.length())
			Scanner();
		row++;
	}
}
void Lexical_Analyzer::Flush()
{
	code = 0;
	ch = ' ';
	strToken = "";
}
void Lexical_Analyzer::Display()
{
	//cout << " (" << strToken << "," << code << ") ";
	result_file  << strToken << " " << code << " " << row << endl;
}
void Lexical_Analyzer::Show_Map()
{
	vector< pair<string, int> > vec;
	for (map<string, int>::iterator it = reserve_word.begin(); it != reserve_word.end(); it++) {
		vec.push_back(pair<string, int>(it->first, it->second));
	}
	sort(vec.begin(), vec.end(), cmp);

	int i = 0;
	cout << "下表为字词-编号对照表：";
	for (vector<pair<string, int>>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		if (i % 4 == 0)
			cout << endl;
		cout << it->first << "	" << it->second << "	";
		i++;
	}
	cout << endl;
	cout << endl;
}

bool cmp(pair<string, int> a, pair<string, int> b) {
	return a.second < b.second;
}