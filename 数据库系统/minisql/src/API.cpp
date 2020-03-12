#include "API.h"
#include "Catalog_manager.h"
#include <iostream>
#include <vector>
#include <string>
#include "DB.hpp"

#define UNKNOWN_FILE 8                                 //三个宏定义，用于定义文件类型
#define TABLE_FILE 9   
#define INDEX_FILE 10

using namespace std;

//打印一个表中所有的记录和打印的记录数
void API::printRecord(string tableName)
{
	//查找字典信息，如果文件不存，直接出错提示
	if (cm->Table_exist(tableName))
	{
		int m;
		m = rm->SelectRecord(tableName);
		cout << endl << m << " records selected." << endl << endl;
	}
	else
		cout << "There is no table " << tableName << endl << endl;
}

//根据where条件打印表中的记录和打印的记录数
void API::printRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions)
{
	//查找字典信息，如果文件不存，直接出错提示
	if (cm->Table_exist(TableName))
	{
		int m;
		m = rm->SelectRecord(TableName, columns, op, conditions);
		cout << endl << endl << m << " records selected" << endl << endl;
	}
	else
		cout << "There is no table " << TableName << endl << endl;
}

//向表中插入记录,传入的参数是表格名称和一个按照顺序存储各个字段的数据的容器
void API::insertRecord(string tableName, vector<string> v)
{
	//查找字典信息，如果文件不存，直接出错提示
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}

	if (rm->InsertRecord(tableName, v))
		cout << "Insert 1 record." << endl << endl;

}

//将表中的记录全部删除,同时输出删除的记录数目
void API::deleteValue(string tableName)
{
	//查找字典信息，如果文件不存，直接出错提示
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}
	//首先在RecordManager中删除记录，然后在数据字典中将记录数改为0
	int num = rm->DeleteRecord(tableName);
	cout << "Delete " << num << " records " << "in " << tableName << endl << endl;
}

//根据where条件删除表中的记录，同时输出删除记录的数目
void API::deleteValue(string tableName, vector<string> &columns, vector<string> &op, vector<string> &conditions)
{
	//查找字典信息，如果文件不存，直接出错提示
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}
	int num = rm->DeletetRecord(tableName, columns, op, conditions);
	cout << "Delete " << num << " records " << "in " << tableName << endl << endl;
}

//删除一个表，包括删除目录中表格和内存中所有记录
void API::dropTable(string tableName)
{
	//查找字典信息，如果文件不存，直接出错提示
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}
	//直接在字典信息中将表删除
	rm->DeleteRecord(tableName);
	cm->Delete_table(tableName);

}

//删除指定索引
void API::dropIndex(string name)
{
	//查找字典信息，如果索引不存，直接出错提示
	if (!cm->Index_exist(name))
	{
		cout << "There is no index " << name << endl << endl;
		return;
	}
	cm->Delete_index(name);

}

//根据给定的表、属性，创建索引
void API::createIndex(string name, string tableName, string colName)
{
	//查找字典信息，如果索引存在，直接出错提示
	if (cm->Index_exist(name))
	{
		cout << "There is index of " << colName << " in table" << tableName << " already" << endl << endl;
		return;
	}

	//在字典信息中增加索引
	cm->Create_index(name, colName, tableName);
}

//根据列、主键等信息创建表
void API::createTable(string name, vector<Catalog_manager::Column>&cols)
{
	//查找字典信息，如果表已经存在，直接出错提示
	if (cm->Table_exist(name))
	{
		cout << "There is table " << name << " already." << endl << endl;
		return;
	}
	cm->Create_table(name, cols);

}

//关闭数据库
void API::closeDatabase()
{
	cm->Write_in();
}
