#include "IndexManager.h"
#include "DB.hpp"
/*��������*/
void IndexManager::CreateIndex(string tablename, string column, string filename) {;
	bm->NewFile(filename); //���������ļ�
	vector<char*> values;
	vector<int> block, offset;
	rm->SelectValue(tablename, column, values, block, offset);
	for (int i = 0; i < values.size(); i++) {
		InsertRecord(tablename, column, values.at(i), block.at(i), offset.at(i));
	}
	bm->write_all_back();
}

void IndexManager::DeleteIndex(string filename) {
	bm->DelFile(filename);
}

void IndexManager::DeleteIndex(string tablename, string column) {
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	bm->DelFile(index.index_name);
	bm->NewFile(index.index_name);
	index.freeNum = -1;  //����������¼��Ϣ
	index.root = -1;
}

void IndexManager::InsertRecord(string tablename, string column, char* value, int block, int offset) {
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	string filename = index.index_name;
	SearchKey key;
	MiddleSearchKey mKey;
	key.block = block;
	key.offset = offset;
	key.value = new char[MAXLENGTH];
	memcpy(key.value, value, index.len);
	mKey.type = key.type = index.type;
	mKey.length = key.length = index.len;
	char* p;
	if (index.root == -1) { //�������޽ڵ�	
		p = CreateLeaf(filename, 0);
		if (!InsertOne(filename, 0, key)) { //����Ҷ�ڵ�
			cout << "Fail to update index!" << endl;
			return;
		}
		index.root = 0; //���¸��ڵ�
	}
	else {
		vector<int> path;
		int leaf = FindLeaf(filename, index.root, value, index.type, index.len, path);
		if (!IsFull(filename, leaf))
			InsertOne(filename, leaf, key);
		else { //Ҷ������
			int newLeaf = FindEmptyBlock(tablename, column); //��Ҷ�ӵĿ��
			InsertDivide(filename, leaf, newLeaf, key, mKey);
			int pos = path.size() - 2; //·���±�
			int newBlock; //���м�ڵ�Ŀ��
			while (pos >= 0 && IsFull(filename, path.at(pos))) { //���ϱ���ֱ���ҵ������Ľڵ���޸��ڵ�
																 //�ҵ�һ���տ�
				newBlock = FindEmptyBlock(tablename, column);
				CreateNode(filename, newBlock);
				InsertDivide(filename, path.at(pos), newBlock, mKey);
				pos--;
			}
			if (pos < 0) {
				newBlock = FindEmptyBlock(tablename, column);
				CreateNode(filename, newBlock);
				InsertOne(filename, newBlock, mKey);
				index.root = newBlock;
			}
			else {
				InsertOne(filename, path.at(pos), mKey);
			}
		}

	}
	bm->write_all_back();
//	printBTree(tablename, column);
}

int IndexManager::FindLeaf(string filename, int root, string value, int type, int len, vector<int> &path) {
	char* p = bm->GetBlock(filename, root);
	path.push_back(root);
	int nextNode;
	while (p[0] != '!')
	{
		int count;
		char* key = new char[MAXLENGTH];
		p++;
		memcpy(&count, p, sizeof(int));
		p += sizeof(int);
		int i;
		for (i = 0; i < count; i++) {
			memcpy(&nextNode, p, sizeof(int));
			p += sizeof(int);
			memcpy(key, p, len);
			if (Compare(key, value ,type) > 0) {  //���ҵ�ֵС��������ֵ
				path.push_back(nextNode);
				break;
			}
			p += len;
		}
		if (i == count) { //Ҫ���ҵ�ֵ���ڵ������е�����ֵ���������ұߵ��ӽڵ�
			memcpy(&nextNode, p, sizeof(int));
			path.push_back(nextNode);
		}	
		p = bm->GetBlock(filename, nextNode);
	}
	return nextNode;
}

int IndexManager::FindLeaf(string filename, int root, char* value, int type, int len, vector<int> &path) {
	char* p = bm->GetBlock(filename, root);
	path.push_back(root);
	int nextNode = root;
	while (p[0] != '!')
	{
		int count;
		char* key = new char[MAXLENGTH];
		p++;
		memcpy(&count, p, sizeof(int));
		p += sizeof(int);
		int i;
		for (i = 0; i < count; i++) {
			memcpy(&nextNode, p, sizeof(int));
			p += sizeof(int);
			memcpy(key, p, len);
			if (Compare(key, value, type) > 0) {  //���ҵ�ֵС��������ֵ
				path.push_back(nextNode);
				break;
			}
			p += len;
		}
		if (i == count) { //Ҫ���ҵ�ֵ���ڵ������е�����ֵ���������ұߵ��ӽڵ�
			memcpy(&nextNode, p, sizeof(int));
			path.push_back(nextNode);
		}
		p = bm->GetBlock(filename, nextNode);
	}
	return nextNode;
}

