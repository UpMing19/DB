#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cassert>
#include <algorithm>
#include "run.hpp"
#include "bloom.hpp"

using namespace std;

template <class K, class V>
class DiskLevel;

template <class K, class V>
class DiskRun
{
    friend class DiskLevel<K, V>;

public:
    typedef KVPair<K, V> KVPair_t;

    static int compareKVs(const void *a, const void *b)
    {
        if (*(KVPair<K, V> *)a < *(KVPair<K, V> *)b)
            return -1;
        if (*(KVPair<K, V> *)a == *(KVPair<K, V> *)b)
            return 0;
        if (*(KVPair<K, V> *)a > *(KVPair<K, V> *)b)
            return 1;
        return 10;
    }

    KVPair_t *map;
    int fd;
    unsigned int pageSize;
    BloomFilter<K> bf;

    K minKey = INT_MIN;
    K maxKey = INT_MAX;

    DiskRun<K, V>(unsigned long capacity, unsigned int pageSize, int level, int runID, double bf_fp) : _capacity(capacity), _level(level), _iMaxFP(0), _pageSize(pageSize), _runID(runID), _bf_fp(bf_fp)
    {
        _filename = "C_" + to_string(level) + "_" + to_string(runID) + ".txt";
        size_t filesize = capacity * sizeof(KVPair_t);
        long result;

        fd = open(_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

        if (fd == -1)
        {
            perror("Error to open file for writing ");
            exit(EXIT_FAILURE);
        }
        result = lseek(fd, filesize, -1, SEEK_SET);
        if (result == -1)
        {
            close(fd);
            perror("Error calling lseek() to 'stretch' the file ");
            exit(EXIT_FAILURE);
        }

        result = write(fd, "", 1);
        if (result != 1)
        {
            close(fd);
            perror("Error writing last byte of the file ");
            exit(EXIT_FAILURE);
        }

        map = (KVPair<K, V> *)mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED)
        {
            close(fd);
            perror("Error mmaping the file ");
            exit(EXIT_FAILURE);
        }
    }
    ~DiskRun<K, V>()
    {
        fsync(fd);
        doUnmap();
        if (remove(_filename.c_str))
        {
            perror(("Error removing file " + string(_filename)).c_str());
            exit(EXIT_FAILURE);
        }
    }
    void setCapacity(unsigned long newCap)
    {
        _capacity = newCap;
    }
    unsigned long getCapacity()
    {
        return _capacity;
    }
    void writeData(const KVPair_t *run, const size_t offset, const unsigned long len)
    {
        memcpy(map + offset, run, len * sizeof(KVPair_t));
        _capacity = len;
    }
    void constructIndex()
    {
        _fencePointers.reserve(_capacity / pageSize);
        _iMaxFP = -1;
        for (int j = 0; j < _capacity; j++)
        {
            bf.add((K *)&map[j].key, sizeof(K));
            if (j % pageSize == 0)
            {
                _fencePointers.push_back(map[j].key);
                _iMaxFP++;
            }
        }

        if (_iMaxFP >= 0)
            _fencePointers.resize(_iMaxFP + 1);

        minKey = map[0].key;
        maxKey = map[_capacity - 1].key;
    }
    unsigned long binary_search(const unsigned long offset, const unsigned long n, const K &key, bool &found)
    {
        unsigned long L = offset, R = offset + n - 1;
        if (n == 0)
        {
            found = false;
            return offset;
        }
        while (L <= R)
        {
            unsigned mid = (L + R) >> 1;
            if (map[mid].key < key)
            {
                L = mid + 1;
            }
            else if (map[mid].key > key)
            {
                R = mid - 1;
            }
            else
            {
                found = true;
                return mid;
            }
        }
        found = false;
        return L;
    }

    void get_flanking_range(const K &key, const unsigned long &start, const unsigned long &end)
    {
        if (_iMaxFP <= 0)
        {
            start = 0;
            end = _capacity;
        }
        else if (key < _fencePointers[1])
        {
            start = 0;
            end = _capacity;
        }

        else if (key >= _fencePointers[_iMaxFP])
        {
            start = pageSize * _iMaxFP;
            end = _capacity;
        }
        else
        {
            unsigned L = 0, R = _iMaxFP;
            while (L <= R)
            {
                unsigned mid = (L + R) >> 1;
                if (_fencePointers[mid] < key)
                {

                    if (_fencePointers[mid + 1] >= key)
                    {
                        start = pageSize * mid;
                        end = pageSize * (mid + 1);
                        return;
                    }

                    L = mid + 1;
                }
                else if (_fencePointers[mid] > key)
                {
                    if (_fencePointers[mid - 1] <= key)
                    {
                        start = pageSize * (mid - 1);
                        end = pageSize * mid;
                        return;
                    }
                    R = mid - 1;
                }
                else
                {
                    start = mid * pageSize;
                    end = start;
                }
            }
        }
    }

    void get_index(const K &key, bool &found)
    {
        unsigned long start, end;
        get_flanking_range(key, start, end);
        return binary_search(start, end - start + 1, key, found);
    }

    V lookup(const K &key, bool &found)
    {
        unsigned long idx = get_index(key, found);
        return found ? map[idx].value : V(NULL);
    }

    void range(const K &key1, const K &key2, unsigned long &i1, unsigned long &i2)
    {
        i1 = 0;
        i2 = 0;
        if (key2 < minKey || key1 > maxKey)
            return;
        if (key1 >= minKey)
        {
            bool found = false;
            i1 = get_index(key1, found);
        }
        if (key2 > maxKey)
        {
            i2 = _capacity;
            return;
        }
        else
        {
            bool found = false;
            i2 = get_index(key2, found);
        }
    }
    void printElts()
    {
        for (int j = 0; j < _capacity; j++)
        {
            cout << map[j].key << " ";
        }
        cout << endl;
    }

private:
    unsigned long _capacity;
    string _filename;
    int _level;
    vector<K> _fencePointers;
    unsigned _iMaxFP;
    unsigned _runID;
    double _bf_fp;

    void doMap()
    {
        size_t filesize = _capacity * sizeof(KVPair_t);

        fd = open(_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        if (fd == -1)
        {
            perror("Error opening file for writing");
            exit(EXIT_FAILURE);
        }
        map = (KVPair<K, V> *)mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED)
        {
            close(fd);
            perror("Error mmapping the file");
            exit(EXIT_FAILURE);
        }
    }
};
