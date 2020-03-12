#ifndef RECORD_MANAGER_H
#define RECORD_MANAGER_H

#define BLOCKSIZE 4096
#define MAXLENGTH 256
#define PRINTLENGTH 10 //默认打印长度

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class RecordManager {
public:
	//向表中插入元组, 插入成功则返回1,函数内部会判断传进参数的数量是否满足条件
	int InsertRecord(string TableName, vector<string> &Record);
	//判断是否有重复值, 没有返回-1
	int IsExist(string TableName, vector<int>& index, vector<string>& value);
	//向一个块中插入记录
	void InsertAtBlock(string TableName, int block, vector<string>& Record);
	//无条件的select语句,返回所有记录，无记录时只输出属性
	int SelectRecord(string TableName);
	//有条件的select语句
	int SelectRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);
	//判断是否符合一个where条件
	bool ConfirmToWhere(char* value, int type, string op, string condition);
	//判断一条记录是否符合判断条件
	bool ConfirmToWhere(string TableName, char* record, vector<string> columns, vector<string> ops, vector<string> conditions);
	//删除表中所有记录，返回删除记录的数量
	int DeleteRecord(string TableName);
	//有条件的delete语句,返回删除记录的数量
	int DeletetRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);
	////求两个索引查询结果的交集
	//void Intersection(vector<int> &block1, vector<int> &offset1, vector<int> &block2, vector<int> &offset2);
	//根据块号和偏移量得到数据
	char* GetData(string TableName, int block, int offset);
	//根据块号和偏移量删除记录及其索引
	void SetDelete(string TableName, int block, int offset);
	//打印一条记录
	void PrintRecord(string TableName, char* record);
	//遍历所有记录，返回某一列的所有value及其入口
	void SelectValue(string TableName, string Column, vector<char *> &Values, vector<int> &block, vector<int> &offset);

	//判断一个字符串是否是整数
	bool IsInt(string s);
	//判断一个字符串是否是浮点数
	bool IsFloat(string s);
};


#endif

