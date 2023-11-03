#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "../log/log.h"
#include "../DB/skiplist.h"

int main(int argc, char **argv)
{

    LOG_INIT("./store", LogLevel::DEBUG);
    SkipList<int, std::string> sk(6);
    sk.insert_element(1, "D");
    sk.insert_element(11, "D123");
    sk.insert_element(13, "D12q3");
    sk.insert_element(14, "D12qr3");
    sk.insert_element(1, "Dqwrq");
    sk.insert_element(121, "wqrqw");
    sk.insert_element(123, "a");
    sk.insert_element(314, "asg");
    sk.display_list();
    sk.dump_file();

}