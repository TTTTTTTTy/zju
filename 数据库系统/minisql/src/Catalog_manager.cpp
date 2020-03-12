#include"Catalog_manager.h"
#include "DB.hpp"

Catalog_manager::Catalog_manager()
{
	char tmp[126];
	int n = 0;
	fstream _file;
	_file.open("db.info", ios::in);
	if (!_file)
	{
		ofstream outfile("db.info");
		if (outfile.is_open()) cout << "db.info被创建" << endl << endl;
		outfile << 0 << endl;
		outfile << 0 << endl;
		outfile << "$end" << endl << endl;
	}
	_file.close();
	ifstream file("db.info");                          //创建字典文件对象
	if (!file)
	{
		cout << "Fail to open db.info when read dictionary information." << endl << endl;
		exit(0);
	}
	else
	{
		file >> table_num;
		file >> index_num;
		file >> tmp;
		while (strcmp(tmp, "$end") != 0)                      //判断是否读到文件尾
		{
			if (tmp[0] == '@')                               //读到表,接下去输入表的各种信息
			{
				Table T;                //创建表结构实体
										//将实体的内容读入
				T.table_name.assign(tmp, 1, strlen(tmp) - 1);
				file >> T.colu_num;

				for (int i = 0; i< T.colu_num; i++)            //读入字段名
				{
					Column c;
					file >> tmp;
					c.column_name.assign(tmp, 0, strlen(tmp));
					file >> c.type;
					file >> c.len;
					file >> c.is_pri;
					file >> c.is_unique;
					file >> tmp;
					string in;
					in.assign(tmp, 0, strlen(tmp));
					T.index.push_back(in);
					T.clolums.push_back(c);
				}

				file >> tmp;
				T.pri_key.assign(tmp, 0, strlen(tmp));     //读入关键字信息
				file >> T.record_num;                       //读入表中的记录数目
				file >> T.record_len;

				tables.push_back(T);                           //将表结构放入表列表中
			}
			else if (tmp[0] == '#')                            //读到索引文件，接下去输入索引文件的信息
			{
				Index *I = new Index;                   //创建一个索引的结构实体
				I->index_name.assign(tmp, 1, strlen(tmp) - 1);
				file >> tmp;
				I->index_table.assign(tmp, 0, strlen(tmp));
				file >> tmp;
				I->index_col.assign(tmp, 0, strlen(tmp));
				file >> I->freeNum;
				file >> I->root;
				file >> I->type;
				file >> I->len;
				indexs.push_back(*I);                           //将索引结构放入索引列表中

			}
			else                                           //错误提示
			{
				cout << "Error！Fail to read db.info." << endl << endl;
				return;
			}
			file >> tmp;
		}
	}
}

//把内容写入文件
int  Catalog_manager::Write_in()
{
	const char * fileName = "db.info";
	ofstream ofile(fileName);
	if (!ofile)
	{
		cout << "Fail to open db.info when write";
		return 0;
	}
	else
	{
		ofile << table_num << endl;
		ofile << index_num << endl << endl;
		for (int i = 0; i < table_num; i++)
		{
			ofile << "@" << tables[i].table_name << endl;
			ofile << tables[i].colu_num << endl;
			for (int j = 0; j < tables[i].colu_num; j++)
			{
				ofile << tables[i].clolums[j].column_name << " ";
				ofile << tables[i].clolums[j].type << " ";
				ofile << tables[i].clolums[j].len << " ";
				ofile << tables[i].clolums[j].is_pri << " ";
				ofile << tables[i].clolums[j].is_unique << endl;
				ofile << tables[i].index[j] << endl;
			}
			ofile << tables[i].pri_key << endl;
			ofile << tables[i].record_num << endl;
			ofile << tables[i].record_len << endl << endl;
		}
		cout << "\n";
		for (int i = 0; i < index_num; i++)
		{
			ofile << "#" << indexs[i].index_name << " " << indexs[i].index_table << " " << indexs[i].index_col << " " << indexs[i].freeNum << " " << indexs[i].root
				<< " " << indexs[i].type << " " << indexs[i].len << endl << endl;
		}
		ofile << "$end";
	}
	return 1;
}

