#include "Interpreter.h"
#include <string.h>
#include <iostream>
using namespace std;

class SyntaxException {};

int Interpreter::interpreter(string s)
{

	int tmp = 0;
	string word;

	word = getWord(s, &tmp);
	if (strcmp(word.c_str(), "create") == 0)
	{
		word = getWord(s, &tmp);

		if (strcmp(word.c_str(), "table") == 0)
		{
			string primaryKey = "";
			string tableName = "";
			word = getWord(s, &tmp);
			if (!word.empty())			//create table tablename
				tableName = word;
			else
			{
				cout << "Syntax Error for no table name" << endl << endl;
				return 0;
			}

			word = getWord(s, &tmp);
			if (word.empty() || strcmp(word.c_str(), "(") != 0)
			{
				cout << "Error in syntax1!" << endl << endl;
				return 0;
			}
			else				// deal with attribute list
			{
				word = getWord(s, &tmp);
				vector<Catalog_manager::Column> cols;
				Catalog_manager::Column col;
				while (!word.empty() && strcmp(word.c_str(), "primary") != 0 && strcmp(word.c_str(), ")") != 0)
				{
					col.column_name = word;
					// deal with the data type
					word = getWord(s, &tmp);
					if (strcmp(word.c_str(), "int") == 0)
					{
						col.type = 1;
						col.len = sizeof(int);
					}
					else if (strcmp(word.c_str(), "float") == 0)
					{
						col.type = 2;
						col.len = sizeof(float);
					}
					else if (strcmp(word.c_str(), "char") == 0)
					{
						col.type = 0;
						word = getWord(s, &tmp);
						if (strcmp(word.c_str(), "("))
						{
							cout << "Syntax Error: unknown data type" << endl << endl;
							return 0;
						}
						word = getWord(s, &tmp);
						istringstream convert(word);
						if (!(convert >> col.len))
						{
							cout << "Syntax error : illegal number in char()" << endl << endl;
							return 0;
						}
						col.len += 1;
						word = getWord(s, &tmp);
						if (strcmp(word.c_str(), ")"))
						{
							cout << "Syntax Error: unknown data type" << endl << endl;
							return 0;
						}
					}
					else
					{
						cout << "Syntax Error: unknown or missing data type!" << endl << endl;
						return 0;
					}
					word = getWord(s, &tmp);
					col.is_unique = false;
					if (strcmp(word.c_str(), "unique") == 0)
					{
						col.is_unique = true;
						word = getWord(s, &tmp);
					}
					col.is_pri = false;
					cols.push_back(col);
					if (strcmp(word.c_str(), ",") != 0)
					{
						if (strcmp(word.c_str(), ")") != 0) {
							cout << "Syntax Error for ,!" << endl << endl;
							return 0;
						}
						else
							break;
					}
					word = getWord(s, &tmp);
				}
				if (strcmp(word.c_str(), "primary") == 0)	// deal with primary key
				{
					word = getWord(s, &tmp);
					if (strcmp(word.c_str(), "key") != 0)
					{
						cout << "Error in syntax!" << endl << endl;
						return 0;
					}
					else
					{
						word = getWord(s, &tmp);
						if (strcmp(word.c_str(), "(") == 0)
						{
							word = getWord(s, &tmp);
							primaryKey = word;
							int i = 0;
							for (i = 0; i<cols.size(); i++)
							{
								if (primaryKey == cols[i].column_name)
								{
									cols[i].is_pri = true;
									cols[i].is_unique = true;
									break;
								}

							}
							if (i == cols.size())
							{
								cout << "Syntax Error: primaryKey does not exist in attributes " << endl << endl;
								return 0;
							}
							word = getWord(s, &tmp);
							if (strcmp(word.c_str(), ")") != 0)
							{
								cout << "Error in syntax3!" << endl << endl;
								return 0;
							}
						}
						else
						{
							cout << "Error in syntax4!" << endl << endl;
							return 0;
						}
						word = getWord(s, &tmp);
						if (strcmp(word.c_str(), ")") != 0)
						{
							cout << "Error in syntax5!" << endl << endl;
							return 0;
						}
					}
				}
				else if (word.empty())
				{
					cout << "Syntax Error: ')' absent!" << endl << endl;
					return 0;
				}

				api->createTable(tableName, cols);
				return 1;
			}
		}
		else if (strcmp(word.c_str(), "index") == 0)
		{
			string indexName = "";
			string tableName = "";
			string attributeName = "";
			//获取要创建的索引名称
			word = getWord(s, &tmp);
			if (!word.empty())
				indexName = word;
			else
			{
				cout << "Error in syntax6!" << endl << endl;
				return 0;
			}
			//获得on
			word = getWord(s, &tmp);
			if (strcmp(word.c_str(), "on") != 0)
			{
				cout << "Error in syntax for lacking 'on' when creating indexes!" << endl << endl;
				return 0;
			}
			//获得表格名称
			word = getWord(s, &tmp);
			if (word.empty())
			{
				cout << "Error in syntax for lacking table when creating indexes!" << endl << endl;
				return 0;
			}
			tableName = word;
			//获取（字段名）格式的内容
			word = getWord(s, &tmp);
			if (strcmp(word.c_str(), "(") != 0)
			{
				cout << "Error in syntax for lacking ( before the column when creating indexes!" << endl << endl;
				return 0;
			}
			//获取字段名称
			word = getWord(s, &tmp);
			if (word.empty())
			{
				cout << "Error in syntax for lacking column when creating indexes!" << endl << endl;
				return 0;
			}
			attributeName = word;
			//获取）
			word = getWord(s, &tmp);
			if (strcmp(word.c_str(), ")") != 0)
			{
				cout << "Error in syntax for lacking ) after the column when creating indexes!" << endl << endl;
				return 0;
			}
			api->createIndex(indexName, tableName, attributeName);
			return 1;
		}
		else
		{
			cout << "Syntax Error for " << word << endl << endl;
			return 0;
		}
		return 0;
	}



	else if (strcmp(word.c_str(), "select") == 0)
	{
		string tableName = "";
		word = getWord(s, &tmp);
		//不是选择所有记录
		if (strcmp(word.c_str(), "*") != 0)
		{
			cout << "Error for we can only select *" << endl << endl;
		}
		//选择所有记录
		else
		{
			word = getWord(s, &tmp);
		}
		if (strcmp(word.c_str(), "from") != 0)
		{
			cout << "Error in syntax7!" << endl << endl;
			return 0;
		}
		//获取表格名称
		word = getWord(s, &tmp);
		if (!word.empty())
			tableName = word;
		else
		{
			cout << "Error in syntax8!" << endl << endl;
			return 0;
		}

		// 没有查询条件的情况
		word = getWord(s, &tmp);
		if (word.empty())
		{
			api->printRecord(tableName);
			return 1;
		}
		else if (strcmp(word.c_str(), "where") == 0)
		{
			vector<string> attribute;
			vector<string> op;
			vector<string> cond;
			string attr = "";
			string operate = "";
			string con = "";
			//获得第一个字段名
			word = getWord(s, &tmp);
			while (1) {
				if (word.empty()) {
					cout << "Error for lacking column when selecting" << endl << endl;
					return 0;
				}
				attr = word;
				word = getWord(s, &tmp);
				//获得操作符
				if (word.empty())
				{
					cout << "Error for lacking operation when selecting" << endl << endl;
					return 0;
				}
				operate = word;
				word = getWord(s, &tmp);
				//获得条件
				if (word.empty())
				{
					cout << "Error for lacking condition when selecting" << endl << endl;
					return 0;
				}
				con = word;
				//这一组条件压入容器
				attribute.push_back(attr);
				op.push_back(operate);
				cond.push_back(con);
				//再次读入命令，看是否条件结束
				word = getWord(s, &tmp);
				//条件结束
				if (word.empty())
					break;
				if (strcmp(word.c_str(), "and") != 0)
				{
					cout << "Error between the two conditions" << endl << endl;
					return 0;
				}
				//读入下一个条件的字段名
				word = getWord(s, &tmp);
			}
			api->printRecord(tableName, attribute, op, cond);
			return 1;
		}
	}



	else if (strcmp(word.c_str(), "drop") == 0)
	{
		word = getWord(s, &tmp);
		//删除表格
		if (strcmp(word.c_str(), "table") == 0)
		{
			//获得表格名称
			word = getWord(s, &tmp);
			if (!word.empty())
			{
				api->dropTable(word);
				return 1;
			}
			else
			{
				cout << "Error for lacking table when dropping!" << endl << endl;
				return 1;
			}
		}
		else if (strcmp(word.c_str(), "index") == 0)
		{
			word = getWord(s, &tmp);
			if (!word.empty())
			{
				api->dropIndex(word);
				return 1;
			}
			else
			{
				cout << "Error for lacking index when dropping!" << endl << endl;
				return 1;
			}
		}
		else
		{
			cout << "Error for unknown thing to drop!" << endl << endl;
			return 0;
		}
	}


	else if (strcmp(word.c_str(), "delete") == 0)
	{
		string tableName = "";
		word = getWord(s, &tmp);
		if (strcmp(word.c_str(), "from") != 0)
		{
			cout << "Error for lacking from when deleting!" << endl << endl;
			return 0;
		}
		//获得表格名称
		word = getWord(s, &tmp);
		if (!word.empty())
			tableName = word;
		else
		{
			cout << "Error for lacking table when deleting!" << endl << endl;
			return 0;
		}
		//获得是否有条件
		word = getWord(s, &tmp);
		if (word.empty())	//无条件删除
		{
			api->deleteValue(tableName);
			return 1;
		}
		else if (strcmp(word.c_str(), "where") == 0)
		{
			vector<string> attribute;
			vector<string> op;
			vector<string> cond;
			string attr = "";
			string operate = "";
			string con = "";
			//获得第一个字段名
			word = getWord(s, &tmp);
			while (1) {
				if (word.empty()) {
					cout << "Error for lacking column when deleting" << endl << endl;
					return 0;
				}
				attr = word;
				word = getWord(s, &tmp);
				//获得操作符
				if (word.empty())
				{
					cout << "Error for lacking operation when deleting" << endl << endl;
					return 0;
				}
				operate = word;
				word = getWord(s, &tmp);
				//获得条件
				if (word.empty())
				{
					cout << "Error for lacking condition when deleting" << endl << endl;
					return 0;
				}
				con = word;
				//这一组条件压入容器
				attribute.push_back(attr);
				op.push_back(operate);
				cond.push_back(con);
				//再次读入命令，看是否条件结束
				word = getWord(s, &tmp);
				//条件结束
				if (word.empty())
					break;
				if (strcmp(word.c_str(), "and") != 0)
				{
					cout << "Error between the two conditions when deleting" << endl << endl;
					return 0;
				}
				//读入下一个条件的字段名
				word = getWord(s, &tmp);
			}
			api->deleteValue(tableName, attribute, op, cond);
			return 1;
		}
	}

	else if (strcmp(word.c_str(), "insert") == 0)
	{
		string tableName = "";
		vector<string> valueVector;
		word = getWord(s, &tmp);
		if (strcmp(word.c_str(), "into") != 0)
		{
			cout << "Error for lacking into when inserting" << endl << endl;
			return 0;
		}
		//获得表格名称
		word = getWord(s, &tmp);
		if (word.empty())
		{
			cout << "Error for lacking table when inserting" << endl << endl;
			return 0;
		}
		tableName = word;
		//获得后面的values
		word = getWord(s, &tmp);
		if (strcmp(word.c_str(), "values") != 0)
		{
			cout << "Error for lacking values when inserting" << endl << endl;
			return 0;
		}
		word = getWord(s, &tmp);
		if (strcmp(word.c_str(), "(") != 0)
		{
			cout << "Error for lacking ( when inserting" << endl << endl;
			return 0;
		}
		word = getWord(s, &tmp);
		while (!word.empty() && strcmp(word.c_str(), ")") != 0)
		{
			valueVector.push_back(word);
			word = getWord(s, &tmp);
			if (strcmp(word.c_str(), ",") == 0)  // bug here
				word = getWord(s, &tmp);
		}
		if (strcmp(word.c_str(), ")") != 0)
		{
			cout << "Error for lacking ) when inserting" << endl << endl;
			return 0;
		}
		api->insertRecord(tableName, valueVector);
		return 1;
	}
	
	else if (strcmp(word.c_str(), "quit") == 0)
	{
		api->closeDatabase();
	}
	else
	{
		cout << "Wrong SQL instruction!" << endl << endl;
	}

	return 0;


}



