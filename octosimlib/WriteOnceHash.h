/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
/*
 * Simple hash table, checking whether a specific 
 * unsigned long long hash already be seen, and storing an ull value.
 * We assume that the value 0 means absence of value.
 */
class WriteOnceHash
{
public:
    WriteOnceHash();
    ~WriteOnceHash();

    bool Resize(unsigned tableSize);
    bool Insert(unsigned long long key, unsigned long long value);
    bool Retrieve(unsigned long long key, unsigned long long * value);

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
    unsigned long long * valueTable;

    void Clear();
    bool DoInsert(unsigned long long key, unsigned long long value);
};

