#include <iostream>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <unordered_map>
#include <memory>
#include <chrono>
#include "LevelDBManager.hpp"


// Open a LevelDB database
bool LevelDBManager::open(const std::string& db_path) {
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::DB* db_ptr;
    leveldb::Status status = leveldb::DB::Open(options, db_path, &db_ptr);
    if (!status.ok()) {
        std::cerr << "Error opening database: " << status.ToString() << std::endl;
        return false;
    }

    db = std::shared_ptr<leveldb::DB>(db_ptr);
    return true;
}

// Put a key-value pair into the database
bool LevelDBManager::put(const std::string& key, const std::string& value) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return false;
    }

    leveldb::Status status = db->Put(leveldb::WriteOptions(), key, value);
    if (!status.ok()) {
        std::cerr << "Error putting key-value pair: " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

// Delete a key-value pair from the database
bool LevelDBManager::del(const std::string& key) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return false;
    }

    leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
    if (!status.ok()) {
        std::cerr << "Error deleting key: " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

// Create and store a snapshot based on timestamp
void LevelDBManager::createSnapshot(const std::string& snapshot_name) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    // snapshots[snapshot_name] = std::make_shared<leveldb::Snapshot>(db->GetSnapshot());
    snapshots[snapshot_name] = db->GetSnapshot();
}

bool LevelDBManager::getSnapshotNames(std::vector<std::string>& snapshot_names) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return false;
    }

    auto it = snapshots.begin();
    while (it != snapshots.end())
    {
        snapshot_names.emplace_back(it->first);
        it++;
    }
    return true;
}

// Get the value associated with a key from a specific snapshot
bool LevelDBManager::getFromSnapshot(const std::string& key, const std::string& snapshot_name, std::string& value) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return false;
    }

    auto it = snapshots.find(snapshot_name);
    if (it == snapshots.end()) {
        std::cerr << "Snapshot '" << snapshot_name << "' does not exist." << std::endl;
        return false;
    }

    leveldb::ReadOptions options;
    // options.snapshot = it->second.get();
    options.snapshot = it->second;

    leveldb::Status status = db->Get(options, key, &value);
    if (!status.ok()) {
        std::cerr << "Error getting value for key from snapshot: " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

// Delete all snapshots
void LevelDBManager::deleteAllSnapshot() {
    std::cout << "Deleting all snapshots..." << std::endl;

    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    auto it = snapshots.begin();
    if (it == snapshots.end()) {
        deleteSnapshot(it->first);
    }
}

// Delete a specific snapshot
void LevelDBManager::deleteSnapshot(const std::string& snapshot_name) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    auto it = snapshots.find(snapshot_name);
    if (it == snapshots.end()) {
        std::cerr << "Snapshot '" << snapshot_name << "' does not exist." << std::endl;
        return;
    }

    //db->ReleaseSnapshot(it->second.get());
    db->ReleaseSnapshot(it->second);
    snapshots.erase(snapshot_name);
}

// Put random key-value pairs into the database
void LevelDBManager::putRandomKeyValuePairs(int num_pairs) {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    leveldb::WriteBatch batch;
    for (int i = 0; i < num_pairs; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        batch.Put(key, value);
    }

    leveldb::Status status = db->Write(leveldb::WriteOptions(), &batch);
    if (!status.ok()) {
        std::cerr << "Error putting random key-value pairs: " << status.ToString() << std::endl;
    }
}

// Get number of total keys
uint64_t LevelDBManager::getNumKeys() {
    if (!db) {
        std::cerr << "Database is not open." << std::endl;
        return 0;
    }

    leveldb::ReadOptions options;
    options.fill_cache = false;
    leveldb::Iterator* it = db->NewIterator(options);
    it->SeekToFirst();
    uint64_t count = 0;
    for (; it->Valid(); it->Next()) {
        ++count;
    }
    delete it;
    return count;
}

// Close the database
void LevelDBManager::close() {
    std::cout << "Closing database..." << std::endl;
    deleteAllSnapshot();
    db.reset();
    snapshots.clear();
}