int IndexManager::Compare(char* x, char* y, int type)  {
	if (type == 0) { //char
		return strcmp(x, y);
	}
	else if (type == 1) { //int
		int xi, yi;
		memcpy(&xi, x, sizeof(int));
		memcpy(&yi, y, sizeof(int));
		if (xi > yi)
			return 1;
		else if (xi == yi)
			return 0;
		else
			return -1;
	}
	else { //float
		float xf, yf;
		memcpy(&xf, x, sizeof(int));
		memcpy(&yf, y, sizeof(int));
		if (xf > yf)
			return 1;
		else if (xf == yf)
			return 0;
		else
			return -1;
	}
}

int IndexManager::Compare(char* x, string y, int type) {
	stringstream s(y);
	if (type == 0) { //char
		return strcmp(x, y.data());
	}
	else if (type == 1) { //int
		int xi, yi;
		memcpy(&xi, x, sizeof(int));
		s >> yi;
		if (xi > yi)
			return 1;
		else if (xi == yi)
			return 0;
		else
			return -1;
	}
	else { //float
		float xf, yf;
		memcpy(&xf, x, sizeof(int));
		s >> yf;
		if (xf > yf)
			return 1;
		else if (xf == yf)
			return 0;
		else
			return -1;
	}
}

void IndexManager::InsertDivide(string filename, int oldLeaf, int newLeaf, SearchKey& key, MiddleSearchKey& mKey) {
	InsertOne(filename, oldLeaf, key);  //�Ƚ��ڵ��������Ľڵ�
	char *p = CreateLeaf(filename, newLeaf);
	char *q = bm->GetBlock(filename, oldLeaf);
	int leftNum = ceil((M - 1) / 2); //�洢����߽ڵ������������
	int rightNum = M - leftNum; //�洢���ұ߽ڵ������������	
	q++;
	memcpy(q, &leftNum, sizeof(int));  //������ڵ�count
	//��leftNum ��ֵ�Ժ��������ʱ�洢
	int length = (M - leftNum) * (2 * sizeof(int) + key.length) + 4; //�洢����
	q += leftNum * (2 * sizeof(int) + key.length) + sizeof(int);  //����count������
	char* temp = new char[length];
	memcpy(temp, q, length);
	//ĩβָ��ָ��newLeaf
	memcpy(q, &newLeaf, sizeof(int));
	//�����ҽڵ�ͷ����Ϣ
	p++;
	memcpy(p, &rightNum, sizeof(int));
	p += sizeof(int); //����count
	memcpy(p, temp, length); //д������

	//���¸��ڵ���������ϴ��ݣ�
	temp += 2 * sizeof(int);
	mKey.value = new char[mKey.length];
	memcpy(mKey.value, temp, mKey.length);  //���ڵ���keyΪ�ҽڵ��е���Сֵ
	mKey.leftBlock = oldLeaf;
	mKey.rightBlock = newLeaf;
	bm->set_dirty(filename, oldLeaf); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, newLeaf); //��Ǵ˿�Ϊdirty  
}


void IndexManager::InsertDivide(string filename, int oldblock, int newblock, MiddleSearchKey& mKey) {
	InsertOne(filename, oldblock, mKey);  //���µ�key�Ȳ�����ڵ�
	char *p = CreateNode(filename, newblock);
	char *q = bm->GetBlock(filename, oldblock);
	int leftNum = ceil((M - 1) / 2); //�洢����߽ڵ������������
	int rightNum = M - 1 - leftNum; //�洢���ұ߽ڵ������������
	q++;
	memcpy(q, &leftNum, sizeof(int));  //������ڵ�count
	q += leftNum * (sizeof(int) + mKey.length) + 2 * sizeof(int);  //����count������
	memcpy(mKey.value, q, mKey.length); //���¸��ڵ�
	mKey.leftBlock = oldblock;
	mKey.rightBlock = newblock;
	q += mKey.length;
	int length = (M - 1 - leftNum) * (sizeof(int) + mKey.length) + sizeof(int); //���Ƶ��ҽڵ�����ݳ���
	//�����ҽڵ�ͷ����Ϣ
	p++;
	memcpy(p, &rightNum, sizeof(int));
	p += sizeof(int); //����count
	memcpy(p, q, length); //д������

	bm->set_dirty(filename, oldblock); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, newblock); //��Ǵ˿�Ϊdirty  
}

