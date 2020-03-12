#ifndef __DB_H
#define __DB_H

#include "BufferManager.hpp"
#include "Catalog_manager.h"
#include "record_manager.h"
#include "IndexManager.h"
using namespace MiniSQL;

extern BufferManager *bm;
extern IndexManager *im;
extern Catalog_manager *cm;
extern RecordManager *rm;

class DB{
    public:
    DB(){
        bm = new BufferManager;
        im = new IndexManager;
        cm = new Catalog_manager;
        rm = new RecordManager;
    }

    void run(){

    }
};

#endif