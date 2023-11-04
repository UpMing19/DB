#pragma once

#include "MurmurHash.h"
#include "run.hpp"
#include <stdlib.h>
#include <cstdint>
#include <climits>

template <typename K, typename V>
class HashTable
{
private:
    KVPair<K, V> *table;

public:
    unsigned long _size;
    unsigned long _elts;
    HashTable(unsigned long size) : _size(size * 2), _elts(0)
    {
        table = new KVPair<K, V>[_size]();
        fill(table, table + _size, (KVPair<K, V>)empty);
    }
    ~HashTable()
    {
        delete[] table;
    }
    unsigned long hashFunc(const K key)
    {
        array<unsigned long, 2> hashValue;
        MurmurHash3_x64_128(&key, sizeof(K), 0, hashValue);
        return (hashValue[0] % _size);
    }
    void resize()
    {
        _size *= 2;
        auto newTable = new KVPair<K, V>[_size]();
        fill(newTable, newTable + _size, (KVPair<K, V>)empty);
        for (unsigned long i = 0; i < _size / 2; i++)
        {
            if (table[i] != empty)
            {
                unsigned long newHash = hashFunc(table[i].key);
                for (int j = 0;; j++)
                {
                    if (newTable[(newHash + j) % _size] == empty)
                    {
                        newTable[(newHash + j) % _size] = table[i];
                        break;
                    }
                }
            }
        }
        delete[] table;
        table = newTable;
    }
    bool get(const K &key, V &value)
    {
        unsigned long hashValue = hashFunc(key);
        for (int i = 0;; i++)
        {
            if (table[(i + hashValue) % _size] == empty)
                return false;
            else if (table[(i + hashValue) % _size].key == key && table[(i + hashValue) % +_size].value == value)
                return true;
        }
    }
    void put(const K &key, V &value)
    {
        if (_elts * 2 > _size)
        {
            resize();
        }
        unsigned long hashValue = hashFunc(key);
        for (unsigned long i = 0;; i++)
        {
            if (table[(i + hashValue) % _size] == empty)
            {
                table[(i + hashValue) % _size] = {key, value};
                _elts++;
                return;
            }
            else if (table[(i + hashValue) % _size].key == key)
            {
                table[(i + hashValue) % +_size].value = value;
                return;
            }
        }
    }

    V putIfEmpty(const K &key, const V &value)
    {
        if (_elts * 2 > _size)
        {
            resize();
        }
        unsigned long hashValue = hashFunc(key);
        for (unsigned long i = 0;; i++)
        {
            if (table[(i + hashValue) % _size] == empty)
            {
                table[(i + hashValue) % _size] = {key, value};
                _elts++;
                return (V)NULL;
            }
            else if (table[(i + hashValue) % _size].key == key)
            {
                return table[(i + hashValue) % +_size].value;
            }
        }
    }
};
