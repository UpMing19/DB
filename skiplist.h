
/********************************************************************************
* @author: MingYu Wang
* @email: M.Wang56@newcastle.ac.uk
* @date: 2023/10/26 17:48
* @description: 
********************************************************************************/

#ifndef SKIPLIST_CPP_SKIPLIST_H
#define SKIPLIST_CPP_SKIPLIST_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define  STORE_FILE "store/dumpFile"


std::mutex mtx;
std::string delimiter = ":";

template<typename K,typename V>
class Node{

public:

    Node();
    Node(K k,V v,int level);
    ~Node();
    K get_key() const;
    V get_value() const ;
    void set_value(V v);

    Node<K,V> **forward;
    int node_level;
private:
    K key;
    V value;
};

template<typename K,typename V>
class SkipList {

public:
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K,V>* create_node(K ,V ,int);
    int insert_element(K,V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();
private:
    bool is_valid_string(const std::string &str);
    void get_key_value_from_string(const std::string &str,std::string *key,std::string *value);

private:
    int _max_level;
    int _skip_list_level;

    Node<K,V> *_header;

    std::ofstream _file_writer;
    std::ifstream _file_reader;

    int _element_count;

};


#endif //SKIPLIST_CPP_SKIPLIST_H
