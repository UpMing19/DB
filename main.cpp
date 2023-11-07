#include <stdio.h>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <random>
#include <algorithm>
#include "./DB/skipList.hpp"
#include "./DB/bloom.hpp"
#include "./DB/hashMap.hpp"
#include "./DB/lsm.hpp"
#include "./DB/MurmurHash.h"



using namespace std;



struct timespec start, finish;
double elapsed;

struct LSMParams {
    const int num_inserts;
    const int num_runs;
    const int elts_per_run;
    const double bf_fp;
    const int pageSize;
    const int disk_runs_per_level;
    const double merge_fraction;
};

void insertLookupTest(){
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distribution(INT32_MIN, INT32_MAX);
//    std::normal_distribution<double>  distribution(0, 10000000);

    
    const int num_inserts = 1000000;
    const int num_runs = 20;
    const int buffer_capacity = 800;
    const double bf_fp = .001;
    const int pageSize = 512;
    const int disk_runs_per_level = 20;
    const double merge_fraction = 1;
    LSM<int32_t, int32_t> lsmTree = LSM<int32_t, int32_t>(buffer_capacity, num_runs,merge_fraction, bf_fp, pageSize, disk_runs_per_level);
    
    std::vector<int> to_insert;
    for (int i = 0; i < num_inserts; i++) {
        int insert = static_cast<int>(distribution(generator));
        to_insert.push_back(insert);
    }
//    shuffle(to_insert.begin(), to_insert.end(), generator);

    std::cout << "Starting inserts" << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_inserts; i++) {
        if ( i % 100000 == 0 ) cout << "insert " << i << endl;
        lsmTree.insert_key(to_insert[i],i);
//        lsmTree.printElts();
        
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double total_insert = (finish.tv_sec - start.tv_sec);
    total_insert += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    std::cout << "Time: " << total_insert << " s" << std::endl;
    std::cout << "Inserts per second: " << (int) num_inserts / total_insert << " s" << std::endl;
    

    std::cout << "Starting lookups" << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int lookup;
    for (int i = 0 ; i < num_inserts; i++) {
        if ( i % 100000 == 0 ) cout << "lookup " << i << endl;

        lsmTree.lookup(to_insert[i], lookup);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double total_lookup = (finish.tv_sec - start.tv_sec);
    total_lookup += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    std::cout << "Time: " << total_lookup << " s" << std::endl;
    std::cout << "Lookups per second: " << (int) num_inserts / total_lookup << " s" << std::endl;
}

int main()
{
 
   insertLookupTest();

    return 0;
}