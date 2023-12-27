#include <iostream>
#include <cassert>
#include "leveldb/db.h"

using namespace std;

static string db_path = "/tmp/testdb";

void check_status(leveldb::Status status) {
    if (!status.ok()) {
        cout<<"Databse error: "<<status.ToString()<<endl;
        assert(status.ok());
    }
}

void leveldb_demo1() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, db_path, &db);

    check_status(status);
    status = db->Put(leveldb::WriteOptions(), "name", "hunk");
    check_status(status);
    status = db->Put(leveldb::WriteOptions(), "site", "chengdu");
    check_status(status);

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
    }
    check_status(it->status());  // Check for any errors found during the scan
    delete it;

    leveldb::ReadOptions ss_options;
    ss_options.snapshot = db->GetSnapshot();

    // Put new value after the snapshot
    status = db->Put(leveldb::WriteOptions(), "site", "beijing");
    check_status(status);

    std::string value1, value2;
    status = db->Get(leveldb::ReadOptions(), "site", &value1);
    check_status(status);
    status = db->Get(ss_options, "site", &value2);
    check_status(status);

    cout<<"Got value from database: "<<value1<<endl; //beijing
    cout<<"Got value from snapshot: "<<value2<<endl; //chengdu
    
    db->ReleaseSnapshot(ss_options.snapshot);
    delete db;
}


int main() {
    cout<<"[BEGIN] testing..."<<endl;
    leveldb_demo1();
    cout<<"[END] testing..."<<endl;
}
