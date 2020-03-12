#ifndef __BUFFER_MANAGER_H
#define __BUFFER_MANAGER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <list>
#include <cstring>
using std::string;
using std::list;
using std::unordered_map;

#define deref(p) (*(*(p)))

namespace MiniSQL{

    constexpr int Block_Size = 4096;
    // 4 KB
    #ifdef DEBUG_MODE
        constexpr int Max_Blocks = 3;
        // maximum number of blocks in memory
    #else
        constexpr int  Max_Blocks = 128;
    #endif
    using BYTE = char;

    struct Block_t{
        string filename;
        // specify which file this block belong to
        bool busy;
        // indicate whether the block is in use.
        bool dirty;
        // indicate whether the block is changed 
        size_t ID;
        // indicate the location. For example:
        // ID=5 means the entry of the block is 5*Block_Size offsets to 
        // the begin of the file.
        BYTE content[Block_Size];
        // content

        Block_t(const string& f, size_t id)
            :filename(f), busy(false), dirty(false), ID(id)
            {
                memset(content, 0, Block_Size);
            }
        ~Block_t()=default;
        void clear();
        Block_t &write_back();
        Block_t& operator=(const Block_t& b);
        Block_t& reset(const string& filename, size_t ID);
    };

    class BufferManager{
        public:
            BufferManager()=default;
            ~BufferManager();
            bool NewFile(const string&);
            // create new file
            bool DelFile(const string&);
            // delete a file
            BYTE* GetBlock(const string &filename, size_t ID, bool newBlock=false);
            // get a block by filename and ID
            size_t TotalBlocks(const string& filename);
            // get total number of blocks from a file
            bool set_busy(const string& filename, size_t ID);
            // set a block busy, and buffermanager will not 
            // replace this block until you set it free.
            // return true if operation succeeded
            bool set_free(const string& filename, size_t ID);
            // set a block free
            // return true if operations succeeded
            bool set_dirty(const string& filename, size_t ID);
            // if you write something into the block, remember 
            // to set it dirty
            // return true if operations succeeded
            void write_all_back();
            // force all dirty blocks to write contents back to disk
            #ifdef DEBUG_MODE
            void BlockList_layout();
            #endif

        private:
            using BlockMap_t = unordered_map<string, list<Block_t*>::iterator>;
            list<Block_t*> BlockList;
            // linked-list for storing blocks in memory
            BlockMap_t block_index;
            // hash map for accessing block in constant time
            bool validate_block(const string &filename, size_t ID);
    };

};

#endif