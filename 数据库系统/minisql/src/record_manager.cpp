#include "record_manager.h"
#include "DB.hpp"

int RecordManager::InsertRecord(string TableName, vector<string>& Record) {
	char *p1, *p2;
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
    //���������ݵĲ�������
	if (cols.size() != Record.size()) {
		cout << "Wrong number of parameters!" << endl << endl;
		return 0;
	}
	//���������ݵĺϷ���
	for (int i = 0; i < cols.size(); i++) {
		//�����char���ͣ�����ַ��������Ƿ񳬹�����
		if (cols.at(i).type == 0) { //char
			if (Record.at(i).length() + 1 > cols.at(i).len) { 
				cout << "Character length exceeds limit!" << endl << endl;;
				return 0;
			}
		}//����Ƿ���int���ͻ�float���ͣ� �Ƿ�����Ƿ��ַ�
		else if ((cols.at(i).type == 1 && !IsInt(Record.at(i))) || (cols.at(i).type == 2 && !IsFloat(Record.at(i)))) {
			cout << "Worng data type!" << endl << endl;
		}
		else if(cols.at(i).type != 1 && cols.at(i).type != 2){
			cout << "Unknown data type!" << endl << endl;
			return 0;
		}
	}
	int bs = bm->TotalBlocks(TableName);  //��ȡ�ļ�����
	if (bs == 0) { //�ļ�������
		bm->NewFile(TableName); //�������ļ�
		p1 = bm->GetBlock(TableName, 0, true);  //�õ���һ����
		InsertAtBlock(TableName, 0, Record);
	}
	else { 	//����������м�¼���ж���unique���������Ƿ����ظ�
		vector<int> columns;
		vector<string> values;
		for (int i = 0; i < cols.size(); i++) {
			if (!cols.at(i).is_unique) //��unique�� ����
				continue;
			if (cm->has_index(TableName, cols.at(i).column_name)) { //�����������ֱ��ͨ��������ѯ�Ƿ��ظ�
				int x, y;
				if (im->SearchOne(TableName, cols.at(i).column_name, Record.at(i), x, y)) {
					cout << "Do not allow duplicate values of " << cols.at(i).column_name << " in Table " << TableName << endl << endl;
					return 0;
				}

			}
			else { //������ѹ�������У��ȴ����
				columns.push_back(i);
				values.push_back(Record.at(i));
			}
		}
		if (columns.size() > 0) { //������ڴ���������
			int i = IsExist(TableName, columns, values); //�������м�¼����Ƿ��ظ�
			if (i != -1) {
				cout << "Do not allow duplicate values of " << cols.at(i).column_name << " in Table " << TableName << endl << endl;
				return 0;
			}
		}
		//ͨ�����м�飬�����¼
		int recordLength = cm->Calculate_rec(TableName);	
		int recordNum;
		int maxRecord = floor((BLOCKSIZE - sizeof(int)) / recordLength); //����ÿ�������������ɵļ�¼��
		int blockIndex = 0;
		while (blockIndex < bs) //�ж��Ƿ���û���Ŀ�
		{
			p1 = bm->GetBlock(TableName, blockIndex);
			memcpy(&recordNum, p1, sizeof(int)); //�õ�������д洢�ļ�¼��
			if (recordNum < maxRecord) {  //�����û����
				InsertAtBlock(TableName, blockIndex, Record);
				return 1;
			}
			blockIndex++;

		}
		p1 = bm->GetBlock(TableName, blockIndex, true); //�����¿�
		int num = 0;
		memcpy(p1, &num, sizeof(int));
		InsertAtBlock(TableName, blockIndex, Record); //�����¼
		return 1;
	}		
}

