#ifndef RECORD_MANAGER_H
#define RECORD_MANAGER_H

#define BLOCKSIZE 4096
#define MAXLENGTH 256
#define PRINTLENGTH 10 //Ĭ�ϴ�ӡ����

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class RecordManager {
public:
	//����в���Ԫ��, ����ɹ��򷵻�1,�����ڲ����жϴ��������������Ƿ���������
	int InsertRecord(string TableName, vector<string> &Record);
	//�ж��Ƿ����ظ�ֵ, û�з���-1
	int IsExist(string TableName, vector<int>& index, vector<string>& value);
	//��һ�����в����¼
	void InsertAtBlock(string TableName, int block, vector<string>& Record);
	//��������select���,�������м�¼���޼�¼ʱֻ�������
	int SelectRecord(string TableName);
	//��������select���
	int SelectRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);
	//�ж��Ƿ����һ��where����
	bool ConfirmToWhere(char* value, int type, string op, string condition);
	//�ж�һ����¼�Ƿ�����ж�����
	bool ConfirmToWhere(string TableName, char* record, vector<string> columns, vector<string> ops, vector<string> conditions);
	//ɾ���������м�¼������ɾ����¼������
	int DeleteRecord(string TableName);
	//��������delete���,����ɾ����¼������
	int DeletetRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);
	////������������ѯ����Ľ���
	//void Intersection(vector<int> &block1, vector<int> &offset1, vector<int> &block2, vector<int> &offset2);
	//���ݿ�ź�ƫ�����õ�����
	char* GetData(string TableName, int block, int offset);
	//���ݿ�ź�ƫ����ɾ����¼��������
	void SetDelete(string TableName, int block, int offset);
	//��ӡһ����¼
	void PrintRecord(string TableName, char* record);
	//�������м�¼������ĳһ�е�����value�������
	void SelectValue(string TableName, string Column, vector<char *> &Values, vector<int> &block, vector<int> &offset);

	//�ж�һ���ַ����Ƿ�������
	bool IsInt(string s);
	//�ж�һ���ַ����Ƿ��Ǹ�����
	bool IsFloat(string s);
};


#endif