int IndexManager::InsertOne(string filename, int leaf, SearchKey &key) {
	char *p = bm->GetBlock(filename, leaf), *q;
	if (p[0] != '!') {
		cout << "Illegal insertion!" << endl;
		return 0;
	}
	p++;
	int count;
	memcpy(&count, p, sizeof(int));
	char* value = new char[MAXLENGTH];
	q = p + sizeof(int);
	//Ѱ�Ҳ���λ��
	int i;
	for (i = 0; i < count; i++) {
		q += 2 * sizeof(int); //������ַ
		memcpy(value, q, key.length); //��ȡkey
		if ((Compare(value, key.value, key.type) > 0))   //�ҵ�λ�ã����뵽��ֵǰ��
			break;
		q += key.length; 
	}
    //�ƶ�ָ�뵽����λ��
	if (i == count);  //δ�ҵ������뵽���
	else {
		q -= 2 * sizeof(int);
	}
	//������λ�õ�����������ʱ�洢
	int length = (count - i) * (2 * sizeof(int) + key.length) + 4; //�洢����
	char* temp = new char[length];
	memcpy(temp, q, length);
	//�����ַ
	memcpy(q, &key.block, sizeof(int));
	q += sizeof(int);
	memcpy(q, &key.offset, sizeof(int));
	q += sizeof(int);
	//��������
	memcpy(q, key.value, key.length);
	q += key.length;
	//����ʱ�洢����������д��
	memcpy(q, temp, length);
	//����count
	count++;
	memcpy(p, &count, sizeof(int));
	bm->set_dirty(filename, leaf); //��Ǵ˿�Ϊdirty  
	p = bm->GetBlock(filename, leaf);
	/*int a, b ,c ,d;
	memcpy(&a, p + 13, 4);
	memcpy(&a, p + 21, 4);
	memcpy(&a, p + 29, 4);
	memcpy(&a, p + 37, 4);*/
	return 1;
}

int IndexManager::InsertOne(string filename, int block, MiddleSearchKey &key) {
	char *p = bm->GetBlock(filename, block);
	if (p[0] != '?') {
		cout << "Illegal insertion!" << endl;
		return 0;
	}
	p++;
	int count, newCount;
	memcpy(&count, p, sizeof(int));
	char* value = new char[MAXLENGTH];
	//����count
	newCount = count + 1;
	memcpy(p, &newCount, sizeof(int));
	p = p + sizeof(int); //����count
	//�սڵ�
	if (count == 0) {
		memcpy(p, &key.leftBlock, sizeof(int));
		p += sizeof(int);
		memcpy(p, key.value, key.length);
		p += key.length;
		memcpy(p, &key.rightBlock, sizeof(int));
	}
	else {
		//Ѱ�Ҳ���λ��
		int i;
		for (i = 0; i < count; i++) {
			p += sizeof(int); //������ַ
			memcpy(value, p, key.length); //��ȡkey
			if ((Compare(value, key.value, key.type) > 0))   //�ҵ�λ�ã����뵽��ֵǰ��
				break;
			p += key.length;
		}
		int length;
		char* temp;
		//�ƶ�ָ�뵽����λ��
		if (i == count) { //δ�ҵ���ֱ�Ӳ��뵽���
			p += sizeof(int); //������ַ
		}
		else {
		
		}
		//������λ�õ�����������ʱ�洢
		length = (count - i) * (sizeof(int) + key.length); //�洢����
		temp = new char[length];
		memcpy(temp, p, length);
		//��������
		memcpy(p, key.value, key.length);
		p += key.length;
		//�����ַ
		memcpy(p, &key.rightBlock, sizeof(int));
		p += sizeof(int);
		if (i < count) {
			memcpy(p, temp, length); //����ʱ�洢����������д��
		}
	}
	bm->set_dirty(filename, block); //��Ǵ˿�Ϊdirty  
	return 1;
}

bool IndexManager::IsFull(string filename, int block) {
	char* p = bm->GetBlock(filename, block);
	p++;
	int count;
	memcpy(&count, p, sizeof(int));
	if (count == M - 1)
		return true;
	else
		return false;
}

char* IndexManager::CreateLeaf(string filename, int block){
	char *p = bm->GetBlock(filename, block, true); 
	p[0] = '!';
	int count = 0;
	memcpy(p + 1, &count, sizeof(int)); //��ʼ��countΪ0
	memcpy(p + 5, "end", 4);
	bm->set_dirty(filename, block); //��Ǵ˿�Ϊdirty  
	bm->write_all_back();
	return p;
}

char* IndexManager::CreateNode(string filename, int block) {
	char *p = bm->GetBlock(filename, block, true);
	p[0] = '?';
	int count = 0;
	memcpy(p + 1, &count, sizeof(int)); //��ʼ��countΪ0
	bm->set_dirty(filename, block); //��Ǵ˿�Ϊdirty  
	bm->write_all_back();
	return p;
}

/*��ѯ����*/