void RecordManager::InsertAtBlock(string TableName, int block, vector<string>& Record) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	char *p = bm->GetBlock(TableName, block), *q;
	int recordNum;
	int recordLength = cm->Calculate_rec(TableName); //��¼����
	memcpy(&recordNum, p, sizeof(int)); //�õ�������д洢�ļ�¼��
	recordNum++;
	memcpy(p, &recordNum, sizeof(int)); //����������д洢�ļ�¼��
    p = p + sizeof(int) + recordLength * (recordNum - 1);  //ָ���ƶ�������λ��,ע���ʱrecordNum�Ѿ���һ
	q = p;
	int in;
	float f;
	stringstream stream;
	for (int i = 0; i < cols.size(); i++) {
		if (cols.at(i).type == 0) { //char
			memcpy(p, Record.at(i).data(), Record.at(i).length() + 1); //ע��'\0'
			p += cols.at(i).len;
		}
		else {
			stream.clear();
			stream.str(Record.at(i).data());
			if (cols.at(i).type == 1) { //int				
				stream >> in;
				memcpy(p, &in, sizeof(int));
				p += sizeof(int);
			}
			else if (cols.at(i).type == 2) { //float
				stream >> f;
				memcpy(p, &f, sizeof(float));
				p += sizeof(float);
			}	
		}
	}
	bm->set_dirty(TableName, block); //��Ǵ˿�Ϊdirty  
	bm->write_all_back(); //�����ݴ�bufferд��file
	cm->Insert_rec(TableName, 1); //��¼����һ
	int pos = 0;
	for (int i = 0; i < cols.size(); i++) {  //��������
		if (cm->has_index(TableName, cols.at(i).column_name)) {
			char *data = new char[cols.at(i).len];
			memcpy(data, q + pos, cols.at(i).len);
			im->InsertRecord(TableName, cols.at(i).column_name, data, block, recordNum);
		}
		pos += cols.at(i).len;
	}
}

int RecordManager::DeleteRecord(string TableName) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	for (int i = 0; i < cols.size(); i++) {  //��������
		if (cm->has_index(TableName, cols.at(i).column_name)) {
			im->DeleteIndex(TableName, cols.at(i).column_name);
		}
	}
	bm->DelFile(TableName);
	int num = cm->Get_rec_num(TableName);
	cm->Delete_all(TableName);
	return num;
}

int RecordManager::SelectRecord(string TableName) {
	int bs = bm->TotalBlocks(TableName);  //��ȡ�ļ�����
	int block = 0; //���±�
	char *p1, *p2;
	int recordLength = cm->Calculate_rec(TableName);
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	vector<int> printLength; //��¼ÿ�����ԵĴ�ӡ���ȣ����Ƽ�ࣩ
	int colNum = cols.size();
	cout.setf(ios::left); //���ö��뷽ʽΪleft 
	for (int i = 0; i < colNum; i++)
	{
		int tempLength = (cols.at(i).column_name.length() > cols.at(i).len) ? cols.at(i).column_name.length() : cols.at(i).len;
		int length;
		if (tempLength > PRINTLENGTH)
			length = tempLength + 2;
		else
			length = PRINTLENGTH + 2;
		printLength.push_back(length);
		cout.width(length);
		cout << cols.at(i).column_name; //�������
	}
	cout << endl; //����
	while (block < bs)  //��bsΪ0ʱֱ����������ֻ�������
	{
		float f;
		int in, recordNum;
		char value[MAXLENGTH];
		p1 = bm->GetBlock(TableName, block);
		memcpy(&recordNum, p1, sizeof(int)); //�˿����м�����¼
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //����ü�¼�ѱ�ɾ����������
				continue;
			for (int j = 0; j < colNum; j++)
			{
				if (cols.at(j).type == 0) { //char
					memcpy(value, p2, cols.at(j).len);
					cout.width(printLength.at(j));
					cout << value;
					p2 += cols.at(j).len;
				}
				else if (cols.at(j).type == 1) { //int				
					memcpy(&in, p2, sizeof(int));
					cout.width(printLength.at(j));
					cout << in;
					p2 += sizeof(int);
				}
				else { //float
					memcpy(&f, p2, sizeof(float));
					cout.width(printLength.at(j));
					cout << f;
					p2 += sizeof(float);
				}
			}
			cout << endl; //����
		}
		block++; //��ȡ��һ��
	}
	return cm->Get_rec_num(TableName);
}

bool RecordManager::ConfirmToWhere(string TableName, char* record, vector<string> columns, vector<string> ops, vector<string> conditions) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	for (int i = 0; i < cols.size(); i++) {
		char* value = new char[cols.at(i).len];
		memcpy(value, record, cols.at(i).len);
		for (int j = 0; j < columns.size(); j++) {
			if (columns.at(j) == cols.at(i).column_name) {			
				if (!ConfirmToWhere(value, cols.at(i).type, ops.at(j), conditions.at(j))) {
					return false;
				}
			}
		}
		record += cols.at(i).len;
	}
	return true;
}

