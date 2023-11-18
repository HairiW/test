#include"Grammar Analyzer.h"
#include<map>
#include<iostream>
#include<sstream>
#include<set>
using namespace std;

void Grammar_Analyzer::OpenFile(string str)
{
	file.open(str);
	if (file.fail())
	{
		cout << "���ļ�ʧ�ܣ�" << endl;
		exit(0);
	}
	string line;
	if (getline(file, line))
	{
		istringstream iss(line);
		iss >> strToken_next;
		iss >> str_code_next;
		iss >> str_row_next;
	}
	else strToken_next = "\EOF";
}
void Grammar_Analyzer::Advance()
{
	if (strToken_next == "\EOF")
		strToken == strToken;
	else
	{
		strToken = strToken_next;
		str_code = str_code_next;
		str_row = str_row_next;
		string line;
		if (getline(file, line))
		{
			istringstream iss(line);
			iss >> strToken_next;
			iss >> str_code_next;
			iss >> str_row_next;
		}
		else strToken_next = "\EOF";
		//if (strToken == "\EOF")
			//return;
	}
}
void Grammar_Analyzer::Prog()
{
	//<prog> -> program<id>;<block>
	Advance();
	if (strToken != "program" && str_code_next == 2)
	{
		Error(1, "program");//������ƴд����
		Advance();
	}
	else if (strToken != "program")
		Error(2, "program");//ȱ�ٱ�����
	else
		Advance();
	if (str_code != 2)
		Error(11, "");//ȱ��id
	else
		Id();
	if (strToken != ";" && !block_first.count(strToken) && strToken_next != "\EOF")
	{
		Error(12, strToken);//�����ַ�
		cout << "����ͬ���У�������";
		while (strToken != ";" && !block_first.count(strToken) && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	if (strToken == ";")
	{
		Advance();
		Block();
	}
	else
	{
		Error(3, ";");//ȱ��;
		Block();
	}
	//if (strToken != "\EOF")
		//Error(10, "");//�������
	cout << "�������" << endl;
}
void Grammar_Analyzer::Block()
{
	//<block> -> [<condecl>][<vardecl>][<proc>]<body>
	if (str_code == 2)
		Body();
	else
	{
		if (!block_first.count(strToken))
		{
			Error(2, "const/var/procedure/begin");
			cout << "����ͬ����������";
			while (!block_first.count(strToken) && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << "	����ͬ����..." << endl;
		}
		if (strToken == "const")
			Condecl();
		if (!block_first.count(strToken) && strToken_next != "\EOF")
		{
			Error(2, "var/procedure/begin");
			cout << "����ͬ����������";
			while (!block_first.count(strToken) && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << "	����ͬ����..." << endl;
		}
		if (strToken == "var")
			Vardecl();
		if (!block_first.count(strToken))
		{
			Error(2, "procedure/begin");
			cout << "����ͬ����������";
			while (!block_first.count(strToken))
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << "	����ͬ����..." << endl;
		}
		if (strToken == "procedure")
			Proc();
		//while (strToken != "begin" && strToken_next != "\EOF")
			//Advance();
		Body();
	}
}
void Grammar_Analyzer::Condecl()
{
	//<condecl> -> const <const>{,<const>};
	Advance();
	if (str_code != 2)
	{
		Error(11, "");
		cout << "����ͬ����������";
		while (str_code != 2 && strToken != ";" && strToken != "var" && strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	if(str_code==2)
		Const();
	while (strToken != ";" && strToken != "var" && strToken != "procedure" && strToken != "begin")
	{
		while (strToken == "," && str_code_next == 2)
		{
			Advance();
			Const();
		}
		if (strToken == "," && str_code_next != 2)
		{
			Error(12, strToken);
			cout << "����ͬ���У�������";
			while (strToken != "var" && strToken != "procedure" && strToken != "begin" && strToken != ";")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << " ����������..." << endl;
		}
		while (str_code == 2 && strToken_next == ":=")//<const>֮��ȱ��","
		{
			Error(4, ",");
			Const();
		}
	}
	if (strToken == ";")//��������
		Advance();
	else if (strToken == "var" || strToken == "procedure" || strToken == "begin")//ȱ��;
		Error(3, ";");//ȱ��;
	else//ȱ��";"�Һ��ж����ַ�
	{
		Error(3, ";");//ȱ��;
		Error(12, strToken);//���ж����ַ�
		cout << "����ͬ���У�������";
		while (strToken != "var" && strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
}
void Grammar_Analyzer::Const()
{
	//<const> -> <id>:=<integer>
	Advance();
	if (strToken == ":=")
	{
		Advance();
		if (str_code == 1)
			Advance();
		else
			Error(13, "");
	}
	else
	{
		Error(9, "");//�Ƿ���ֵ
		if (str_code_next == 1)
			Advance();
		if (str_code == 1)
			Advance();
		else
			Error(13, "");
	}
}
void Grammar_Analyzer::Vardecl()
{
	//<vardecl> -> var <id>{,<id>};
	Advance();
	if (str_code != 2)
	{
		Error(11, "");
		cout << "����ͬ����������";
		while (str_code != 2 && strToken != ";" && strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	if(str_code==2)
		Id();
	while (strToken != "procedure" && strToken != "begin" && strToken != ";")
	{
		while (strToken == "," && str_code_next == 2)
		{
			Advance();
			Id();
		}
		if (strToken == "," && str_code_next != 2)
		{
			Error(12, strToken);
			cout << "����ͬ���У�������";
			while (strToken != "procedure" && strToken != "begin" && strToken != ";")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << " ����������..." << endl;
		}
		while (str_code == 2)//<id>֮��ȱ��","
		{
			Error(4, ",");
			Id();
		}
	}
	
	if (strToken == ";")//��������
		Advance();
	else if (strToken == "procedure" || strToken == "begin")//ȱ��;
		Error(3, ";");//ȱ��;
	else//ȱ��";"�Һ��ж����ַ�
	{
		Error(3, ";");//ȱ��;
		Error(12, strToken);//���ж����ַ�
		cout << "����ͬ���У�������";
		while (strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	/*while (strToken != "," && strToken != ";")
	{
		if (str_code == 2)
		{
			Error(4, ",");//ȱ��,
			Id();
		}
		else
			Advance();
	}
	if (strToken == ";")
		Advance();
	else
	{
		Error(3, ";");//ȱ��;
		while (strToken != "procedure" && strToken != "begin")
			Advance();
	}*/
}
void Grammar_Analyzer::Proc()
{
	bool proc_id = false;
	bool l_bracket = false;
	//<proc> -> procedure <id> ([<id>{,<id>}]);<block>{;<proc>}
	Advance();
	if (str_code != 2)
	{
		Error(12, strToken);
		cout << "����ͬ����������";
		while (str_code != 2 && strToken != "("  && strToken != ")" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	else
	{
		Id();
		proc_id = true;
	}
	if (proc_id == false && str_code == 2)
	{
		Id();
		proc_id = true;
	}
	else if(strToken != "(" && strToken != ")" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
	{
		Error(12, strToken);
		cout << "����ͬ����������";
		while (strToken != "("  && strToken != ")" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	if (strToken == "(")
	{
		l_bracket = true;
		Advance();
		while (strToken != ")" && strToken != ";" && !block_first.count(strToken) && strToken_next != "\EOF")
		{
			if (str_code == 2 && strToken_next == ",")
			{
				Id();
				Advance();
			}
			else if (str_code == 2 && str_code_next == 2)
			{
				Error(4, ",");//ȱ��,
				Id();
			}
			else if (strToken == ",")
			{
				Error(12, strToken);
				Advance();
			}
			else if (str_code == 2 && strToken_next == ")")
				Id();
		}
	}
	if (strToken == ")" || strToken == ";")
	{
		if (l_bracket == false)
			Error(5, "(");//ȱ��(
		if (strToken == ";")
			Error(6, ")");//ȱ��)
		else
			Advance();
		Advance();
		/*if (!block_first.count(strToken))
		{
			Error(12, "");//�����ַ�
			cout << "����ͬ���У�������";
			while (!block_first.count(strToken))
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << " ����������..." << endl;
		}*/
		Block();
		while (strToken == ";" && strToken_next == "procedure")
		{
			Advance();
			Proc();
		}
	}
	else if (block_first.count(strToken))
	{
		Error(5, "");//ȱ��(
		Error(6, "");//ȱ��)
		Error(3, "");//ȱ��;
		Block();
		while (strToken == ";" && strToken_next == "procedure")
		{
			Advance();
			Proc();
		}
	}
}
void Grammar_Analyzer::Body()
{
	//<body> -> begin <statement>{;<statement>}end
	if (strToken != "begin" && statement_first.count(strToken_next) && strToken_next != "\EOF")
		Error(1, "begin");
	else if (strToken != "begin" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
	{
		Error(12, strToken);
		cout << "����ͬ����������";
		while (strToken != "begin" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	if (strToken == "begin")
	{
		Advance();
		Statement();
	}
	else if (strToken == "\EOF")
		return;
	else
	{
		Error(7, "");//ȱ��begin
		Statement();
	}
	while ((strToken == ";" && (statement_first.count(strToken_next) || str_code_next == 2)) || (statement_first.count(strToken) || ((strToken_next == ":=" || strToken_next == "=") && str_code == 2)))
	{
		while (strToken == ";" && (statement_first.count(strToken_next) || str_code_next == 2))
		{
			Advance();
			while (str_code == 2 || statement_first.count(strToken))
				Statement();
		}
		while (statement_first.count(strToken) || ((strToken_next == ":=" || strToken_next == "=") && str_code == 2))
		{
			Error(3, ";");
			Statement();
		}
	}
	if (strToken != "end" && strToken != "else" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
	{
		Error(12, strToken);
		cout << "����ͬ����������";
		while (strToken != "end" && strToken != "else" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "ͬ������" << strToken << " ����������..." << endl;
	}
	if (strToken == "end")
	{
		Advance();
	}
	else
	{
		Error(8, "");//ȱ��end
		//Advance();
	}
	/*while (strToken != ";" && !statement_first.count(strToken) && strToken != "else" && strToken != "end")
	{
		Error(3, ";");//ȱ��;
		Statement();
	}
	if (strToken == "end")
		Advance();
	else
	{
		Error(8, "end");//ȱ��end
		while (!body_follow.count(strToken))
			Advance();
	}*/
}
void Grammar_Analyzer::Statement()
{
	/*<statement> -> <id> : = <exp>
					 |if <lexp> then <statement>[else <statement>]
					 |while <lexp> do <statement>
					 |call <id> ([<exp>{,<exp>}])
					 |<body>
					 |read (<id>{��<id>})
					 |write (<exp>{,<exp>})*/
	if (str_code == 2  && (strToken_next == ":=" || strToken_next == "="))
	{
		Id();
		if (strToken == ":=")
		{
			Advance();
			Exp();
		}
		else if (strToken == "=")
		{
			Error(9, "");//�Ƿ���ֵ
			Advance();
			Exp();
		}
	}
	else if (str_code == 2 || lexp_fisrt.count(strToken) || statement_first.count(strToken) )
	{
		int tag = 0;
		if (!statement_first.count(strToken) && strToken != "then" && strToken != "do" && strToken != "(" && strToken_next != "\EOF")
		{
			Error(2, "");
			cout << "����ͬ����������";
			while (!statement_first.count(strToken) && strToken != "then" && strToken != "do" && strToken != "(" && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << " ����������..." << endl;
		}
		if (strToken == "if" || strToken == "while")
		{
			if (strToken == "if") tag = 1;
			else tag = 2;
			Advance();
			LExp();
		}
		else if (strToken == "call")
		{
			tag == 3;
			Advance();
			Id();
		}
		else if (strToken == "begin")
		{
			tag == 4;
			Advance();
			Body();
		}
		else if (strToken == "read" || strToken == "write")
		{
			if (strToken == "read") tag = 5;
			else tag = 6;
			Advance();
		}

		if (strToken == "then" )
		{
			if (!tag)
				Error(2, "if");
			Advance();
			while (str_code != 2 && !statement_first.count(strToken))
			{
				Error(12, strToken);
				Advance();
			}
			Statement();
			if (strToken == "else")
			{
				Advance();
				Statement();
			}
		}
		else if (strToken == "do")
		{
			if (!tag)
				Error(2, "while");
			Advance();
			Statement();
		}
		else if (strToken == "(" || statement_first.count(strToken) || str_code == 2)
		{
			if (!tag)
			{
				Error(12, strToken);
				return;
			}
			else if (tag == 1)
			{
				Error(2, "then");
				Statement();
				if (strToken == "else")
				{
					Advance();
					Statement();
				}
			}
			else if (tag == 2)
			{
				Error(2, "do");
				Statement();
			}
			else if (tag == 3)
			{
				if (str_code == 2 && strToken_next == "(")
				{
					Id();
					Advance();
					Exp();
					while (strToken != ")")
					{
						if (strToken == "," && (exp_first.count(strToken_next)|| str_code_next == 2))
						{
							Advance();
							Exp();
						}
						else
						{
							Error(12, strToken);
							Advance();
						}
					}
					if (strToken == ")")
						Advance();
					else
						Error(6, ")");
				}
			}
			else if (tag == 4)
			{
				Error(7, "begin");
				Statement();
			}
			else if(tag == 5)
			{
				Advance();
				Id();
				if (strToken != ")")
				{
					while (strToken != ")")
					{
						if (strToken == "," && str_code_next == 2)
						{
							Advance();
							Id();
						}
						else if (str_code == 2 && strToken_next == ")")
							Id();
						else
						{
							Error(12, strToken);
							Advance();
						}
					}
				}
				Advance();
			}
			else if (tag == 6)
			{
				Advance();
				Exp();
				if (strToken != ")")
				{
					while (strToken != ")")
					{
						if (strToken == "," && (exp_first.count(strToken_next)||str_code_next == 2))
						{
							Advance();
							Id();
						}
						else
						{
							Error(12, strToken);
							Advance();
						}
					}
				}
				Advance();
			}
		}

	}
	/*else if (strToken == "if" || lexp_fisrt.count(strToken_next))
	{
		if (strToken != "if" && strToken != "then" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
		{
			Error(12, "");
			cout << "����ͬ����������";
			while (strToken != "if" && strToken != "then" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << " ����������..." << endl;
		}
		if (strToken == "if")
		{
			Advance();
			LExp();
		}
		if (strToken == "then" || str_code == 2 || statement_first.count(strToken_next))
		{
			if (strToken != "then")
				Error(2, "then");//ȱ��then
			Advance();
			Statement();
			while (strToken == "else")
			{
				Advance();
				Statement();
			}
		}
	}
	else if (strToken == "while" || lexp_fisrt.count(strToken_next))
	{
		if (strToken != "while" && strToken != "do" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
		{
			Error(12, "");
			cout << "����ͬ����������";
			while (strToken != "while" && strToken != "do" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "ͬ������" << strToken << " ����������..." << endl;
		}
		if (strToken == "while")
		{
			Advance();
			LExp();
		}
		if (strToken == "do" || str_code == 2 || statement_first.count(strToken_next))
		{
			if (strToken != "do")
				Error(2, "do");//ȱ��then
			Advance();
			Statement();
		}
	}
	else if (strToken == "call"||str_code_next == 2)
	{
		if(strToken!="call")
			Error(2, "call");//������ƴд����
		Advance();
		Id();
		bool l_bracket = false;
		while (!id_follow.count(strToken))
			Advance();
		if (strToken == "("||exp_follow.count(strToken))
		{
			if (strToken == "(")
			{
				l_bracket = true;
				Advance();
			}
			Exp();
		}
		while (strToken == ",")
		{
			Advance();
			Exp();
		}
		if (strToken == ")")
		{
			Advance();
			if (!l_bracket)
				Error(5, "(");//ȱ��(
		}
		else
		{
			Error(6, "(");//ȱ��)
			while (strToken != ";" && strToken != "end" && strToken != "else")
				Advance();
		}
	}
	else if (strToken == "begin")
	{
		Body();
	}
	else if(strToken == "read"||strToken_next == "(")
	{
		if(strToken != "read")
			Error(1, "read");//������ƴд����
		Advance();
		bool l_bracket = false;
		if (strToken == "("||str_code == 2)
		{
			if (strToken == "(")
			{
				l_bracket = true;
				Advance();
			}
			Id();
		}
		while (strToken == ",")
		{
			Advance();
			Id();
		}
		if (strToken == ")")
		{
			Advance();
			if (!l_bracket)
				Error(5, "(");//ȱ��(
		}
		else
		{
			Error(6, ")");//ȱ��)
			while (strToken != ";" && strToken != "end" && strToken != "else")
				Advance();
		}
	}
	else if (strToken == "writer"||strToken_next == "(")
	{
		if (strToken != "writer")
			Error(1, "writer");//������ƴд����
		Advance();
		bool l_bracket = false;
		if (strToken == "("||exp_first.count(strToken))
		{
			if (strToken == "(")
			{
				Advance();
				l_bracket = true;
			}
			Exp();
		}
		while (strToken == ",")
		{
			Advance();
			Exp();
		}
		if (strToken == ")")
		{
			Advance();
			if (!l_bracket)
				Error(5, "(");//ȱ��(
		}
		else
		{
			Error(6, ")");//ȱ��)
			while (strToken != ";" && strToken != "end" && strToken != "else")
				Advance();
		}
	}
	else
		Error(20,"");//ֹͣ����*/
}
void Grammar_Analyzer::LExp()
{
	//<lexp> �� <exp> <lop> <exp>|odd <exp>
	if (strToken == "odd")
	{
		while(!exp_first.count(strToken) && str_code != 1 && str_code != 2)
			Advance();
		Exp();
	}
	else
	{
		//while (!exp_first.count(strToken))
			//Advance();
		Exp();
		while (!exp_follow.count(strToken))
		{
			Advance();
		}
		Lop();
		while (!lop_follow.count(strToken)&&str_code!=1&&str_code!=2)
		{
			Advance();
		}
		Exp();
	}
}
void Grammar_Analyzer::Exp()
{
	//<exp> �� [+|-]<term>{<aop><term>}
	while (!exp_first.count(strToken)&&str_code!=1&&str_code!=2)
		Advance();
	if (strToken == "+" || strToken == "-")
		Advance();
	Term();
	while (strToken == "+" || strToken == "-")
	{
		Advance();
		Term();
	}
//	while (!exp_follow.count(strToken))
		//Advance();
}
void Grammar_Analyzer::Term()
{
	//<term> �� <factor>{<mop><factor>}
	while(str_code!=1 && str_code!=2 && strToken!="(")
		Advance();
	Factor();
	while (strToken == "*" || strToken == "/")
	{
		Advance();
		Factor();
	}
}
void Grammar_Analyzer::Factor()
{
	//<factor>��<id>|<integer>|(<exp>)
	while (str_code != 1 && str_code != 2 && strToken != "(")
		Advance();
	if (str_code == 2)
		Id();
	else if (str_code == 1)
	{
		Advance();
	}
	else if (strToken == "(")
	{
		Advance();
		Exp();
	}
}
void Grammar_Analyzer::Lop()
{
	//<lop> �� =|<>|<|<=|>|>=
	if (strToken == "=" || strToken == "<>" || strToken == "<" || strToken == "<=" || strToken == ">" || strToken == ">=")
		Advance();
	else
	{
		Error(12, strToken);
		Advance();
	}
}
void Grammar_Analyzer::Id()
{
	//<id> �� l{l|d}
	if (str_code == 2)
		Advance();
}
int Grammar_Analyzer::Error(int ecode, string str)
{
	if (ecode == 1)//������ƴд����
		cout << "line:" << str_row << "	�ؼ���" << str << "ƴд����" << endl;
	else if (ecode == 2)//ȱ�ٱ�����
		cout << "line:" << str_row << "	ȱ�ٹؼ���:" << str << endl;
	else if (ecode == 3)//ȱ��;
		cout << "line:" << str_row - 1 << "	ȱ�� ; ��" << endl;
	else if (ecode == 4)//ȱ��,
		cout << "line:" << str_row << "	ȱ�� , ��" << endl;
	else if (ecode == 5)//ȱ��(
		cout << "line:" << str_row << "	ȱ�� ( ��" << endl;
	else if (ecode == 6)
		cout << "line:" << str_row << "	ȱ�� ) ��" << endl;
	else if (ecode == 7)
		cout << "line:" << str_row - 1 << "	ȱ�� begin ��" << endl;
	else if (ecode == 8)
		cout << "line:" << str_row << "	ȱ�� end ��" << endl;
	else if (ecode == 9)
		cout << "line:" << str_row << "	�Ƿ���ֵ�� = ��" << endl;
	else if (ecode == 10)//�������
		cout << "������룡" << endl;
	else if (ecode == 11)
		cout << "line:" << str_row << "	ȱ��id��" << endl;
	else if (ecode == 12)
		cout << "line:" << str_row << "	�����ַ�: " << str <<endl;
	else if (ecode == 13)
		cout << "line:" << str_row << "	ȱ�����֣�" << endl;
	else
	{
		cout << "��ֹ����" << endl;
		exit(0);
	}
}