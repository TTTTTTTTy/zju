#include "API.h"
#include "Catalog_manager.h"
#include <iostream>
#include <vector>
#include <string>
#include "DB.hpp"

#define UNKNOWN_FILE 8                                 //�����궨�壬���ڶ����ļ�����
#define TABLE_FILE 9   
#define INDEX_FILE 10

using namespace std;

//��ӡһ���������еļ�¼�ʹ�ӡ�ļ�¼��
void API::printRecord(string tableName)
{
	//�����ֵ���Ϣ������ļ����棬ֱ�ӳ�����ʾ
	if (cm->Table_exist(tableName))
	{
		int m;
		m = rm->SelectRecord(tableName);
		cout << endl << m << " records selected." << endl << endl;
	}
	else
		cout << "There is no table " << tableName << endl << endl;
}

//����where������ӡ���еļ�¼�ʹ�ӡ�ļ�¼��
void API::printRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions)
{
	//�����ֵ���Ϣ������ļ����棬ֱ�ӳ�����ʾ
	if (cm->Table_exist(TableName))
	{
		int m;
		m = rm->SelectRecord(TableName, columns, op, conditions);
		cout << endl << endl << m << " records selected" << endl << endl;
	}
	else
		cout << "There is no table " << TableName << endl << endl;
}

//����в����¼,����Ĳ����Ǳ�����ƺ�һ������˳��洢�����ֶε����ݵ�����
void API::insertRecord(string tableName, vector<string> v)
{
	//�����ֵ���Ϣ������ļ����棬ֱ�ӳ�����ʾ
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}

	if (rm->InsertRecord(tableName, v))
		cout << "Insert 1 record." << endl << endl;

}

//�����еļ�¼ȫ��ɾ��,ͬʱ���ɾ���ļ�¼��Ŀ
void API::deleteValue(string tableName)
{
	//�����ֵ���Ϣ������ļ����棬ֱ�ӳ�����ʾ
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}
	//������RecordManager��ɾ����¼��Ȼ���������ֵ��н���¼����Ϊ0
	int num = rm->DeleteRecord(tableName);
	cout << "Delete " << num << " records " << "in " << tableName << endl << endl;
}

//����where����ɾ�����еļ�¼��ͬʱ���ɾ����¼����Ŀ
void API::deleteValue(string tableName, vector<string> &columns, vector<string> &op, vector<string> &conditions)
{
	//�����ֵ���Ϣ������ļ����棬ֱ�ӳ�����ʾ
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}
	int num = rm->DeletetRecord(tableName, columns, op, conditions);
	cout << "Delete " << num << " records " << "in " << tableName << endl << endl;
}

//ɾ��һ��������ɾ��Ŀ¼�б����ڴ������м�¼
void API::dropTable(string tableName)
{
	//�����ֵ���Ϣ������ļ����棬ֱ�ӳ�����ʾ
	if (!cm->Table_exist(tableName))
	{
		cout << "There is no table " << tableName << endl << endl;
		return;
	}
	//ֱ�����ֵ���Ϣ�н���ɾ��
	rm->DeleteRecord(tableName);
	cm->Delete_table(tableName);

}

//ɾ��ָ������
void API::dropIndex(string name)
{
	//�����ֵ���Ϣ������������棬ֱ�ӳ�����ʾ
	if (!cm->Index_exist(name))
	{
		cout << "There is no index " << name << endl << endl;
		return;
	}
	cm->Delete_index(name);

}

//���ݸ����ı����ԣ���������
void API::createIndex(string name, string tableName, string colName)
{
	//�����ֵ���Ϣ������������ڣ�ֱ�ӳ�����ʾ
	if (cm->Index_exist(name))
	{
		cout << "There is index of " << colName << " in table" << tableName << " already" << endl << endl;
		return;
	}

	//���ֵ���Ϣ����������
	cm->Create_index(name, colName, tableName);
}

//�����С���������Ϣ������
void API::createTable(string name, vector<Catalog_manager::Column>&cols)
{
	//�����ֵ���Ϣ��������Ѿ����ڣ�ֱ�ӳ�����ʾ
	if (cm->Table_exist(name))
	{
		cout << "There is table " << name << " already." << endl << endl;
		return;
	}
	cm->Create_table(name, cols);

}

//�ر����ݿ�
void API::closeDatabase()
{
	cm->Write_in();
}
