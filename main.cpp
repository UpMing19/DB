#include "skiplist.h"

#define FILE_PATH "./store/dumpFile.txt"

int main()
{

    SkipList<int, std::string> skipList(6);

    skipList.insert_element(1, "A");
    skipList.insert_element(3, "AB");
    skipList.insert_element(5, "C");
    skipList.insert_element(7, "D");
    skipList.insert_element(19, "A");
    skipList.insert_element(13, "AB");
    skipList.insert_element(15, "C");
    skipList.insert_element(7, "D");

    skipList.display_list();



    return 0;
}