int IndexManager::SearchOne(string tablename, string column, string value, int& block, int& offset) {
	Catalog_manager::Index &index = cm->Get_index(tablename, column);	
	string filename = index.index_name;
	if (index.root == -1) {
		cout << "No index exists!" << endl;
		return 0;
	}
	int leaf = SearchLeaf(tablename, column, value);
	char *p = bm->GetBlock(filename, leaf);
	p++;
	int count;
	memcpy(&count, p, sizeof(int));
	char* key = new char[MAXLENGTH];
	p += sizeof(int);
	for (int i = 0; i < count; i++) {
		memcpy(&block, p, sizeof(int)); //��ȡ��ַ
		p += sizeof(int);
		memcpy(&offset, p, sizeof(int)); 
		p += sizeof(int);
		memcpy(key, p, index.len); //��ȡkey
		if ((Compare(key, value, index.type) == 0))   //�ҵ�ֵ
			return 1;
		p += index.len;
	}
	return 0;
}

void IndexManager::SearchMany(string tablename, string column, string value, int type, string op, vector<int>& block, vector<int>& offset) {
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	string filename = index.index_name;
	if (index.root == -1) {
		cout << "No index exists!" << endl;
		return;
	}
	int count, tmpblock, tmpoffset, find = 0, nextblock;
	int leaf = SearchLeaf(tablename, column, value);
	if (op == ">=" || op == ">") {
		char *p = bm->GetBlock(filename, leaf);
		p++;	
		memcpy(&count, p, sizeof(int));
		char* key = new char[MAXLENGTH];
		p += sizeof(int);
		for (int i = 0; i < count; i++) {
			memcpy(&tmpblock, p, sizeof(int)); //��ȡ��ַ
			p += sizeof(int);
			memcpy(&tmpoffset, p, sizeof(int));
			p += sizeof(int);
			if (!find) {
				memcpy(key, p, index.len); //��ȡkey
				int k;
				memcpy(&k, key, 4);
				if ((Compare(key, value, index.type) >= 0)) {   //�ҵ�ֵ
				
					find = 1;
					if (!(op == ">" && Compare(key, value, index.type) == 0)) {
						block.push_back(tmpblock);
						offset.push_back(tmpoffset);
					}
				}
			}
			else {
				block.push_back(tmpblock);
				offset.push_back(tmpoffset);
			}
			p += index.len;
		}
		memcpy(&nextblock, p, sizeof(int));
		char end[5];
		memcpy(end, p, sizeof(int));
		while (strcmp(end, "end"))
		{
			p = bm->GetBlock(filename, nextblock);
			p++;
			memcpy(&count, p, sizeof(int));
			p += sizeof(int);
			for (int i = 0; i < count; i++) {
				memcpy(&tmpblock, p, sizeof(int)); //��ȡ��ַ
				p += sizeof(int);
				memcpy(&tmpoffset, p, sizeof(int));
				p += sizeof(int);
				block.push_back(tmpblock);
				offset.push_back(tmpoffset);
				p += index.len;
			}
			memcpy(&nextblock, p, sizeof(int));
			memcpy(end, p, 4);
		}
	}
	else if (op == "<=" || op == "<") {
		int node = index.root;
		char *p = bm->GetBlock(filename, node);
		//�ҵ�����ߵ�Ҷ��
		while (p[0] != '!') {
			p += sizeof(int) + 1;
			memcpy(&node, p, sizeof(int));
			p = bm->GetBlock(filename, node);
		}
		//���ֱ������leaf����value���ڣ����ڵĽڵ㣩
		while (node != leaf) {
			p = bm->GetBlock(filename, node);
			p++;
			memcpy(&count, p, sizeof(int));
			p += sizeof(int);
			for (int i = 0; i < count; i++) {
				memcpy(&tmpblock, p, sizeof(int)); //��ȡ��ַ
				p += sizeof(int);
				memcpy(&tmpoffset, p, sizeof(int));
				p += sizeof(int);
				block.push_back(tmpblock);
				offset.push_back(tmpoffset);
				p += index.len;
			}
			memcpy(&node, p, sizeof(int));
		}
		//��leaf����ڵ���Ѱ��ʣ�µķ���������ֵ
		p = bm->GetBlock(filename, leaf);
		p++;
		memcpy(&count, p, sizeof(int));
		char* key = new char[MAXLENGTH];
		p += sizeof(int);
		for (int i = 0; i < count; i++) {
			memcpy(&tmpblock, p, sizeof(int)); //��ȡ��ַ
			p += sizeof(int);
			memcpy(&tmpoffset, p, sizeof(int));
			p += sizeof(int);
			memcpy(key, p, index.len); //��ȡkey
			if ((Compare(key, value, index.type) >= 0)) {   //�ҵ�ֵ
				if (op == "<=" && Compare(key, value, index.type) == 0) {
					block.push_back(tmpblock);
					offset.push_back(tmpoffset);
				}
				return;
			}
			block.push_back(tmpblock);
			offset.push_back(tmpoffset);
			p += index.len;
		}

	}
	else {
		cout << "Invalid operation!" << endl;
	}
}

