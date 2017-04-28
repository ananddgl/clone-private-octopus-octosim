#pragma once
/*
 * Simple hash table, checking whether a specific 
 * unsigned long long hash already be seen.
 * We assume that the value 0 means absence of value.
 */
class WriteOnceHash
{
public:
    WriteOnceHash();
    ~WriteOnceHash();

    bool Resize(unsigned tableSize);
    bool Insert(unsigned long long key);
    bool Retrieve(unsigned long long key);

    unsigned int GetCount() {
        return tableCount;
    }

    unsigned int GetSize() {
        return tableSize;
    }

private:
    unsigned int tableSize;
    unsigned int tableCount;
    unsigned long long * hashTable;

    void Clear();
    bool DoInsert(unsigned long long key);
};

