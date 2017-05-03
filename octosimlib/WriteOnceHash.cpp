#include <stdlib.h>
#include <string.h>
#include "WriteOnceHash.h"

WriteOnceHash::WriteOnceHash()
    :
    tableSize(0),
    tableCount(0),
    hashTable(NULL)
{
}


WriteOnceHash::~WriteOnceHash()
{
    Clear();
}

void WriteOnceHash::Clear()
{
    if (hashTable != NULL)
    {
        delete[] hashTable;
        hashTable = NULL;
    }
    tableSize = 0;
}

bool WriteOnceHash::Resize(unsigned newSize)
{
    bool ret = false;
    unsigned long long * oldTable = hashTable;
    unsigned int oldSize = tableSize;

    if (oldSize >= newSize)
    {
        ret = true;
    }
    else
    {
        unsigned long long * newTable = new unsigned long long[newSize];

        if (newTable != NULL)
        {
            hashTable = newTable;
            tableSize = newSize;
            memset(hashTable, 0, sizeof(unsigned long long)*tableSize);
            ret = true;
            tableCount = 0;

            if (oldTable != NULL)
            {
                for (unsigned int i = 0; ret && i < oldSize; i++)
                {
                    if (oldTable[i] != 0)
                    {
                        ret = DoInsert(oldTable[i]);
                    }
                }

                if (!ret)
                {
                    hashTable = oldTable;
                    tableSize = oldSize;
                    delete[] newTable;
                }
                else
                {
                    delete[] oldTable;
                }
            }
        }
    }

    return ret;
}


bool WriteOnceHash::Insert(unsigned long long key)
{
    bool ret = true;
    unsigned int newCount = tableCount+1;

    if (key == 0)
    {
        ret = false;
    }
    else if (tableSize < 2 * newCount)
    {
        unsigned int newSize = tableSize;

        if (tableSize == 0)
        {
            newSize = 128;
        }

        while (newSize < 4* newCount)
        {
            newSize *= 2;
        }

        ret = Resize(newSize);
    }

    if (ret)
    {
        ret = DoInsert(key);
    }

    return ret;
}

bool WriteOnceHash::DoInsert(unsigned long long key)
{
    bool ret = false;
    unsigned int hash_index = key%tableSize;

    for (unsigned int i = 0; i < tableSize; i++)
    {
        if (hashTable[hash_index] == 0)
        {
            hashTable[hash_index] = key;
            tableCount++;
            ret = true;
            break;
        }
        else if (hashTable[hash_index] == key)
        {
            /* found it. Do not insert it twice! */
            ret = false;
        }
        else
        {
            hash_index++;

            if (hash_index >= tableSize)
            {
                hash_index = 0;
            }
        }
    }

    return ret;
}

bool WriteOnceHash::Retrieve(unsigned long long key)
{
    bool ret = false;
    unsigned int hash_index;

    if (key != 0 && tableSize > 0)
    {
        hash_index = key%tableSize;

        for (unsigned int i = 0; i < tableSize; i++)
        {
            if (hashTable[hash_index] == 0)
            {
                /* found a hole, which means the key is not there */
                break;
            }
            else if (hashTable[hash_index] == key)
            {
                /* found it! */
                ret = true;
            }
            else
            {
                /* did not find the key here */
                hash_index++;

                if (hash_index >= tableSize)
                {
                    hash_index = 0;
                }
            }
        }
    }

    return ret;
}