//使用给定的名称和字段创建表格
int Catalog_manager::Create_table(string name, vector<Column>&cols)
{
	if (Table_exist(name)) {
		cout << "Error! Table " << name << " already exists." << endl;
		return 0;
	}

	Table *T = new Table;
	T->table_name = name;
	T->clolums = cols;
	T->colu_num = cols.size();
	T->record_len = Calculate_clos(cols);
	T->record_num = 0;
	for (int i = 0; i < T->colu_num; i++)
		T->index.push_back("NULL");
	table_num++;
	int flag = 0;
	vector<Column>::iterator it;
	string indexName;
	string col;
	for (it = cols.begin(); it != cols.end(); it++)
		if (it->is_pri)
		{
			T->pri_key = it->column_name;
			col = it->column_name;
			indexName = name + it->column_name;
			flag = 1;
			break;
		}
	if (flag == 0)
		T->pri_key = "NULL";
	tables.push_back(*T);
	if (T->pri_key != "NULL")
		Create_index(indexName, col, name);
	cout << "Successfully create table " << name << " ." << endl << endl;
	return 0;
}

//对给定表格的字段创建索引
int Catalog_manager::Create_index(string name, string column, string table)
{
	int i;
	int n = tables.size();
	Index I;
	for (i = 0; i < n; i++)
	{
		if (tables[i].table_name == table)
		{
			int j;
			for (j = 0; j < tables[i].clolums.size(); j++)
			{
				if (tables[i].clolums[j].column_name == column)
				{
					//判断索引是否已经存在，如果存在则输出提示，返回0
					if (tables[i].index[j] != "NULL")
					{
						cout << "Fail to create index, for the index already exists." << endl << endl;
						return 0;
					}
					else if (!tables[i].clolums[j].is_unique)
					{
						cout << "Error for the column is not unique!" << endl << endl;
						return 0;
					}
					tables[i].index[j] = name;
					I.type = tables[i].clolums[j].type;
					I.len = tables[i].clolums[j].len;
					break;
				}
			}
			if (j == tables[i].clolums.size())
			{
				cout << "Fail to create index, for coll " << column << " does not exist." << endl << endl;
				return 0;
			}
			break;
		}
	}
	if (i == n)
	{
		cout << "Fail to create index, for table " << table << " does not exist." << endl << endl;
		return 0;
	}
	I.index_name = name;
	I.index_col = column;
	I.index_table = table;
	I.freeNum = -1;
	I.root = -1;
	indexs.push_back(I);
	index_num++;
	im->CreateIndex(table, column, name);
	cout << "Create index of " << column << " in table" << table << " successfully" << endl << endl;
	return 1;

}

//删除指定索引
int Catalog_manager::Delete_index(string name)
{
	if (!Index_exist(name)) {
		cout << "Error! Index " << name << " does not exsit!" << endl << endl;
		return 0;
	}
	int i;
	int n = indexs.size();
	for (i = 0; i < n; i++)
	{
		if (indexs[i].index_name == name) {
			int m = Table_pos(indexs[i].index_table);
			for (int j = 0; j<tables[m].index.size(); j++)
			{
				if (tables[m].clolums[j].column_name == indexs[i].index_col)
				{
					tables[m].index[j] = "NULL";
					break;
				}
			}
			indexs.erase(i + indexs.begin());
			index_num--;
			im->DeleteIndex(name);
			cout << "Drop index  " << name << " successfully." << endl << endl;
			return 1;
		}
	}
	cout << "Error! Index " << name << " does not exists!" << endl << endl;
	return 0;

}

//删除指定表格上的索引
int Catalog_manager::Delete_index_ontable(string table)
{
	int i;
	for (i = 0; i < indexs.size(); i++)
	{
		if (indexs[i].index_table == table) {
			string name = indexs[i].index_name;
			Delete_index(name);
			i = -1;
		}
	}
	return 0;
}


//删除给定名称的表格
int Catalog_manager::Delete_table(string name)
{
	if (!Table_exist(name)) {
		cout << "Error! Table " << name << " does not exsit!" << endl << endl;
		return 0;
	}
	int i;
	int n = tables.size();
	for (i = 0; i < n; i++) {
		if (tables[i].table_name == name) {
			Delete_index_ontable(name);
			tables.erase(tables.begin() + i);
			table_num--;
			cout << "Drop table " << name << " successfully" << endl << endl;
			return 1;
		}
	}
}