int RecordManager::IsExist(string TableName, vector<int>& index, vector<string>& value) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	vector<int> length, pos, type;
	string op("=");
	int location = 0;
	for (int i = 0, j = 0; i < cols.size() && j < index.size(); i++) {
		if (index.at(j) == i) {
			length.push_back(cols.at(i).len);
			pos.push_back(location);
			type.push_back(cols.at(i).type);
			j++;
		}
		location += cols.at(i).len;			
	}
	int bs = bm->TotalBlocks(TableName);  //��ȡ�ļ�����
	int block = 0; //���±�
	int recordNum; //ÿ���еļ�¼����
	char *p1, *p2, *p3;
	int recordLength = cm->Calculate_rec(TableName);
	while (block < bs)
	{
		float f;
		int in;
		char recordValue[MAXLENGTH];
		p1 = bm->GetBlock(TableName, block);
		memcpy(&recordNum, p1, sizeof(int)); //�˿����м�����¼
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //����ü�¼�ѱ�ɾ����������
				continue;
			int flag = 1;
			//�Ƚ��Ƿ���������
			for (int j = 0; j < index.size(); j++) {
				p3 = p2 + pos.at(j);
				memcpy(recordValue, p3, length.at(j));
				if (ConfirmToWhere(recordValue, type.at(j), op, value.at(j))) {
					return index.at(j);
				}
			}
		}
		block++; //��ȡ��һ��
	}	
	return -1;
}

bool RecordManager::ConfirmToWhere(char* value, int type, string op, string condition) {
	int vi, ci;
	float vf, cf;
	if (type == 0) { //char
		if (op == "=") {
			if (!strcmp(value, condition.data()))
				return true;
			else
				return false;
		}
		else if (op == "<>") {
			if (!strcmp(value, condition.data()))
				return false;
			else
				return true;
		}
		else if (op == "<") {
			if (strcmp(value, condition.data()) < 0)
				return true;
			else
				return false;
		}
		else if (op == ">") {
			if (strcmp(value, condition.data()) > 0)
				return true;
			else
				return false;
		}
		else if (op == "<=") {
			if (strcmp(value, condition.data()) <= 0)
				return true;
			else
				return false;
		}
		else if (op == ">=") {
			if (strcmp(value, condition.data()) >= 0)
				return true;
			else
				return false;
		}
		else{ //�Ƿ��ַ�
			cout << "Illegal operation" << endl; 
			return false; 
		}
	}
	else if (type == 1) { //int
		stringstream stream(condition);
		stream >> ci;
		memcpy(&vi, value, sizeof(int));
		if (op == "=") 
			return (vi == ci) ? true : false;
		else if (op == "<>")
			return (vi != ci) ? true : false;
		else if (op == "<") 
			return (vi < ci) ? true : false;
		else if (op == ">")
			return (vi > ci) ? true : false;
		else if (op == "<=") 
			return (vi <= ci) ? true : false;
		else if (op == ">=") 
			return (vi >= ci) ? true : false;
		else { //�Ƿ��ַ�
			cout << "Illegal operation" << endl;
			return false;
		}
	}
	else { //float
		stringstream stream(condition);
		stream >> cf;
		memcpy(&vf, value, sizeof(float));
		if (op == "=")
			return (vf == cf) ? true : false;
		else if (op == "<>")
			return (vf != cf) ? true : false;
		else if (op == "<")
			return (vf < cf) ? true : false;
		else if (op == ">")
			return (vf > cf) ? true : false;
		else if (op == "<=")
			return (vf <= cf) ? true : false;
		else if (op == ">=")
			return (vf >= cf) ? true : false;
		else { //�Ƿ��ַ�
			cout << "Illegal operation" << endl << endl;
			return false;
		}
	}
}

