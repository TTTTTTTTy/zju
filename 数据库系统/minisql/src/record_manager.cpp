#include "record_manager.h"
#include "DB.hpp"

int RecordManager::InsertRecord(string TableName, vector<string>& Record) {
	char *p1, *p2;
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
    //检查插入数据的参数数量
	if (cols.size() != Record.size()) {
		cout << "Wrong number of parameters!" << endl << endl;
		return 0;
	}
	//检查插入数据的合法性
	for (int i = 0; i < cols.size(); i++) {
		//如果是char类型，检查字符串长度是否超过限制
		if (cols.at(i).type == 0) { //char
			if (Record.at(i).length() + 1 > cols.at(i).len) { 
				cout << "Character length exceeds limit!" << endl << endl;;
				return 0;
			}
		}//检查是否是int类型或float类型， 是否包含非法字符
		else if ((cols.at(i).type == 1 && !IsInt(Record.at(i))) || (cols.at(i).type == 2 && !IsFloat(Record.at(i)))) {
			cout << "Worng data type!" << endl << endl;
		}
		else if(cols.at(i).type != 1 && cols.at(i).type != 2){
			cout << "Unknown data type!" << endl << endl;
			return 0;
		}
	}
	int bs = bm->TotalBlocks(TableName);  //获取文件块数
	if (bs == 0) { //文件不存在
		bm->NewFile(TableName); //创建新文件
		p1 = bm->GetBlock(TableName, 0, true);  //得到第一个块
		InsertAtBlock(TableName, 0, Record);
	}
	else { 	//如果表中已有记录，判断在unique的属性上是否有重复
		vector<int> columns;
		vector<string> values;
		for (int i = 0; i < cols.size(); i++) {
			if (!cols.at(i).is_unique) //非unique， 跳过
				continue;
			if (cm->has_index(TableName, cols.at(i).column_name)) { //如果有索引，直接通过索引查询是否重复
				int x, y;
				if (im->SearchOne(TableName, cols.at(i).column_name, Record.at(i), x, y)) {
					cout << "Do not allow duplicate values of " << cols.at(i).column_name << " in Table " << TableName << endl << endl;
					return 0;
				}

			}
			else { //将数据压入容器中，等待检查
				columns.push_back(i);
				values.push_back(Record.at(i));
			}
		}
		if (columns.size() > 0) { //如果存在待检查的数据
			int i = IsExist(TableName, columns, values); //遍历所有记录检查是否重复
			if (i != -1) {
				cout << "Do not allow duplicate values of " << cols.at(i).column_name << " in Table " << TableName << endl << endl;
				return 0;
			}
		}
		//通过所有检查，插入记录
		int recordLength = cm->Calculate_rec(TableName);	
		int recordNum;
		int maxRecord = floor((BLOCKSIZE - sizeof(int)) / recordLength); //计算每个块中最多可容纳的记录数
		int blockIndex = 0;
		while (blockIndex < bs) //判断是否有没满的块
		{
			p1 = bm->GetBlock(TableName, blockIndex);
			memcpy(&recordNum, p1, sizeof(int)); //得到这个块中存储的记录数
			if (recordNum < maxRecord) {  //这个块没有满
				InsertAtBlock(TableName, blockIndex, Record);
				return 1;
			}
			blockIndex++;

		}
		p1 = bm->GetBlock(TableName, blockIndex, true); //创建新块
		int num = 0;
		memcpy(p1, &num, sizeof(int));
		InsertAtBlock(TableName, blockIndex, Record); //插入记录
		return 1;
	}		
}

