# DB Server
The Skiplist-Based LSM Tree

#可以优化的地方

 1.内存泄漏 （可以封装智能指针）
 2.为长汉字的时候
 3.可以加分布式
 4.日志系统代替cout输出
    a.freebuf and flushbuf 向文件中写的时候使用队列，pop出来的放入free，然后在析构函数中释放内存
    b.在析构函数中使用粒度较小的锁（每个数据结构都使用一个锁） 能够减少冲突，提高并发





#LOG TEST
    g++ -g -Wall -O2 -pthread -std=c++11 test/log_test.cpp log/log.cpp -o bin/logtest
#SkipList TEST
    g++ -g -Wall -O2 -pthread -std=c++11 test/skiplist_test.cpp DB/skiplist.hpp  log/log.cpp -o bin/skiplist

1. 持久化模块:
使你的数据在系统重启后依然可以恢复。
可以考虑使用WAL（Write-Ahead Logging）或快照（Snapshot）等机制。
2. 缓存模块:
添加LRU缓存或其他缓存策略以提高读取速度。
用于存储热点数据，减少直接从磁盘读取的次数。
3. 并发控制:
使用读写锁、乐观锁等机制来处理并发读写。
可以提高在多线程环境中的性能。
4. 网络模块:
提供网络接口，使得可以远程访问和操作KV存储。
可以使用gRPC, RESTful API, 或者自定义的TCP/UDP协议。
5. 数据压缩:
数据在写入磁盘之前可以进行压缩，读取时再解压。
可以使用现成的压缩库如zlib。
6. 监控和指标:
收集运行时信息，如读写延迟、错误率等。
有助于分析系统性能和定位问题。
7. 自动化测试和Benchmark:
编写测试用例确保系统的稳定性和正确性。
编写Benchmark测试性能。
8. 错误处理和恢复:
添加更多的错误检查和异常处理。
实现自我修复的机制来提高系统的鲁棒性。
9. 事务支持:
提供ACID特性。
支持多步操作的原子性。
10. 安全性:


/*

*/