int IndexManager::SearchLeaf(string tablename, string column, string value) {
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	string filename = index.index_name;
	char *p = bm->GetBlock(filename, index.root);
	int nextNode = index.root;
	while (p[0] != '!')
	{
		int count, i;
		p++;
		memcpy(&count, p, sizeof(int));
		p += sizeof(int);
		for (i = 0; i < count; i++) {
			memcpy(&nextNode, p, sizeof(int));
			p += sizeof(int);
			char* key = new char[index.len];
			memcpy(key, p, index.len);
			p += index.len;
			if (Compare(key, value, index.type) > 0) { //Ҫ���ҵ�ֵС������ֵ��������ߵ��ӽڵ�
				p = bm->GetBlock(filename, nextNode);
				break;
			}
		}
		if (i == count) { //Ҫ���ҵ�ֵ���ڵ������е�����ֵ���������ұߵ��ӽڵ�
			memcpy(&nextNode, p, sizeof(int));
			p = bm->GetBlock(filename, nextNode);
		}
	}
	return nextNode;
}

/*ɾ������*/

void IndexManager::DeleteRecord(string tablename, string column, char* value) {
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	string filename = index.index_name;
	vector<int> path;
	int leaf = FindLeaf(filename, index.root, value, index.type, index.len, path);
	char *p = bm->GetBlock(filename, leaf);
	int pos = path.size() - 1; //·����Ҷ�ڵ���±�
	DeleteOne(filename, leaf, value, index.type, index.len);	
	if (leaf == index.root) //��ֻ�и��ڵ㣬�����ж�
		return;
	if (IsLess(filename, leaf)) 
	{  //ɾ����¼��Ҷ���ڼ�ֵ�Թ���
		int node = path.at(pos);
		int parent = path.at(pos - 1);
		int leftNode = FindLeft(filename, parent, node, index.len);
		int rightNode = FindRight(filename, parent, node, index.len);
		if (leftNode != -1 && IsAmple(filename, leftNode))  //�и������ڵ�
			BorrowFromLeft(filename, path.at(pos - 1), leftNode, node, index.len);
		else if (rightNode != -1 && IsAmple(filename, rightNode))  //�и�����ҽڵ�
			BorrowFromRight(filename, path.at(pos - 1), node, rightNode, index.len);
		else 
		{
			if (leftNode != -1)  //����ϲ�
				MergeLeaves(filename, parent, leftNode, node, index);
			else 
				MergeLeaves(filename, parent, node, rightNode, index);
			while (parent != index.root && IsLess(filename, parent)) 
			{
				pos--;
				node = parent;
				parent = path.at(pos - 1);
				int leftNode = FindLeft(filename, parent, node, index.len);
				int rightNode = FindRight(filename, parent, node, index.len);
				if (leftNode != -1 && IsAmple(filename, leftNode))  //�и������ڵ�
					BorrowFromLeft(filename, path.at(pos - 1), leftNode, node, index.len);
				else if (rightNode != -1 && IsAmple(filename, rightNode))  //�и�����ҽڵ�
					BorrowFromRight(filename, path.at(pos - 1), node, rightNode, index.len);
				else if (leftNode != -1) 
					MergeNodes(filename, parent, leftNode, node, index);
				else 
					MergeNodes(filename, parent, node, rightNode, index);
			}
			if (IsLess(filename, parent))  //���¸��ڵ�
			{
				int free = index.freeNum;
				char *p = bm->GetBlock(filename, index.root);
				memcpy(p, &free, sizeof(int));
				index.freeNum = index.root;
				index.root = node;
			}
		}

	}
//	printBTree(tablename, column);
}

bool IndexManager::IsLess(string filename, int block) {
	char *p = bm->GetBlock(filename, block);
	int limit = (p[0] == '!') ? ceil((M - 1) / 2) : (ceil(M / 2) - 1); //Ҫ�����С��������
	p++;
	int count;
	memcpy(&count, p, sizeof(int));
	if (count < limit)
		return true;
	else 
		return false;
}

bool IndexManager::IsAmple(string filename, int block) {
	char *p = bm->GetBlock(filename, block);
	int limit = p[0] == '!' ? ceil((M - 1) / 2) : ceil(M / 2) - 1; //Ҫ�����С��������
	p++;
	int count;
	memcpy(&count, p, sizeof(int));
	if (count > limit)
		return true;
	else
		return false;
}

void IndexManager::DeleteOne(string filename, int leaf, char *value, int type, int len) {
	char *p = bm->GetBlock(filename, leaf), *q;
	p++;
	int count;
	memcpy(&count, p, sizeof(int));
	char* key = new char[len];
	q = p;
	p += sizeof(int);
	for (int i = 0; i < count; i++) {
		p += 2 * sizeof(int);
		memcpy(key, p, len); //��ȡkey
		if ((Compare(key, value, type) == 0)) {  //�ҵ�ֵ
			p += len;
			int length = (count - i - 1) * (len + 2 * sizeof(int)) + 4; 
			char * tmp = new char[length];
			memcpy(tmp, p, length);
			p -= len + 2 * sizeof(int);
			memcpy(p, tmp, length);
			count--;
			memcpy(q, &count, sizeof(int)); //����count
			bm->set_dirty(filename, leaf); //��Ǵ˿�Ϊdirty  
			return;
		}
		p += len;
	}


}

