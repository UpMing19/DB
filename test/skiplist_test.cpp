#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "../log/log.h"
#include "../DB/skiplist.hpp"

int main(int argc, char **argv)
{

    LOG_INIT("./store", LogLevel::INFO);

    SkipList<int, string, 12> sp(1, 10);
    sp.insert_key(1, "DaMing");
    sp.insert_key(2, "Shane");
    sp.insert_key(3, "Ding");
    sp.insert_key(2, "DaMing2");
    sp.insert_key(8, "Oane");
    sp.insert_key(6, "Mohan");
    vector<KVPair<int, string>> ans = sp.get_all();
    for (auto &p : ans)
    {
        cout<<p.key<<":"<<p.value<<endl;
    }
    sp.dump_file();
}