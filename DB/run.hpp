#pragma once

#include <stdio.h>
#include <cstdint>
#include <vector>
using namespace std;

template <typename K, typename V>
struct KVPair
{
    const K key;
    V value;

    bool operator==(KVPair kv) const
    {
        return ((key == kv.key) && (value == kv.value));
    }
    bool operator!=(KVPair kv) const
    {
        return ((key != kv.key) || (value != kv.value));
    }

    bool operator<(KVPair kv) const
    {
        return (key < kv.key);
    }

    bool operator>(KVPair kv) const
    {
        return (key > kv.key);
    }
};
template <class K, typename V>
class Run
{
public:
    virtual ~Run();
    virtual K get_min() = 0;
    virtual K get_max() = 0;
    virtual void insert_key(const K &key, const V &value) = 0;
    virtual void delete_key(const K &key) = 0;
    virtual V search_key(const K &key, bool &found) = 0;
    virtual unsigned long long num_elements() = 0;
    virtual void set_size(const unsigned long size) = 0;
    virtual vector<KVPair<K, V>> get_all() = 0;
    virtual vector<KVPair<K, V>> get_all_in_arnge(const key &key1, const key &key2) = 0;
    virtual void dump_file();
    virtual void load_file();
};
