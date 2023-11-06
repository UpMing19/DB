#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <cstring>
#include "run.hpp"
#include "diskRun.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cassert>
#include <algorithm>

#define LEFTCHILD(x) x * 2 + 1
#define RIGHTCHILD(x) x * 2 + 2
#define PARENT(x) (x - 1) / 2

using namespace std;

int TOMBSTONE = INT_MIN;

template <class K, class V>
class DiskLevel
{

public:
    typedef KVPair<K, V> KVPair_t;
    typedef pair<KVPair<K, V>, int> KVIntPair_t;
    KVPair_t KVPAIRMAX;
    KVIntPair_t KVINTPAIRMAX;
    V V_TOMBSTONE = (V)TOMBSTONE;

    struct StaticHeap
    {
        int size;
        vector<KVIntPair_t> arr;
        KVIntPair_t max;

        StaticHeap(unsigned sz, KVIntPair_t mx)
        {
            size = 0;
            max = mx;
            arr = vector<KVIntPair_t>(sz, mx)
        }
        void push(KVIntPair_t blob)
        {
            unsigned i = size++;
            while (i && blob < arr[PARENT(i)])
            {
                arr[i] = arr[PARENT(i)];
                i = PARENT(i);
            }
            arr[i] = blob;
        }

        void heapify(int i)
        {
            int smallest = (LEFTCHILD(i) < size && arr[LEFTCHILD(i)] < arr[i]) ? LEFTCHILD(i) : i;
            if (RIGHTCHILD(i) < size && arr[RIGHTCHILD(i)] < arr[smallest])
                smallest = RIGHTCHILD(i);
            if (smallest == i)
                return;
            KVIntPair_t temp;
            temp = arr[i];
            arr[i] = arr[smallest];
            arr[smallest] = temp;
            heapify(smallest);
        }

        KVIntPair_t pop()
        {
            KVIntPair_t ret = arr[0];
            arr[0] = arr[--size];
            heapify(0);
            return ret;
        }
    };
 
    int _level;
    unsigned _pageSize;
    unsigned long _runSize;
    unsigned _numRuns;
    unsigned _activeRun;
    unsigned _mergeSize;
    double _bf_fo;
    vector<DiskRun<K,V> *> runs;

};