int IndexManager::FindLeft(string filename, int parent, int child, int len) {
	char *p = bm->GetBlock(filename, parent);
	p++;
	int count, leftblock, rightblock;
	memcpy(&count, p, sizeof(int));
	p += sizeof(int);
	memcpy(&leftblock, p, sizeof(int));
	for (int i = 0; i < count; i++) {
		p += sizeof(int) + len;
		memcpy(&rightblock, p, sizeof(int));
		if (rightblock == child) {
			return leftblock;
		}
		leftblock = rightblock;
	}
	return -1;
}

int IndexManager::FindRight(string filename, int parent, int child, int len) {
	char *p = bm->GetBlock(filename, parent);
	p++;
	int count, leftblock, rightblock;
	memcpy(&count, p, sizeof(int));
	p += sizeof(int);
	memcpy(&leftblock, p, sizeof(int));
	for (int i = 0; i < count; i++) {
		p += sizeof(int) + len;
		memcpy(&rightblock, p, sizeof(int));
		if (leftblock == child) {
			return rightblock;
		}
		leftblock = rightblock;
	}
	return -1;
}

void IndexManager::BorrowFromLeft(string filename, int parent, int left, int right, int len) {
	char *p = bm->GetBlock(filename, left);
	char *q = bm->GetBlock(filename, right);
	char *pa = bm->GetBlock(filename, parent);
	if (p[0] == '?') {
		int pcount, qcount, count;
		p++;
		memcpy(&pcount, p, sizeof(int));
		//����ڵ�õ�һ��key�������ӽڵ㲢����count
		count = pcount - 1;
		memcpy(p, &count, sizeof(int));
		p += sizeof(int);
		char * data = new char[len];
		char * addr = new char[sizeof(int)];
		char * key = new char[len];
		p += (pcount - 1) * (sizeof(int) + len) + sizeof(int);
		memcpy(data, p, len);
		memcpy(addr, p + len, sizeof(int));	
		//���ڵ�key���ƣ���ڵ�key����
		pa++;
		memcpy(&count, pa, sizeof(int));
		pa += sizeof(int);
		int block;
		for (int i = 0; i < count; i++) {
			memcpy(&block, pa, sizeof(int));
			pa += sizeof(int);
			if (block == left) {
				memcpy(key, pa, len); //�洢���ڵ�key
				memcpy(pa, data, len); //��ڵ�key����
				break;
			}
			pa += len;
		}
		//����ڵ�����ӽڵ�͸��ڵ�key�����ҽڵ�
		q++;
		memcpy(&qcount, q, sizeof(int));
		count = qcount + 1;
		memcpy(q, &count, sizeof(int));
		q += sizeof(int);
		int length = qcount * (sizeof(int) + len) + sizeof(int); //�洢����
		char* temp = new char[length];
		memcpy(temp, q, length);
		memcpy(q, addr, sizeof(int));
		q += sizeof(int);
		memcpy(q, key, len);
		q += len;
		memcpy(q, temp, length); //����ʱ�洢����������д��	
	}
	else {
		int pcount, qcount, count;
		p++;
		memcpy(&pcount, p, sizeof(int));
		//����ڵ�õ�һ��value�����ַ���������ڵ�ɾ��
		count = pcount - 1;
		memcpy(p, &count, sizeof(int));
		p += sizeof(int);
		char * data = new char[len];
		char * addr = new char[2 * sizeof(int)];
		p += (pcount - 1) * (2 * sizeof(int) + len);
		memcpy(addr, p, 2 * sizeof(int));
		memcpy(data, p + 2 * sizeof(int), len);
		memcpy(p, &right, sizeof(int));
	    //��value�����ַ�����ҽڵ�
		q++;
		memcpy(&qcount, q, sizeof(int));
		count = qcount + 1;
		memcpy(q, &count, sizeof(int));
		q += sizeof(int);
		int length = qcount * (2 * sizeof(int) + len) + 4; //�洢����
		char* temp = new char[length];
		memcpy(temp, q, length);
		memcpy(q, addr, 2 * sizeof(int));
		q += 2 * sizeof(int);
		memcpy(q, data, len);
		q += len;
		memcpy(q, temp, length); //����ʱ�洢����������д��
		//���¸��ڵ�ָ���ҽڵ��ָ��
		p = bm->GetBlock(filename, parent);
		p++;
		memcpy(&count, p, sizeof(int));
		p += sizeof(int);
		int block;
		for (int i = 0; i < count; i++) {
			memcpy(&block, p, sizeof(int));
			p += sizeof(int);
			if (block == left) {
				memcpy(p, data, len); //��������
				return;
			}
			p += len;
		}
	}
	bm->set_dirty(filename, left); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, right); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, parent); //��Ǵ˿�Ϊdirty  

}

