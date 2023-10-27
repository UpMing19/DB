#include "skiplist.hpp"


int main()
{
 SkipList<int, std::string> skipList(60);
	skipList.insert_element(1, "DaMing"); 
	skipList.insert_element(3, "Lin"); 
	skipList.insert_element(7, "Ding"); 
	skipList.insert_element(8, "Shane"); 
	skipList.insert_element(9, "Cheriy"); 
	skipList.insert_element(19, "Biliy"); 
	skipList.insert_element(19, "Lu"); 

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // // skipList.load_file();

    // skipList.search_element(9);
    // skipList.search_element(18);


    // skipList.display_list();

     skipList.delete_element(3);
     skipList.delete_element(7);

    // std::cout << "skipList size:" << skipList.size() << std::endl;

    // skipList.display_list();

    return 0;
}