void RecordManager::InsertAtBlock(string TableName, int block, vector<string>& Record) {
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	char *p = bm->GetBlock(TableName, block), *q;
	int recordNum;
	int recordLength = cm->Calculate_rec(TableName); //记录长度
	memcpy(&recordNum, p, sizeof(int)); //得到这个块中存储的记录数
	recordNum++;
	memcpy(p, &recordNum, sizeof(int)); //更新这个块中存储的记录数
    p = p + sizeof(int) + recordLength * (recordNum - 1);  //指针移动到插入位置,注意此时recordNum已经加一
	q = p;
	int in;
	float f;
	stringstream stream;
	for (int i = 0; i < cols.size(); i++) {
		if (cols.at(i).type == 0) { //char
			memcpy(p, Record.at(i).data(), Record.at(i).length() + 1); //注意'\0'
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
	bm->set_dirty(TableName, block); //标记此块为dirty  
	bm->write_all_back(); //将数据从buffer写回file
	cm->Insert_rec(TableName, 1); //记录数加一
	int pos = 0;
	for (int i = 0; i < cols.size(); i++) {  //更新索引
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
	for (int i = 0; i < cols.size(); i++) {  //更新索引
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
	int bs = bm->TotalBlocks(TableName);  //获取文件块数
	int block = 0; //块下标
	char *p1, *p2;
	int recordLength = cm->Calculate_rec(TableName);
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	vector<int> printLength; //记录每条属性的打印长度（控制间距）
	int colNum = cols.size();
	cout.setf(ios::left); //设置对齐方式为left 
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
		cout << cols.at(i).column_name; //输出属性
	}
	cout << endl; //换行
	while (block < bs)  //当bs为0时直接跳出，即只输出属性
	{
		float f;
		int in, recordNum;
		char value[MAXLENGTH];
		p1 = bm->GetBlock(TableName, block);
		memcpy(&recordNum, p1, sizeof(int)); //此块中有几条记录
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //如果该记录已被删除，则跳过
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
			cout << endl; //换行
		}
		block++; //读取下一块
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
	int bs = bm->TotalBlocks(TableName);  //获取文件块数
	int block = 0; //块下标
	int recordNum; //每块中的记录数量
	char *p1, *p2, *p3;
	int recordLength = cm->Calculate_rec(TableName);
	while (block < bs)
	{
		float f;
		int in;
		char recordValue[MAXLENGTH];
		p1 = bm->GetBlock(TableName, block);
		memcpy(&recordNum, p1, sizeof(int)); //此块中有几条记录
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //如果该记录已被删除，则跳过
				continue;
			int flag = 1;
			//比较是否满足条件
			for (int j = 0; j < index.size(); j++) {
				p3 = p2 + pos.at(j);
				memcpy(recordValue, p3, length.at(j));
				if (ConfirmToWhere(recordValue, type.at(j), op, value.at(j))) {
					return index.at(j);
				}
			}
		}
		block++; //读取下一块
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
		else{ //非法字符
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
		else { //非法字符
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
		else { //非法字符
			cout << "Illegal operation" << endl << endl;
			return false;
		}
	}
}

int RecordManager::SelectRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions) {
	int bs = bm->TotalBlocks(TableName);  //获取文件块数
	int block = 0; //块下标
	int recordNum; //每块中的记录数量
	int selectNum = 0;
	char *p1, *p2, *p3;
	int recordLength = cm->Calculate_rec(TableName);
	vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
	vector<int> printLength; //记录每条属性的打印长度（控制间距）
	int colNum = cols.size();
	cout.setf(ios::left); //设置对齐方式为left 
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
		cout << cols.at(i).column_name; //输出属性
	}
	cout << endl; //换行
	vector<int> index; //每一个要比较的column的下标
	for (int i = 0; i < columns.size(); i++) {
		for (int j = 0; j < cols.size(); j++) {
			if (columns.at(i) == cols.at(j).column_name) {
				index.push_back(j);
				break;
			}				
		}
	}
	if (index.size() != columns.size()) { //判断是否输入的每个属性都合法
		cout << "Non-existent columns!" << endl << endl;
		return 0;
	}
	bool checked = false; //是否已通过索引查询
	vector<int> blockResult, offsetResult;
	for (int i = 0; i < columns.size(); i++) { //检查每一个条件是否存在索引
		if (cm->has_index(TableName, columns.at(i)) && op.at(i) != "<>") {
			if (op.at(i) == "=") {  //存在索引且条件为"="
				int Block, Offset;
				if (im->SearchOne(TableName, columns.at(i), conditions.at(i), Block, Offset) == 0) {
					return 0;
				}
				else { //判断是否符合其他条件
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
				checked = true; //标记已通过索引查询过
				break;
			}			
		}
	}
	if (checked) {  //通过索引已找到符合部分条件的记录
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
    //不存在索引
	while (block < bs)
	{
		float f;
		int in;
		char value[MAXLENGTH];
		p1 = bm->GetBlock(TableName, block);
		memcpy(&recordNum, p1, sizeof(int)); //此块中有几条记录
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //如果该记录已被删除，则跳过
				continue;
			int flag = 1;
			//比较是否满足条件
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
		block++; //读取下一块
	}
	return selectNum;
}

int RecordManager::DeletetRecord(string TableName, vector<string> &columns, vector<string> &op, vector<string> &conditions) {
	int bs = bm->TotalBlocks(TableName);  //获取文件块数
	if (bs == 0) {
		return 0;
	}
	else {
		int block = 0; //块下标
		int deleteNum = 0; //已删除几条记录
		int recordNum; //每个块中的记录数
		char *p1, *p2, *p3, *p4;
		int recordLength = cm->Calculate_rec(TableName);
		vector<Catalog_manager::Column> cols = cm->Get_table_col(TableName);
		vector<int> index; //每一个要比较的column的下标
		for (int i = 0; i < columns.size(); i++) {
			for (int j = 0; j < cols.size(); j++) {
				if (columns.at(i) == cols.at(j).column_name)
					index.push_back(j);
			}
		}
		int *hasChecked = new int[columns.size()](); //各属性是否存在可搜索的索引,数组初始化为零
		bool checked = false; //初始化不存在索引
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
					checked = true; //标记已通过索引查询过
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
		//条件上不存在索引
		while (block < bs)
		{
			char value[MAXLENGTH];
			p1 = bm->GetBlock(TableName, block);
			memcpy(&recordNum, p1, sizeof(int)); //此块中有几条记录
			p1 += sizeof(int);
			for (int i = 0; i < recordNum; i++) {
				p2 = p1 + i * recordLength;
				char del[4];
				memcpy(del, p2, 4);
				if (!strcmp(del, "del"))  //如果该记录已被删除，则跳过
					continue;
				int flag = 1;
				//比较是否满足条件
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
		    	if (flag) {  //删除此条记录及其索引
					memcpy(p2, "end", 4); //删除标志
					deleteNum++;
					cm->Delect_recs(TableName, 1);
					bm->set_dirty(TableName, block); //标记此块为dirty  
					int pos = 0;
					for (int j = 0; j < cols.size(); j++) {  //更新索引
						if (cm->has_index(TableName, cols.at(j).column_name)) {
							char *data = new char[cols.at(j).len];
							memcpy(data, p2 + pos, cols.at(j).len);
							im->DeleteRecord(TableName, cols.at(j).column_name, data);
						}
						pos += cols.at(i).len;
					}
				}
			}
			block++; //读取下一块
		}
		bm->write_all_back(); //将所有块的内容写回文件
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
//		if (i >= block2.size()) { //删除指定元素，返回指向删除元素的下一个元素的位置的迭代器
//			itBlock = block1.erase(itBlock);
//			itOffset = offset1.erase(itOffset);
//		}
//		else {
//			++itBlock; //迭代器指向下一个元素位置
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
	cm->Delect_recs(TableName, 1); //更新记录数
	int pos = 0;
	for (int i = 0; i < cols.size(); i++) {  //更新索引
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
	int bs = bm->TotalBlocks(TableName);  //获取文件块数
	int blockIndex = 0; //块下标
	int pos = 0; //属性在记录中的位置
	int length; //属性长度
	int recordNum; //块中有几条记录
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
	while (blockIndex < bs)  //当bs为0时跳出
	{
		p1 = bm->GetBlock(TableName, blockIndex);
		memcpy(&recordNum, p1, sizeof(int)); //此块中有几条记录
		p1 += sizeof(int);
		for (int i = 0; i < recordNum; i++) {
			p2 = p1 + i * recordLength;
			char del[4];
			memcpy(del, p2, 4);
			if (!strcmp(del, "del"))  //如果该记录已被删除，则跳过
				continue;
			p2 += pos;
			char *value = new char[length];
			memcpy(value, p2, length);
			Values.push_back(value);
			block.push_back(blockIndex);
			offset.push_back(i + 1);
		}
		blockIndex++; //读取下一块
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