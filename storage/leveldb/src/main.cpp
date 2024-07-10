#include <iostream>
#include <cassert>
#include "leveldb/db.h"
#include "LevelDBManager.hpp"

const std::string db_path = "/tmp/leveldb.test";

int db_manager_test() {
    LevelDBManager db_manager;

    // Open the LevelDB database
    if (!db_manager.open("test_db")) {
        std::cerr << "Failed to open database." << std::endl;
        return 1;
    }

    // Get number of total keys
    std::cout << "Number of total keys: " << db_manager.getNumKeys() << std::endl;

    // Put some key-value pairs
    db_manager.put("site", "beijing");
    db_manager.put("name", "hunk");

    std::string key_alias("alias");
    db_manager.put(key_alias, "before_snapshot1");
    // Create first snapshot
    db_manager.createSnapshot("snapshot1");
    // Update value
    db_manager.put(key_alias, "before_snapshot2");

    // Create second snapshot
    db_manager.createSnapshot("snapshot2");
    // Update value of key 'alias'
    db_manager.put(key_alias, "after_snapshot2");

    // Get a value for a key from first snapshot
    std::string value;
    if (db_manager.getFromSnapshot(key_alias, "snapshot1", value))
        std::cout << "Value for alias from snapshot1: " << value << std::endl;

    // Get a value for a key from second snapshot
    if (db_manager.getFromSnapshot(key_alias, "snapshot2", value))
        std::cout << "Value for alias from snapshot2: " << value << std::endl;

    // Delete the snapshot
    db_manager.deleteSnapshot("snapshot1");

    // Put random key-value pairs
    db_manager.putRandomKeyValuePairs(10 * 1000);

    // Get number of total keys
    std::cout << "Number of total keys: " << db_manager.getNumKeys() << std::endl;

    // Close the database (automatically done by destructor)
    return 0;
}


int main() {
    std::cout<<"[BEGIN] Testing LevelDBManager..."<<std::endl;
    db_manager_test();
    std::cout<<"[END] Testing LevelDBManager..."<<std::endl;
}
