#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>
#include<Utility>
#include<algorithm>
#include<iomanip>
using namespace std;
bool cmp(pair<string, int> a, pair<string, int> b);

class Lexical_Analyzer
{
private:
	char ch;
	int code;
	int index;
	int id_cnt;
	int const_cnt;
	int row;
	ifstream file;
	string strToken;
	string filepath;
	string line;
	map<string, int> reserve_word;
	vector<string> id_list;
	vector<string> const_list;
public:
	Lexical_Analyzer()
	{
		ch = ' ';
		code = 0;
		index = 0;
		id_cnt = -1;
		const_cnt = -1;
		strToken = "";
		filepath = "";
		line = "";
		row = 0;
		reserve_word["const"] = 1;
		reserve_word["id"] = 2;
		reserve_word["var"] = 3;
		reserve_word["procedure"] = 4;
		reserve_word[","] = 5;
		reserve_word[";"] = 6;
		reserve_word["begin"] = 7;
		reserve_word["end"] = 8;
		reserve_word["if"] = 9;
		reserve_word["then"] = 10;
		reserve_word["while"] = 11;
		reserve_word["do"] = 12;
		reserve_word["call"] = 13;
		reserve_word["read"] = 14;
		reserve_word["write"] = 15;
		reserve_word[":="] = 16;
		reserve_word["="] = 17;
		reserve_word["<>"] = 18;
		reserve_word["+"] = 19;
		reserve_word["-"] = 20;
		reserve_word["*"] = 21;
		reserve_word["/"] = 22;
		reserve_word[">"] = 23;
		reserve_word["<"] = 24;
		reserve_word[">="] = 25;
		reserve_word["<="] = 26;
		reserve_word["odd"] = 27;
		reserve_word["("] = 28;
		reserve_word[")"] = 29;
	}
	~Lexical_Analyzer()
	{
		file.close();
	}
	void GetPath(string str);
	void OpenFile();
	void GetChar();
	void GetBC();
	void Concat();
	int Reserve();
	bool IsLetter();
	bool IsDigit();
	void Retract();
	int InsertId();
	int InsertConst();
	void ProcError(int type);//1.非法标识符:error_code = 1; 2.:后无=:error_code = 2; 3.非法字符:error_code = 3
	void Proc();
	void Scanner();
	void Flush();
	void Display();
	void Show_Map();
};

int main()
{
	Lexical_Analyzer la;
	la.GetPath("code.txt");
	la.Proc();
	return 0;
}
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
}
void Lexical_Analyzer::Scanner()
{
	Flush();
	GetChar();
	GetBC();
	if (ch == '\n'||ch == ' ')
		return;
	if (IsLetter())
	{
		while (IsLetter() or IsDigit())//当ch是字母或数字时
		{
			Concat();
			GetChar();
		}
		if(index<=line.length())
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
			ProcError(1);
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
			ProcError(2);
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
		GetChar();
		if (ch != '=')
			Retract();
		Concat();
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
		ProcError(3);
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
	if ((ch >= '0' && ch <= '9') )
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
	Show_Map();
	row = 1;
	cout << "以下为识别出的字词：" << endl;
	while (getline(file, line))
	{
		index = 0;
		while(index<line.length())
			Scanner();
		cout << endl;
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
	cout << "(" << strToken << "," << code << ") ";
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
	/*for (map<string, int>::iterator it = reserve_word.begin(); it != reserve_word.end(); it++)
	{
		if (i % 4 == 0)
			cout << endl;
		cout << it->first << "	" << it->second << "	";
		i++;
	}*/
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