//返回指定表格字段上的索引
Catalog_manager::Index &Catalog_manager::Get_index(string table, string column)
{
	string name = table + "-" + column;
	int n = indexs.size();
	int i;
	for (i = 0; i < n; i++)
	{
		if ((indexs[i].index_table == table) && (indexs[i].index_col == column)) {
			return indexs[i];
		}
	}
}

//判断指定表格字段上是否存在索引
bool Catalog_manager::has_index(string table, string colName)
{
	int m = Table_pos(table);
	for (int i = 0; i < tables[m].clolums.size(); i++)
	{
		if (tables[m].clolums[i].column_name == colName)
		{
			if (tables[m].index[i] != "NULL")
				return true;
			else
				return false;
		}
	}
	return false;
}



//插入记录
int Catalog_manager::Insert_rec(string table, int num)
{
	int m = Table_pos(table);
	if (m == -1) {
		cout << "Fail to insert records, for table " << table << " does not exsit." << endl << endl;
		return 0;
	}
	tables[m].record_num += num;
	return 1;
}

//删除表格中所有记录
int Catalog_manager::Delete_all(string name)
{
	int m = Table_pos(name);
	tables[m].record_num = 0;
	return 1;
}

//删除表格中指定条记录
int Catalog_manager::Delect_recs(string name, int num)
{
	int m = Table_pos(name);
	if (tables[m].record_num < num) {
		cout << "Error! There are not enough records to erase!" << endl;
		return 0;
	}
	tables[m].record_num -= num;
	return 1;

}

//查找文件判断该表格是否存在
bool Catalog_manager::Table_exist(string name)
{
	int i;
	int n = tables.size();
	for (i = 0; i < n; i++)
		if (tables[i].table_name == name)
			return true;
	return false;
}

//查找文件判断该索引是否存在
bool Catalog_manager::Index_exist(string name)
{
	int i;
	int n = indexs.size();
	for (i = 0; i < n; i++)
		if (indexs[i].index_name == name)
			return true;
	return false;
}

//计算一个字段类型的长度
int Catalog_manager::Calculate_clo(Column C)
{
	if (C.type == 1)
		return sizeof(int);
	if (C.type == 2)
		return sizeof(float);
	if (C.type == 0)
		return C.len * sizeof(char);
}

//计算指定表格中每条记录的长度
int Catalog_manager::Calculate_rec(string name)
{
	int i;
	int n = tables.size();
	int len = 0;
	for (i = 0; i < n; i++)
		if (tables[i].table_name == name)
		{
			len = Calculate_clos(tables[i].clolums);
			return len;
		}
	cout << "Error! Table " << name << " does not exsit!" << endl << endl;
	return 0;
}

//计算指定表格中所有记录的长度
int Catalog_manager::Calculate_recs(string name)
{
	int i;
	int n = tables.size();
	int len;
	for (i = 0; i < n; i++)
		if (tables[i].table_name == name)
		{
			len = Calculate_clos(tables[i].clolums);
			len *= tables[i].record_num;
			return len;
		}
}

//计算指定多字段的记录长度
int Catalog_manager::Calculate_clos(vector<Column>&cols)
{
	int len = 0;
	int i;
	int n = cols.size();
	for (i = 0; i < n; i++)
	{
		len += Calculate_clo(cols[i]);
	}
	return len;
}

//返回指定表格在容器中的位置
int Catalog_manager::Table_pos(string name)
{
	int i;
	int n = tables.size();
	for (i = 0; i < n; i++)
	{
		if (tables[i].table_name == name)
			return i;
	}
	if (i == n)
		return -1;
}

//获得指定表格的记录条数
int Catalog_manager::Get_rec_num(string name)
{
	int m = Table_pos(name);
	return tables[m].record_num;
}

//根据表名获得所有字段
vector<Catalog_manager::Column> Catalog_manager::Get_table_col(string table)
{
	int m = Table_pos(table);
	return tables[m].clolums;
}