void IndexManager::BorrowFromRight(string filename, int parent, int left, int right, int len) {
	char *p = bm->GetBlock(filename, left);
	char *q = bm->GetBlock(filename, right);
	char *pa = bm->GetBlock(filename, parent);
	if (p[0] == '?') {
		int pcount, qcount, count;
		q++;
		memcpy(&qcount, q, sizeof(int));
		//���ҽڵ�õ�һ��key�������ӽڵ㲢������ҽڵ�ɾ��
		count = qcount - 1;
		memcpy(q, &count, sizeof(int));
		q += sizeof(int);
		char * data = new char[len];
		char * addr = new char[sizeof(int)];
		char * key = new char[len];
		memcpy(addr, q, sizeof(int));
		memcpy(data, q + sizeof(int), len);
		int length = (qcount - 1) * (sizeof(int) + len) + sizeof(int); //�洢����
		char* temp = new char[length];
		memcpy(temp, q + sizeof(int) + len, length);
		memcpy(q, temp, length);
		//���ڵ�key���ƣ��ҽڵ�key����
		pa++;
		memcpy(&count, pa, sizeof(int));
		pa += sizeof(int);
		int block;
		for (int i = 0; i < count; i++) {
			memcpy(&block, pa, sizeof(int));
			pa += sizeof(int);
			if (block == left) {
				memcpy(key, pa, len); //�洢���ڵ�key
				memcpy(pa, data, len); //�ҽڵ�key����
				break;
			}
			pa += len;
		}		
		//���ҽڵ�����ӽڵ�͸��ڵ�key������ڵ�
		p++;
		memcpy(&pcount, p, sizeof(int));
		count = pcount + 1;
		memcpy(p, &count, sizeof(int));
		p += pcount * (sizeof(int) + len) + 2 * sizeof(int);
		memcpy(p, key, len);
		p += len;
		memcpy(p, addr, sizeof(int));
	}
	else {
		int pcount, qcount, count;
		q++;
		memcpy(&qcount, q, sizeof(int));
		//���ҽڵ�õ�һ��value�����ַ��������ҽڵ�ɾ��
		count = qcount - 1;
		memcpy(q, &count, sizeof(int));
		q += sizeof(int);
		char * data = new char[len];
		char * addr = new char[2 * sizeof(int)];
		memcpy(addr, q, 2 * sizeof(int));
		memcpy(data, q + 2 * sizeof(int), len);
		int length = (qcount - 1) * (2 * sizeof(int) + len) + 4; //�洢����
		char* temp = new char[length];
		memcpy(temp, q + 2 * sizeof(int) + len, length);
		memcpy(q, temp, length);
		//��value�����ַ������ڵ�
		p++;
		memcpy(&pcount, p, sizeof(int));
		count = pcount + 1;
		memcpy(p, &count, sizeof(int));
		p += pcount * (2 * sizeof(int) + len) + sizeof(int);
		memcpy(p, addr, 2 * sizeof(int));
		p += 2 * sizeof(int);
		memcpy(p, data, len);
		p += len;
		memcpy(p, &right, sizeof(int));
		//���¸��ڵ�ָ���ҽڵ��ָ��
		p = bm->GetBlock(filename, parent);
		p++;
		memcpy(&count, p, sizeof(int));
		p += sizeof(int);
		int block;
		for (int i = 0; i < count; i++) {
			memcpy(&block, p, sizeof(int));
			p += sizeof(int);
			if (block == left) {
				memcpy(p, temp, len); //��������
				return;
			}
			p += len;
		}
	}
	bm->set_dirty(filename, left); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, right); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, parent); //��Ǵ˿�Ϊdirty  
}

void IndexManager::MergeLeaves(string filename, int parent, int left, int right, Catalog_manager::Index & index) {
	char *p = bm->GetBlock(filename, left);
	char *q = bm->GetBlock(filename, right);
	int pcount, qcount, count;
	p++;
	memcpy(&pcount, p, sizeof(int));
	q++;
	memcpy(&qcount, q, sizeof(int));
	//�ϲ��������
	count = pcount + qcount;
	memcpy(p, &count, sizeof(int));
	p += sizeof(int) + pcount * (2 * sizeof(int) + index.len); //����count����������
	q += sizeof(int);
	int length = qcount * (2 * sizeof(int) + index.len) + sizeof(int);
	memcpy(p, q, length);
	//���ҽڵ���������
	int free = index.freeNum;
	q = q - sizeof(int) - 1;
	memcpy(q, &free, sizeof(int));
	index.freeNum = right;
	//���¸��ڵ�
	p = bm->GetBlock(filename, parent);
	p++;
	memcpy(&count, p, sizeof(int));
	int num = count - 1;
	memcpy(p, &num, sizeof(int));
	p += sizeof(int);
	int block;
	for (int i = 0; i < count; i++) {
		memcpy(&block, p, sizeof(int));
		p += sizeof(int);
		if (block == left) {
			int length = (count - i - 1) * (sizeof(int) + index.len); //�洢����
			char* temp = new char[length];
			memcpy(temp, p + sizeof(int) + index.len, length);
			memcpy(p, temp, length);
			break;
		}
		p += index.len;
	}
	bm->set_dirty(filename, left); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, right); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, parent); //��Ǵ˿�Ϊdirty  
}

