#include <string>
#include <iostream>
#include "DB.hpp"
#include "Interpreter.h"

int GetOneSql(ifstream &ifs, string &str)
{
	char currch;
	str.erase();
	while (!ifs.eof())
	{
		ifs.get(currch);
		if (ifs.bad())//there is an unrecoverable error.
			return -1;
		str += currch;
		if (currch == ';') {
			if (!str.empty()) {
				str.erase(0, str.find_first_not_of("\n"));
				str.erase(str.find_last_not_of("\n") + 1);
			}
			return str.length();
		}
	}
	return -1; //reach the end of file and don't encounter ';', ingnore the final sql.
}

int main() {
	DB db;
	db.run();
	Interpreter ip;
	string s, s1, word;
	bool quit = false;
	while (1)
	{
		cin >> word;  //�ж��Ƿ���execfile���
		if (word == "execfile") {
			cin >> word;  //�õ��ļ���
			if (word.size()>0 && word.at(word.size() - 1) == ';')
				word = word.substr(0, word.length() - 1);
			ifstream inf(word);
			if (!inf) {
				cout << "fail to open file" << endl << endl;
			}
			else {
				string sline;//ÿһ��
				string sql; //ÿ��sql���
				while (!inf.eof())
				{
					if (GetOneSql(inf, sql) > 0)
					{
						cout << sql << endl; //����Ļ�����ִ�е�sql���
						ip.interpreter(sql); //ִ��sql���
					}						
					else {
						cout << "execute file finished." << endl << endl;
						break;
					}
					if (sql == "quit;")
					{
						quit = true;
						break;
					}						
					sql.erase();
				}
			}

		}
		else {
			s1 = word + " ";
			s = word;
			while (s.size() > 0 && s.at(s.size() - 1) != ';') {
				getline(cin, s);
				s1 += s;
			}
			ip.interpreter(s1);
			if (s == "quit;")
			{
				quit = true;
				break;
			}
			s1.clear();

		}
		if (quit)
			break;
	}

}
