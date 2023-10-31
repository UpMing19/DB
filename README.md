# SkipList-Cpp
SkipList

#可以优化的地方

 1.内存泄漏 （可以封装智能指针）
 2.为长汉字的时候
 3.可以加分布式
 4.日志系统代替cout输出
    a.freebuf and flushbuf 向文件中写的时候使用队列，pop出来的放入free，然后在析构函数中释放内存
    b.在析构函数中使用粒度较小的锁（每个数据结构都使用一个锁） 能够减少冲突，提高并发





#LOG TEST
    g++ -g -Wall -O2 -pthread -std=c++11 test/log_test.cpp log/log.cpp -o bin/logtest