void IndexManager::MergeNodes(string filename, int parent, int left, int right, Catalog_manager::Index & index) {
	char *p = bm->GetBlock(filename, left);
	char *q = bm->GetBlock(filename, right);
	char *pa = bm->GetBlock(filename, parent);
	int pcount, qcount, count;
	p++;
	memcpy(&pcount, p, sizeof(int));
	q++;
	memcpy(&qcount, q, sizeof(int));
	count = pcount + qcount + 1;
	memcpy(p, &count, sizeof(int));
	p += 2 * sizeof(int) + pcount * (sizeof(int) + index.len); //����count����������
	//���ڵ�keyֵ����
	char *key = new char[index.len];
	pa++;
	memcpy(&count, pa, sizeof(int));
	int newcount = count - 1;
	memcpy(pa, &newcount, sizeof(int));
	pa += sizeof(int);
	int block, i;
	for (i = 0; i < count; i++)
	{
		memcpy(&block, pa, sizeof(int));
		pa += sizeof(int);
		if (block == left) {
			memcpy(key, pa, index.len); //�洢���ڵ�key
			break;
		}
		pa += index.len;
	}
	int length = (count - i - 1) * (sizeof(int) + index.len); //�洢����
	char* temp = new char[length];
	memcpy(temp, pa + sizeof(int) + index.len, length);
	memcpy(pa, temp, length);
	memcpy(p, key, index.len);
	p += index.len;
	//�ϲ��������
	q += sizeof(int);
	length = qcount * (sizeof(int) + index.len) + sizeof(int);
	memcpy(p, q, length);
	//���ҽڵ���������
	int free = index.freeNum;
	q = q - sizeof(int) - 1;
	memcpy(q, &free, sizeof(int));
	index.freeNum = right;
	bm->set_dirty(filename, left); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, right); //��Ǵ˿�Ϊdirty  
	bm->set_dirty(filename, parent); //��Ǵ˿�Ϊdirty  
}

int IndexManager::FindEmptyBlock(string tablename, string column)
{
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	int newLeaf; //��Ҷ�ӵĿ��
	if (index.freeNum == -1) {
		newLeaf = bm->TotalBlocks(index.index_name);
	}
	else {
		newLeaf = index.freeNum;
		char* p = bm->GetBlock(index.index_name, newLeaf);
		memcpy(&index.freeNum, p, sizeof(int)); //���¿�����
	}
	return newLeaf;
}
void IndexManager::printBTree(string tablename, string column)
{
	Catalog_manager::Index &index = cm->Get_index(tablename, column);
	if (index.root == -1) 
	{
		cout << "EMPTY TREE" << endl;
		return;
	}
	vector<int> nodes;
	vector<int> layers;
	int layer = 0;
	nodes.push_back(index.root);
	layers.push_back(1);
	while (nodes.size() > 0)
	{
		if (layers[0] != layer) {
			cout << endl << "layers:" << layers[0] << " ";
			layer++;
		}
		int node = nodes.at(0);
		char *p = bm->GetBlock(index.index_name, node);
		if (p[0] == '?')
		{
			int lay = layers[0] + 1;
			cout << "M" << node << " ";
			int count;
			p++;
			memcpy(&count, p, 4);
			p += 4;
			for (int i = 0; i < count; i++)
			{
				int child;
				memcpy(&child, p, 4);
				nodes.push_back(child);
				layers.push_back(lay);
				p += 4;
				int n;
				memcpy(&n, p, 4);
				p += 4;
				cout << n << " ";
			}	
			int child;
			memcpy(&child, p, 4);
			nodes.push_back(child);
			layers.push_back(lay);
			cout << "  ";
		}
		else 
		{
			cout << "L" << node << " ";
			int count;
			p++;
			memcpy(&count, p, 4);
			p += 4;
			for (int i = 0; i < count; i++)
			{
				p += 8;
				int n;
				memcpy(&n, p, 4);
				p += 4;
				cout << n << " ";
			}
			cout << "  ";
	
			int next;
			memcpy(&next, p, sizeof(int));
		
			cout << next << " ";
		}
		vector<int>::iterator k = nodes.begin();
		nodes.erase(k); // ɾ����һ��Ԫ��
		vector<int>::iterator j = layers.begin();
		layers.erase(j); // ɾ����һ��Ԫ��
	}



}