string Interpreter::getWord(string s, int *tmp)
{
	string word;
	int idx1, idx2;

	while ((s[*tmp] == ' ' || s[*tmp] == 10 || s[*tmp] == '\t') && s[*tmp] != 0)
	{
		(*tmp)++;
	}
	idx1 = *tmp;

	if (s[*tmp] == '(' || s[*tmp] == ',' || s[*tmp] == ')')
	{
		(*tmp)++;
		idx2 = *tmp;
		word = s.substr(idx1, idx2 - idx1);
		if (word.size()>0 && word.at(word.size() - 1) == ';')
			word = word.substr(0, word.length() - 1);
		return word;
	}
	else if (s[*tmp] == 39)
	{
		(*tmp)++;
		while (s[*tmp] != 39 && s[*tmp] != 0)
			(*tmp)++;
		if (s[*tmp] == 39)
		{
			idx1++;
			idx2 = *tmp;
			(*tmp)++;
			word = s.substr(idx1, idx2 - idx1);
			if (word.size()>0 && word.at(word.size() - 1) == ';')
				word = word.substr(0, word.length() - 1);
			return word;
		}
		else
		{
			word = "";
			return word;
		}
	}
	else
	{
		while (s[*tmp] != ' ' &&s[*tmp] != '(' && s[*tmp] != 10 && s[*tmp] != 0 && s[*tmp] != ')' && s[*tmp] != ',')
			(*tmp)++;
		idx2 = *tmp;
		if (idx1 != idx2)
			word = s.substr(idx1, idx2 - idx1);
		else
			word = "";
		if (word.size()>0 && word.at(word.size() - 1) == ';')
			word = word.substr(0, word.length() - 1);
		return word;
	}
}