int RecordManager::SelectRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions) {
	int bs = bm->TotalBlocks(TableName);  //��ȡ�ļ�����
	int block = 0; //���±�
	int recordNum; //ÿ���еļ�¼����
	int selectNum = 0;
	char *p1, *p2, *p3;
	int recordLength = cm->Calculate_rec(TableName);
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	vector<int> printLength; //��¼ÿ�����ԵĴ�ӡ���ȣ����Ƽ�ࣩ
	int colNum = cols.size();
	cout.setf(ios::left); //���ö��뷽ʽΪleft 
	for (int i = 0; i < colNum; i++)
	{
		int tempLength = (cols.at(i).column_name.length() > cols.at(i).len) ? cols.at(i).column_name.length() : cols.at(i).len;
		int length;
		if (tempLength > PRINTLENGTH)
			length = tempLength + 2;
		else
			length = PRINTLENGTH + 2;
		printLength.push_back(length);
		cout.width(length);
		cout << cols.at(i).column_name; //�������
	}
	cout << endl; //����
	vector<int> index; //ÿһ��Ҫ�Ƚϵ�column���±�
	for (int i = 0; i < columns.size(); i++) {
		for (int j = 0; j < cols.size(); j++) {
			if (columns.at(i) == cols.at(j).column_name) {
				index.push_back(j);
				break;
			}				
		}
	}
	if (index.size() != columns.size()) { //�ж��Ƿ������ÿ�����Զ��Ϸ�
		cout << "Non-existent columns!" << endl << endl;
		return 0;
	}
	bool checked = false; //�Ƿ���ͨ��������ѯ
	vector<int> blockResult, offsetResult;
	for (int i = 0; i < columns.size(); i++) { //���ÿһ�������Ƿ��������
		if (cm->has_index(TableName, columns.at(i)) && op.at(i) != "<>") {
			if (op.at(i) == "=") {  //��������������Ϊ"="
				int Block, Offset;
				if (im->SearchOne(TableName, columns.at(i), conditions.at(i), Block, Offset) == 0) {
					return 0;
				}
				else { //�ж��Ƿ������������
					char *record = GetData(TableName, Block, Offset);
					if (ConfirmToWhere(TableName, record, columns, op, conditions)) {
						PrintRecord(TableName, record);
						selectNum++;
						return selectNum;
					}					
					return 0;
				}
			}
			else {
				im->SearchMany(TableName, columns.at(i), conditions.at(i), cols.at(index.at(i)).type, op.at(i), blockResult, offsetResult);
		/*		else
				{
					tmpblock.clear();
					tmpoffset.clear();
					im->SearchMany(TableName, columns.at(i), conditions.at(i), cols.at(index.at(i)).type, op.at(i), tmpblock, tmpoffset);
					Intersection(blockResult, offsetResult, tmpblock, tmpoffset);					
				}	*/
				checked = true; //�����ͨ��������ѯ��
				break;
			}			
		}
	}
	if (checked) {  //ͨ���������ҵ����ϲ��������ļ�¼
		for (int i = 0; i < blockResult.size(); i++) {
			char *record = GetData(TableName, blockResult.at(i), offsetResult.at(i));
			if (ConfirmToWhere(TableName, record, columns, op, conditions)) {
				PrintRecord(TableName, record);
				selectNum++;
			}   
		}
		return selectNum;
	}
	selectNum = 0;
    //����������
	while (block < bs)
	{
		float f;
		int in;
		char value[MAXLENGTH];
		p1 = bm->GetBlock(TableName, block);
		memcpy(&recordNum, p1, sizeof(int)); //�˿����м�����¼
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //����ü�¼�ѱ�ɾ����������
				continue;
			int flag = 1;
			//�Ƚ��Ƿ���������
			for (int j = 0; j < columns.size(); j++) {
				p3 = p2;
				int k;
				for (k = 0; k < index.at(j); k++)
					p3 += cols.at(k).len;
				memcpy(value, p3, cols.at(k).len);
				if (ConfirmToWhere(value, cols.at(k).type, op.at(j), conditions.at(j)) == false) {
					flag = 0;
					break;
				}
			}
			if (flag) {
				selectNum++;
				memcpy(value, p2, recordLength);
				PrintRecord(TableName, value);
			}
		}
		block++; //��ȡ��һ��
	}
	return selectNum;
}

