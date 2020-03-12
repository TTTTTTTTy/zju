

#ifndef API_H
#define API_H
#include "record_manager.h"
#include "Catalog_manager.h"
#include <string>
#include <vector>
using namespace std;
class API {
public:
	API() {}
	~API() {}

	//删除一个表，包括删除目录中表格和内存中所有记录
	void dropTable(string tableName);
	//删除指定索引
	void dropIndex(string name);

	//根据给定的表、属性，创建索引
	void createIndex(string name, string tableName, string colName);
	//根据列、主键等信息创建表
	void createTable(string name, vector<Catalog_manager::Column>&cols);

	//打印一个表中所有的记录和打印的记录数
	void printRecord(string tableName);
	//根据where条件打印表中的记录和打印的记录数，可以有多组and条件
	void printRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);

	//向表中插入入记录,传入的参数是表格名称和一个按照顺序存储各个字段的数据的容器
	void insertRecord(string tableName, vector<string> v);

	//将表中的记录全部删除,同时输出删除的记录数目
	void deleteValue(string tableName);
	//根据where条件删除表中的记录，同时输出删除记录的数目
	void deleteValue(string tableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);

	//关闭数据库
	void closeDatabase();

};

#endif