#ifndef __INDEX_MANAGER_H
#define __INDEX_MANAGER_H

#define M  5 //B+树的阶数

#pragma warning(disable : 4996)  

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

#include "Catalog_manager.h"

using namespace std;

class IndexManager {
public:	
	struct SearchKey {
		int type;
		int length;
		char* value;
		int block;
		int offset;
	};
	struct MiddleSearchKey {
		int type;
		int length;
		char* value;
		int leftBlock;
		int rightBlock;
	};
	//创建一个索引
	void CreateIndex(string tablename, string column, string filename);
	//删除一个索引
	void DeleteIndex(string filename);
	//删除一个索引内所有信息
	void DeleteIndex(string tablename, string column);
	//查找一个值等于value的记录, 没有则返回0
	int SearchOne(string tablename, string column, string value, int& block, int& offset);
	//查找一批>,  >=, <, <=记录的地址
	void SearchMany(string tablename, string column, string value, int type, string op, vector<int>& block, vector<int>& offset);
	//查找一个搜索码可能存在的叶子号
	int SearchLeaf(string tablename, string column, string value);
	//插入一条记录
	void InsertRecord(string tablename, string column, char * value, int block, int offset);
	//删除一条记录
	void DeleteRecord(string tablename, string column, char* value);

	//判断叶子或中间节点是否已满
	bool IsFull(string filename, int block);
	//判断块中的value值是否过少
	bool IsLess(string filename, int block);
	//判断是否有富余的value值
	bool IsAmple(string filename, int block);
	//在叶子中删除一个节点
	void DeleteOne(string filename, int leaf, char *value, int type, int len);
	//向左借一条记录
	void BorrowFromLeft(string filename, int parent, int left, int right, int len);
	//向右借一条记录
	void BorrowFromRight(string filename, int parent, int left, int right, int len);
	//合并两个叶子
	void MergeLeaves(string filename, int parent, int left, int right, Catalog_manager::Index & index);
	//合并两个中间节点
	void MergeNodes(string filename, int parent, int left, int right, Catalog_manager::Index & index);
	//查找左节点（没有则返回-1）
	int FindLeft(string filename, int parent, int child, int len);
	//查找右节点（没有则返回-1）
	int FindRight(string filename, int parent, int child, int len);
	//到叶子块满时调用函数
	void InsertDivide(string filename, int oldLeaf, int newLeaf, SearchKey& key, MiddleSearchKey& mKey);
	//当中间块满时调用函数
	void InsertDivide(string filename, int oldblock, int newblock, MiddleSearchKey& mKey);
	//找到可能存在记录的叶子块的块号, 并存储路径
	int FindLeaf(string filename, int root, string value, int type, int len, vector<int> &path);
	//找到可能存在记录的叶子块的块号, 并存储路径
	int FindLeaf(string filename, int root, char *value, int type, int len, vector<int> &path);
	//在叶子节点中插入一个节点,成功则返回1
	int InsertOne(string filename, int block, SearchKey &key);
	//在非叶子节点中插入一个节点,成功则返回1
	int InsertOne(string filename, int leaf, MiddleSearchKey &key);
	//找到一个可用的块
	int FindEmptyBlock(string tablename, string column);
	//创建一个新的叶子节点
	char* CreateLeaf(string filename, int block);
	//创建一个新的中间节点
	char* CreateNode(string filename, int block);
	//比较大小, 小于返回负值， 等于返回0， 大于返回正值
	int Compare(char* x, char* y, int type);
	int Compare(char* x, string y, int type);
	//输出b+树的结构（测试用）
	void printBTree(string tablename, string column);

};

#endif

