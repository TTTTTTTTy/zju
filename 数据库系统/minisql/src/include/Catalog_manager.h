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
		string column_name;		//��������
		int type;		//�������ͣ������֣�char 0, int 1, float 2  
		int len;		//��Ϊchar���ͣ���Ϊchar(n)�е�n+1����\0������Ϊ�������ͣ���Ϊ�������͵Ĵ�С
		bool is_pri;		//�Ƿ�����
		bool is_unique;		//�Ƿ�unique
	};

	struct Index
	{
		string index_name;		//��������(table-column)
		string index_table;		//�������ڵı������
		string index_col;		//�������ڵ��ֶ�����
		int freeNum;	//������
		int root;	//���ڵ�
		int type;   //�����ֶε����ͣ�char 0, int 1, float 2 
		int len;	//��Ϊchar���ͣ���Ϊchar(n)�е�n+1����\0������Ϊ�������ͣ���Ϊ�������͵Ĵ�С
	};

	struct Table
	{
		string table_name;	//�������
		vector<Column> clolums;		//���ĸ�������
		int colu_num;               //������Ե���Ŀ
		int record_num;		//����м�¼������
		int record_len;		//����д�ż�¼�ĳ���
		string pri_key;		//��������
		vector<string> index;	//��Ӧ�ֶ��ϵ�������Ϣ
	};

	vector<Table> tables;	//�趨һ����ű�������
	vector<Index> indexs;	//�趨һ���������������
	int table_num;
	int index_num;

	Catalog_manager();
	~Catalog_manager() {}

	//�Ը��������ֶδ�������
	int Create_index(string name, string column, string table);
	//ɾ��ָ������
	int Delete_index(string name);
	//ɾ��ָ������ϵ�����
	int Delete_index_ontable(string table);
	//����ָ������ֶ��ϵ�����
	Index& Get_index(string table, string column);

	//ʹ�ø��������ƺ��ֶδ������
	int Create_table(string name, vector<Column>&cols);
	//ɾ���������Ƶı��
	int Delete_table(string name);

	//�����¼
	int Insert_rec(string table, int num);

	//��ü�¼����
	int Get_rec_num(string name);
	//���ݱ�����������ֶ�
	vector<Column> Get_table_col(string table);

	//ɾ����������м�¼
	int Delete_all(string name);
	//ɾ�������ָ�����ļ�¼
	int Delect_recs(string name, int num);


	//�����ļ��жϸñ���Ƿ����
	bool Table_exist(string name);
	//�����ļ��жϸ������Ƿ����
	bool Index_exist(string name);
	//�ж�ָ������ֶ����Ƿ��������
	bool has_index(string table, string colName);


	//����һ���ֶ����͵ĳ���
	int Calculate_clo(Column C);
	//����ָ�����ֶεļ�¼����
	int Calculate_clos(vector<Column>&cols);
	//����ָ�������ÿ����¼�ĳ���
	int Calculate_rec(string name);
	//����ָ����������м�¼�ĳ���
	int Calculate_recs(string name);

	//����ָ������������е�λ��
	int Table_pos(string name);

	//����Ϣд���ļ�
	int Write_in();

};

#endif