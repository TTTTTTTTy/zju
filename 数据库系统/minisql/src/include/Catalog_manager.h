#ifndef CATALOG_MANAGER_H
#define CATALOG_MANAGER_H

#include<algorithm>
#include <iostream>
#include <fstream>
#include <io.h>
#include<string>
#include<vector>
using namespace std;

class Catalog_manager
{
public:

	struct Column
	{
		string column_name;		//属性名称
		int type;		//属性类型，仅三种：char 0, int 1, float 2  
		int len;		//若为char类型，则为char(n)中的n+1（放\0）；若为其他类型，则为其他类型的大小
		bool is_pri;		//是否主键
		bool is_unique;		//是否unique
	};

	struct Index
	{
		string index_name;		//索引名称(table-column)
		string index_table;		//索引所在的表格名称
		string index_col;		//索引所在的字段名称
		int freeNum;	//空链表
		int root;	//根节点
		int type;   //所建字段的类型：char 0, int 1, float 2 
		int len;	//若为char类型，则为char(n)中的n+1（放\0）；若为其他类型，则为其他类型的大小
	};

	struct Table
	{
		string table_name;	//表格名称
		vector<Column> clolums;		//表格的各个属性
		int colu_num;               //表格属性的数目
		int record_num;		//表格中记录的条数
		int record_len;		//表格中存放记录的长度
		string pri_key;		//表格的主键
		vector<string> index;	//对应字段上的索引信息
	};

	vector<Table> tables;	//设定一个存放表格的容器
	vector<Index> indexs;	//设定一个存放索引的容器
	int table_num;
	int index_num;

	Catalog_manager();
	~Catalog_manager() {}

	//对给定表格的字段创建索引
	int Create_index(string name, string column, string table);
	//删除指定索引
	int Delete_index(string name);
	//删除指定表格上的索引
	int Delete_index_ontable(string table);
	//返回指定表格字段上的索引
	Index& Get_index(string table, string column);

	//使用给定的名称和字段创建表格
	int Create_table(string name, vector<Column>&cols);
	//删除给定名称的表格
	int Delete_table(string name);

	//插入记录
	int Insert_rec(string table, int num);

	//获得记录条数
	int Get_rec_num(string name);
	//根据表名获得所有字段
	vector<Column> Get_table_col(string table);

	//删除表格中所有记录
	int Delete_all(string name);
	//删除表格中指定条的记录
	int Delect_recs(string name, int num);


	//查找文件判断该表格是否存在
	bool Table_exist(string name);
	//查找文件判断该索引是否存在
	bool Index_exist(string name);
	//判断指定表格字段上是否存在索引
	bool has_index(string table, string colName);


	//计算一个字段类型的长度
	int Calculate_clo(Column C);
	//计算指定多字段的记录长度
	int Calculate_clos(vector<Column>&cols);
	//计算指定表格中每条记录的长度
	int Calculate_rec(string name);
	//计算指定表格中所有记录的长度
	int Calculate_recs(string name);

	//返回指定表格在容器中的位置
	int Table_pos(string name);

	//把信息写入文件
	int Write_in();

};

#endif