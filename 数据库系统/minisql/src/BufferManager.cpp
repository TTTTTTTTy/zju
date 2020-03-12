#include "BufferManager.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <utility>
#include <cstring>

using std::ios;
using std::fstream;
using std::ofstream;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::swap;

namespace MiniSQL {

	Block_t& Block_t::write_back() {
		fstream fp(filename, ios::binary | ios::in | ios::out);
		if (!fp) {
			cerr << "File: " << filename << ", Block:" << ID << ",write back failed!" << endl;
			return *this;
		}
#ifdef DEBUG_MODE
		cout << "File: " << filename << " , block: " << ID << " is writing back!" << endl;
#endif
		fp.seekg(Block_Size*ID, ios::beg);
		fp.write(content, Block_Size);
		fp.close();
		dirty = false;
		// the data is written back, reset dirty bit
		return *this;
	}

	void Block_t::clear() {
		busy = dirty = false;
		filename = "";
		ID = 0;
	}

	Block_t& Block_t::operator=(const Block_t &b) {
		filename = b.filename;
		busy = b.busy;
		dirty = b.dirty;
		ID = b.ID;
		memmove(content, b.content, Block_Size);
		return *this;
	}

	Block_t& Block_t::reset(const string& f, size_t id) {
		busy = dirty = false;
		filename = f;
		ID = id;
		return *this;
	}

	bool BufferManager::NewFile(const string& filename) {
		ofstream fp(filename, ofstream::trunc);
		if (!fp) {
			fp.close();
			return false;
		}
		fp.close();
		return true;
	}

	bool BufferManager::DelFile(const string &filename) {
		for (auto &iter : BlockList) {
			if (iter->filename == filename) {
				block_index.erase(filename + std::to_string(iter->ID));
				iter->clear();
				// erase hash entry
			}
		}
		if (std::remove(filename.data()) != 0) {
			cerr << "Remove file " << filename << " failed!" << endl;
			return false;
		}
		return true;
	}

	size_t BufferManager::TotalBlocks(const string &filename) {
		ifstream fp(filename, ifstream::ate | ifstream::binary);
		auto res = fp.tellg() / Block_Size;
		fp.close();
		return res;
	}

	bool BufferManager::set_busy(const string& filename, size_t ID) {
		if (ID >= TotalBlocks(filename))
			return false;
		auto iter = block_index.find(filename + std::to_string(ID));
		if (iter == block_index.end())
			return false;
		deref(iter->second).busy = true;
		return true;
	}

	bool BufferManager::set_free(const string &filename, size_t ID) {
		if (ID >= TotalBlocks(filename))
			return false;
		auto iter = block_index.find(filename + std::to_string(ID));
		if (iter == block_index.end())
			return false;
		deref(iter->second).busy = false;
		return true;
	}

	bool BufferManager::set_dirty(const string& filename, size_t ID) {
		if (ID >= TotalBlocks(filename))
			return false;
		auto iter = block_index.find(filename + std::to_string(ID));
		if (iter == block_index.end())
			return false;
		deref(iter->second).dirty = true;
		return true;
	}


	BYTE* BufferManager::GetBlock(const string &filename, size_t ID, bool newBlock) {
		auto iter = block_index.find(filename + std::to_string(ID));
		if (iter != block_index.end()) {
			// The requesting block is already in memory
			auto block = *(iter->second);
			BlockList.erase(iter->second);
			BlockList.push_front(block);
			iter->second = BlockList.begin();
			// move the block to the head of the list
			return block->content;
		}

		bool file_resize = false;
		if (ID >= TotalBlocks(filename)) {
			if (ID == TotalBlocks(filename) && newBlock == true) {
				file_resize = true;
			}
			else {
				cerr << "Request a invalid block!" << endl;
				return nullptr;
			}
		}
		else if (newBlock == true) {
			cerr << "The block requested is already allocated!" << endl;
			return nullptr;
		}

		if (BlockList.size() < Max_Blocks) {
			// There is still room for block
			BlockList.push_front(new Block_t(filename, ID));
		}
		else {
			// need to replace one block
			for (auto rit = BlockList.rbegin(); rit != BlockList.rend(); ++rit) {
				if (deref(rit).busy == false) {
					// find the least recently used block, the block should not be busy
					block_index.erase(deref(rit).filename + std::to_string(deref(rit).ID));
					// remove hash entry
					if (deref(rit).dirty) {
						// if the block is dirty, write back the data first
						deref(rit).write_back();
					}
					deref(rit).reset(filename, ID);
					// reset the block
					auto block = *(rit);
					BlockList.erase(std::next(rit).base());
					BlockList.push_front(block);
					break;
					// find the first satisfied one and break
				}
			}
		}
		if (file_resize == true) {
			deref(BlockList.begin()).write_back();
			// resize the file
		}
		else {
			// read from file
			ifstream fp(filename, ifstream::binary);
			if (!fp) {
				cerr << "Open " << filename << "failed!" << endl;
				return nullptr;
			}
			fp.seekg(ID * Block_Size, fstream::beg);
			fp.read(deref(BlockList.begin()).content, Block_Size);
			fp.close();
		}
		block_index.insert({ filename + std::to_string(ID), BlockList.begin() });
		// create hash entry
		return deref(BlockList.begin()).content;
	}

	void BufferManager::write_all_back() {
		for (auto &iter : BlockList) {
			if (iter->dirty == true)
				iter->write_back();
		}
	}

	BufferManager::~BufferManager() {
		for (auto &it : BlockList) {
			delete it;
		}
	}

#ifdef DEBUG_MODE 
	void BufferManager::BlockList_layout() {
		for (auto &iter : BlockList) {
			cout << "| " << iter->filename << ", " << iter->ID << " |";
		}
		cout << endl;
	}
#endif
};