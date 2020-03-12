

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

	//ɾ��һ��������ɾ��Ŀ¼�б����ڴ������м�¼
	void dropTable(string tableName);
	//ɾ��ָ������
	void dropIndex(string name);

	//���ݸ����ı����ԣ���������
	void createIndex(string name, string tableName, string colName);
	//�����С���������Ϣ������
	void createTable(string name, vector<Catalog_manager::Column>&cols);

	//��ӡһ���������еļ�¼�ʹ�ӡ�ļ�¼��
	void printRecord(string tableName);
	//����where������ӡ���еļ�¼�ʹ�ӡ�ļ�¼���������ж���and����
	void printRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);

	//����в������¼,����Ĳ����Ǳ�����ƺ�һ������˳��洢�����ֶε����ݵ�����
	void insertRecord(string tableName, vector<string> v);

	//�����еļ�¼ȫ��ɾ��,ͬʱ���ɾ���ļ�¼��Ŀ
	void deleteValue(string tableName);
	//����where����ɾ�����еļ�¼��ͬʱ���ɾ����¼����Ŀ
	void deleteValue(string tableName, vector<string> &columns, vector<string> &op, vector<string> &conditions);

	//�ر����ݿ�
	void closeDatabase();

};

#endif