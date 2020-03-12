#ifndef __INDEX_MANAGER_H
#define __INDEX_MANAGER_H

#define M  5 //B+���Ľ���

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
	//����һ������
	void CreateIndex(string tablename, string column, string filename);
	//ɾ��һ������
	void DeleteIndex(string filename);
	//ɾ��һ��������������Ϣ
	void DeleteIndex(string tablename, string column);
	//����һ��ֵ����value�ļ�¼, û���򷵻�0
	int SearchOne(string tablename, string column, string value, int& block, int& offset);
	//����һ��>,  >=, <, <=��¼�ĵ�ַ
	void SearchMany(string tablename, string column, string value, int type, string op, vector<int>& block, vector<int>& offset);
	//����һ����������ܴ��ڵ�Ҷ�Ӻ�
	int SearchLeaf(string tablename, string column, string value);
	//����һ����¼
	void InsertRecord(string tablename, string column, char * value, int block, int offset);
	//ɾ��һ����¼
	void DeleteRecord(string tablename, string column, char* value);

	//�ж�Ҷ�ӻ��м�ڵ��Ƿ�����
	bool IsFull(string filename, int block);
	//�жϿ��е�valueֵ�Ƿ����
	bool IsLess(string filename, int block);
	//�ж��Ƿ��и����valueֵ
	bool IsAmple(string filename, int block);
	//��Ҷ����ɾ��һ���ڵ�
	void DeleteOne(string filename, int leaf, char *value, int type, int len);
	//�����һ����¼
	void BorrowFromLeft(string filename, int parent, int left, int right, int len);
	//���ҽ�һ����¼
	void BorrowFromRight(string filename, int parent, int left, int right, int len);
	//�ϲ�����Ҷ��
	void MergeLeaves(string filename, int parent, int left, int right, Catalog_manager::Index & index);
	//�ϲ������м�ڵ�
	void MergeNodes(string filename, int parent, int left, int right, Catalog_manager::Index & index);
	//������ڵ㣨û���򷵻�-1��
	int FindLeft(string filename, int parent, int child, int len);
	//�����ҽڵ㣨û���򷵻�-1��
	int FindRight(string filename, int parent, int child, int len);
	//��Ҷ�ӿ���ʱ���ú���
	void InsertDivide(string filename, int oldLeaf, int newLeaf, SearchKey& key, MiddleSearchKey& mKey);
	//���м����ʱ���ú���
	void InsertDivide(string filename, int oldblock, int newblock, MiddleSearchKey& mKey);
	//�ҵ����ܴ��ڼ�¼��Ҷ�ӿ�Ŀ��, ���洢·��
	int FindLeaf(string filename, int root, string value, int type, int len, vector<int> &path);
	//�ҵ����ܴ��ڼ�¼��Ҷ�ӿ�Ŀ��, ���洢·��
	int FindLeaf(string filename, int root, char *value, int type, int len, vector<int> &path);
	//��Ҷ�ӽڵ��в���һ���ڵ�,�ɹ��򷵻�1
	int InsertOne(string filename, int block, SearchKey &key);
	//�ڷ�Ҷ�ӽڵ��в���һ���ڵ�,�ɹ��򷵻�1
	int InsertOne(string filename, int leaf, MiddleSearchKey &key);
	//�ҵ�һ�����õĿ�
	int FindEmptyBlock(string tablename, string column);
	//����һ���µ�Ҷ�ӽڵ�
	char* CreateLeaf(string filename, int block);
	//����һ���µ��м�ڵ�
	char* CreateNode(string filename, int block);
	//�Ƚϴ�С, С�ڷ��ظ�ֵ�� ���ڷ���0�� ���ڷ�����ֵ
	int Compare(char* x, char* y, int type);
	int Compare(char* x, string y, int type);
	//���b+���Ľṹ�������ã�
	void printBTree(string tablename, string column);

};

#endif