int RecordManager::DeletetRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions) {
	int bs = bm->TotalBlocks(TableName);  //��ȡ�ļ�����
	if (bs == 0) {
		return 0;
	}
	else {
		int block = 0; //���±�
		int deleteNum = 0; //��ɾ��������¼
		int recordNum; //ÿ�����еļ�¼��
		char *p1, *p2, *p3, *p4;
		int recordLength = cm->Calculate_rec(TableName);
		vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
		vector<int> index; //ÿһ��Ҫ�Ƚϵ�column���±�
		for (int i = 0; i < columns.size(); i++) {
			for (int j = 0; j < cols.size(); j++) {
				if (columns.at(i) == cols.at(j).column_name)
					index.push_back(j);
			}
		}
		int *hasChecked = new int[columns.size()](); //�������Ƿ���ڿ�����������,�����ʼ��Ϊ��
		bool checked = false; //��ʼ������������
		vector<int> blockResult, offsetResult, tmpblock, tmpoffset;
		for (int i = 0; i < columns.size(); i++) {
			if (cm->has_index(TableName, columns.at(i)) && op.at(i) != "<>") {
				if (op.at(i) == "=") {
					int Block, Offset;
					if (im->SearchOne(TableName, columns.at(i), conditions.at(i), Block, Offset) == 0) {
						return deleteNum;
					}
					else {
						char *record = GetData(TableName, Block, Offset);
						for (int j = 0; j < columns.size(); j++) {
							if (j == i)
								continue;
							if (!ConfirmToWhere(record, cols.at(j).type, op.at(j), conditions.at(j)))
								return deleteNum;
						}
						SetDelete(TableName, Block, Offset);
						deleteNum++;
						return deleteNum;
					}
				}
				else {			
					im->SearchMany(TableName, columns.at(i), conditions.at(i), cols.at(i).type, op.at(i), blockResult, offsetResult);
					checked = true; //�����ͨ��������ѯ��
					break;
				}
			}
		}
		if (checked) {
			for (int i = 0; i < blockResult.size(); i++) {
				char *record = GetData(TableName, blockResult.at(i), offsetResult.at(i));
				if (ConfirmToWhere(TableName, record, columns, op, conditions)) {
					SetDelete(TableName, blockResult.at(i), offsetResult.at(i));
					deleteNum++;
				}
				
			}
			return deleteNum;
		}
		//�����ϲ���������
		while (block < bs)
		{
			char value[MAXLENGTH];
			p1 = bm->GetBlock(TableName, block);
			memcpy(&recordNum, p1, sizeof(int)); //�˿����м�����¼
			p1 += sizeof(int);
			for (int i = 0; i < recordNum; i++) {
				p2 = p1 + i * recordLength;
				char del[4];
				memcpy(del, p2, 4);
				if (!strcmp(del, "del"))  //����ü�¼�ѱ�ɾ����������
					continue;
				int flag = 1;
				//�Ƚ��Ƿ���������
				for (int j = 0; j < columns.size(); j++) {
					p3 = p2;
					int k;
					for (k = 0; k < index.at(j); k++)
						p3 += cols.at(k).len;
					memcpy(value, p3, cols.at(k).len);
					if (ConfirmToWhere(value, cols.at(k).type, op.at(j), conditions.at(j)) == false) {
						flag = 0;
						break;
					}
				}
		    	if (flag) {  //ɾ��������¼��������
					memcpy(p2, "end", 4); //ɾ����־
					deleteNum++;
					cm->Delect_recs(TableName, 1);
					bm->set_dirty(TableName, block); //��Ǵ˿�Ϊdirty  
					int pos = 0;
					for (int j = 0; j < cols.size(); j++) {  //��������
						if (cm->has_index(TableName, cols.at(j).column_name)) {
							char *data = new char[cols.at(j).len];
							memcpy(data, p2 + pos, cols.at(j).len);
							im->DeleteRecord(TableName, cols.at(j).column_name, data);
						}
						pos += cols.at(i).len;
					}
				}
			}
			block++; //��ȡ��һ��
		}
		bm->write_all_back(); //�����п������д���ļ�
		return deleteNum;
	}
}

//void RecordManager::Intersection(vector<int> &block1, vector<int> &offset1, vector<int> &block2, vector<int> &offset2) {
//	
//	
//	vector<int>::iterator itBlock = block1.begin();
//	vector<int>::iterator itOffset = offset1.begin();
//	for (; itBlock != block1.end();)
//	{
//		int i;
//		for (i = 0; i < block2.size(); i++) {
//			if ((*itBlock == block2.at(i)) && (*itOffset == offset2.at(i)))
//				break;
//		}
//		if (i >= block2.size()) { //ɾ��ָ��Ԫ�أ�����ָ��ɾ��Ԫ�ص���һ��Ԫ�ص�λ�õĵ�����
//			itBlock = block1.erase(itBlock);
//			itOffset = offset1.erase(itOffset);
//		}
//		else {
//			++itBlock; //������ָ����һ��Ԫ��λ��
//			++itOffset;
//		}			
//
//	}
//}



