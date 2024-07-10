#ifndef LEVELDB_MANAGER_H_
#define LEVELDB_MANAGER_H_

#include <iostream>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <vector>

class LevelDBManager {
private:
    std::shared_ptr<leveldb::DB> db;
    //std::unordered_map<std::string, std::shared_ptr<leveldb::Snapshot>> snapshots;
    std::unordered_map<std::string, const leveldb::Snapshot*> snapshots;

public:
    LevelDBManager() {}

    ~LevelDBManager() {
        close();
    }

    // Open a LevelDB database
    bool open(const std::string& db_path);
    // Put a key-value pair into the database
    bool put(const std::string& key, const std::string& value);
    bool del(const std::string& key);

    // Create and store a snapshot based on timestamp
    void createSnapshot(const std::string& snapshot_name);
    // Get all snapshots
    bool getSnapshotNames(std::vector<std::string>& snapshot_names);
    // Get the value associated with a key from a specific snapshot
    bool getFromSnapshot(const std::string& key, const std::string& snapshot_name, std::string& value);
    void deleteAllSnapshot();
    // Delete a specific snapshot
    void deleteSnapshot(const std::string& snapshot_name);
    // Put random key-value pairs into the database
    void putRandomKeyValuePairs(int num_pairs);

    uint64_t getNumKeys();
    // Close the database
    void close();
};

#endif