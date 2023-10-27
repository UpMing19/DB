#include "skiplist.cpp"

#define FILE_PATH "./store/dumpFile.txt"

int main()
{

    SkipList<int, std::string> skipList(100);

    skipList.insert_element(1, "A");
    skipList.insert_element(3, "AB");
    skipList.insert_element(5, "C");
    skipList.insert_element(7, "D");
    skipList.insert_element(19, "ADD");
    skipList.insert_element(13, "ADB");
    skipList.insert_element(15, "D");
    skipList.insert_element(7, "DDD");

    skipList.display_list();



    return 0;
}