char*  RecordManager::GetData(string TableName, int block, int offset) {
	char *p = bm->GetBlock(TableName, block);
	int recordNum, recordLength = cm->Calculate_rec(TableName);;
	memcpy(&recordNum, p, sizeof(int)); 
	p += sizeof(int);
	p += recordLength * (offset - 1);
	char * data = new char[recordLength];
	memcpy(data, p, recordLength);
	return data;
}

void RecordManager::SetDelete(string TableName, int block, int offset) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	char *p = bm->GetBlock(TableName, block);
	int recordNum, recordLength = cm->Calculate_rec(TableName);;
	memcpy(&recordNum, p, sizeof(int));
	p += sizeof(int);
	p += recordLength * (offset - 1);
	char * data = new char[recordLength];
	memcpy(data, p, recordLength);
	memcpy(p, "del", 4);
	cm->Delect_recs(TableName, 1); //���¼�¼��
	int pos = 0;
	for (int i = 0; i < cols.size(); i++) {  //��������
		if (cm->has_index(TableName, cols.at(i).column_name)) {
			char *key = new char[cols.at(i).len];
			memcpy(key, data + pos, cols.at(i).len);
			im->DeleteRecord(TableName, cols.at(i).column_name, key);
		}
		pos += cols.at(i).len;
	}
	
}

void RecordManager::PrintRecord(string TableName, char* record) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	for (int j = 0; j < cols.size(); j++)
	{
		int tempLength = (cols.at(j).column_name.length() > cols.at(j).len) ? cols.at(j).column_name.length() : cols.at(j).len;
		int length;
		if (tempLength > PRINTLENGTH)
			length = tempLength + 2;
		else
			length = PRINTLENGTH + 2;
		if (cols.at(j).type == 0) { //char
			char *value = new char[cols.at(j).len];
			memcpy(value, record, cols.at(j).len);
			cout.width(length);
			cout << value;
			record += cols.at(j).len;
		}
		else if (cols.at(j).type == 1) { //int		
			int in;
			memcpy(&in, record, sizeof(int));
			cout.width(length);
			cout << in;
			record += sizeof(int);
		}
		else { //float
			float f;
			memcpy(&f, record, sizeof(float));
			cout.width(length);
			cout << f;
			record += sizeof(float);
		}
	}
	cout << endl;
}

void RecordManager::SelectValue(string TableName, string Column, vector<char *> &Values, vector<int> &block, vector<int> &offset) {
	int bs = bm->TotalBlocks(TableName);  //��ȡ�ļ�����
	int blockIndex = 0; //���±�
	int pos = 0; //�����ڼ�¼�е�λ��
	int length; //���Գ���
	int recordNum; //�����м�����¼
	char *p1, *p2;
	int recordLength = cm->Calculate_rec(TableName);
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	int colNum = cols.size();
	for (int i = 0; i < colNum; i++)
	{
		if (cols.at(i).column_name == Column) {
			length = cols.at(i).len;
			break;
		}
		else {
			pos += cols.at(i).len;
		}
	}
	while (blockIndex < bs)  //��bsΪ0ʱ����
	{
		p1 = bm->GetBlock(TableName, blockIndex);
		memcpy(&recordNum, p1, sizeof(int)); //�˿����м�����¼
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //����ü�¼�ѱ�ɾ����������
				continue;
			p2 += pos;
			char *value = new char[length];
			memcpy(value, p2, length);
			Values.push_back(value);
			block.push_back(blockIndex);
			offset.push_back(i + 1);
		}
		blockIndex++; //��ȡ��һ��
	}
}

bool RecordManager::IsInt(string s)
{
	stringstream sin(s);
	int in;
	//double t;
	char p;
	if (!(sin >> in))
		return false;
	if (sin >> p)
		return false;
	else 
		return true;
}

bool RecordManager::IsFloat(string s)
{
	stringstream sin(s);
    float in;
	//double t;
	char p;
	if (!(sin >> in))
		return false;
	if (sin >> p)
		return false;
	else 
		return true;
}