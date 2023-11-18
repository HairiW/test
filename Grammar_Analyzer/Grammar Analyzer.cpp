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
		cout << "打开文件失败！" << endl;
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
		Error(1, "program");//保留字拼写错误
		Advance();
	}
	else if (strToken != "program")
		Error(2, "program");//缺少保留字
	else
		Advance();
	if (str_code != 2)
		Error(11, "");//缺少id
	else
		Id();
	if (strToken != ";" && !block_first.count(strToken) && strToken_next != "\EOF")
	{
		Error(12, strToken);//多余字符
		cout << "正在同步中，跳过：";
		while (strToken != ";" && !block_first.count(strToken) && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
	}
	if (strToken == ";")
	{
		Advance();
		Block();
	}
	else
	{
		Error(3, ";");//缺少;
		Block();
	}
	//if (strToken != "\EOF")
		//Error(10, "");//多余代码
	cout << "编译完成" << endl;
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
			cout << "正在同步，跳过：";
			while (!block_first.count(strToken) && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << "	继续同步中..." << endl;
		}
		if (strToken == "const")
			Condecl();
		if (!block_first.count(strToken) && strToken_next != "\EOF")
		{
			Error(2, "var/procedure/begin");
			cout << "正在同步，跳过：";
			while (!block_first.count(strToken) && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << "	继续同步中..." << endl;
		}
		if (strToken == "var")
			Vardecl();
		if (!block_first.count(strToken))
		{
			Error(2, "procedure/begin");
			cout << "正在同步，跳过：";
			while (!block_first.count(strToken))
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << "	继续同步中..." << endl;
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
		cout << "正在同步，跳过：";
		while (str_code != 2 && strToken != ";" && strToken != "var" && strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
			cout << "正在同步中，跳过：";
			while (strToken != "var" && strToken != "procedure" && strToken != "begin" && strToken != ";")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
		}
		while (str_code == 2 && strToken_next == ":=")//<const>之间缺少","
		{
			Error(4, ",");
			Const();
		}
	}
	if (strToken == ";")//正常运行
		Advance();
	else if (strToken == "var" || strToken == "procedure" || strToken == "begin")//缺少;
		Error(3, ";");//缺少;
	else//缺少";"且含有多余字符
	{
		Error(3, ";");//缺少;
		Error(12, strToken);//含有多余字符
		cout << "正在同步中，跳过：";
		while (strToken != "var" && strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
		Error(9, "");//非法赋值
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
		cout << "正在同步，跳过：";
		while (str_code != 2 && strToken != ";" && strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
			cout << "正在同步中，跳过：";
			while (strToken != "procedure" && strToken != "begin" && strToken != ";")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
		}
		while (str_code == 2)//<id>之间缺少","
		{
			Error(4, ",");
			Id();
		}
	}
	
	if (strToken == ";")//正常运行
		Advance();
	else if (strToken == "procedure" || strToken == "begin")//缺少;
		Error(3, ";");//缺少;
	else//缺少";"且含有多余字符
	{
		Error(3, ";");//缺少;
		Error(12, strToken);//含有多余字符
		cout << "正在同步中，跳过：";
		while (strToken != "procedure" && strToken != "begin")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
	}
	/*while (strToken != "," && strToken != ";")
	{
		if (str_code == 2)
		{
			Error(4, ",");//缺少,
			Id();
		}
		else
			Advance();
	}
	if (strToken == ";")
		Advance();
	else
	{
		Error(3, ";");//缺少;
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
		cout << "正在同步，跳过：";
		while (str_code != 2 && strToken != "("  && strToken != ")" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
		cout << "正在同步，跳过：";
		while (strToken != "("  && strToken != ")" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
				Error(4, ",");//缺少,
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
			Error(5, "(");//缺少(
		if (strToken == ";")
			Error(6, ")");//缺少)
		else
			Advance();
		Advance();
		/*if (!block_first.count(strToken))
		{
			Error(12, "");//多余字符
			cout << "正在同步中，跳过：";
			while (!block_first.count(strToken))
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
		Error(5, "");//缺少(
		Error(6, "");//缺少)
		Error(3, "");//缺少;
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
		cout << "正在同步，跳过：";
		while (strToken != "begin" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
		Error(7, "");//缺少begin
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
		cout << "正在同步，跳过：";
		while (strToken != "end" && strToken != "else" && strToken != ";" && strToken != "begin" && strToken_next != "\EOF")
		{
			cout << strToken << " ";
			Advance();
		}
		cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
	}
	if (strToken == "end")
	{
		Advance();
	}
	else
	{
		Error(8, "");//缺少end
		//Advance();
	}
	/*while (strToken != ";" && !statement_first.count(strToken) && strToken != "else" && strToken != "end")
	{
		Error(3, ";");//缺少;
		Statement();
	}
	if (strToken == "end")
		Advance();
	else
	{
		Error(8, "end");//缺少end
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
					 |read (<id>{，<id>})
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
			Error(9, "");//非法赋值
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
			cout << "正在同步，跳过：";
			while (!statement_first.count(strToken) && strToken != "then" && strToken != "do" && strToken != "(" && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
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
			cout << "正在同步，跳过：";
			while (strToken != "if" && strToken != "then" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
		}
		if (strToken == "if")
		{
			Advance();
			LExp();
		}
		if (strToken == "then" || str_code == 2 || statement_first.count(strToken_next))
		{
			if (strToken != "then")
				Error(2, "then");//缺少then
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
			cout << "正在同步，跳过：";
			while (strToken != "while" && strToken != "do" && !statement_first.count(strToken) && str_code != 2 && strToken_next != "\EOF")
			{
				cout << strToken << " ";
				Advance();
			}
			cout << endl << "同步至：" << strToken << " 继续编译中..." << endl;
		}
		if (strToken == "while")
		{
			Advance();
			LExp();
		}
		if (strToken == "do" || str_code == 2 || statement_first.count(strToken_next))
		{
			if (strToken != "do")
				Error(2, "do");//缺少then
			Advance();
			Statement();
		}
	}
	else if (strToken == "call"||str_code_next == 2)
	{
		if(strToken!="call")
			Error(2, "call");//保留字拼写错误
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
				Error(5, "(");//缺少(
		}
		else
		{
			Error(6, "(");//缺少)
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
			Error(1, "read");//保留字拼写错误
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
				Error(5, "(");//缺少(
		}
		else
		{
			Error(6, ")");//缺少)
			while (strToken != ";" && strToken != "end" && strToken != "else")
				Advance();
		}
	}
	else if (strToken == "writer"||strToken_next == "(")
	{
		if (strToken != "writer")
			Error(1, "writer");//保留字拼写错误
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
				Error(5, "(");//缺少(
		}
		else
		{
			Error(6, ")");//缺少)
			while (strToken != ";" && strToken != "end" && strToken != "else")
				Advance();
		}
	}
	else
		Error(20,"");//停止编译*/
}
void Grammar_Analyzer::LExp()
{
	//<lexp> → <exp> <lop> <exp>|odd <exp>
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
	//<exp> → [+|-]<term>{<aop><term>}
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
	//<term> → <factor>{<mop><factor>}
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
	//<factor>→<id>|<integer>|(<exp>)
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
	//<lop> → =|<>|<|<=|>|>=
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
	//<id> → l{l|d}
	if (str_code == 2)
		Advance();
}
int Grammar_Analyzer::Error(int ecode, string str)
{
	if (ecode == 1)//保留字拼写错误
		cout << "line:" << str_row << "	关键字" << str << "拼写错误！" << endl;
	else if (ecode == 2)//缺少保留字
		cout << "line:" << str_row << "	缺少关键字:" << str << endl;
	else if (ecode == 3)//缺少;
		cout << "line:" << str_row - 1 << "	缺少 ; ！" << endl;
	else if (ecode == 4)//缺少,
		cout << "line:" << str_row << "	缺少 , ！" << endl;
	else if (ecode == 5)//缺少(
		cout << "line:" << str_row << "	缺少 ( ！" << endl;
	else if (ecode == 6)
		cout << "line:" << str_row << "	缺少 ) ！" << endl;
	else if (ecode == 7)
		cout << "line:" << str_row - 1 << "	缺少 begin ！" << endl;
	else if (ecode == 8)
		cout << "line:" << str_row << "	缺少 end ！" << endl;
	else if (ecode == 9)
		cout << "line:" << str_row << "	非法赋值号 = ！" << endl;
	else if (ecode == 10)//多余代码
		cout << "多余代码！" << endl;
	else if (ecode == 11)
		cout << "line:" << str_row << "	缺少id！" << endl;
	else if (ecode == 12)
		cout << "line:" << str_row << "	错误字符: " << str <<endl;
	else if (ecode == 13)
		cout << "line:" << str_row << "	缺少数字！" << endl;
	else
	{
		cout << "终止编译" << endl;
		exit(0);
	}
}