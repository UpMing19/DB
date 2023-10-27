
/********************************************************************************
 * @author: MingYu Wang
 * @email: M.Wang56@newcastle.ac.uk
 * @date: 2023/10/26 17:48
 * @description:
 ********************************************************************************/


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


template<typename K, typename V>
Node<K, V>::Node() {
}

template<typename K, typename V>
Node<K, V>::Node(K k, V v, int level) {
    this->value = v;
    this->key = k;
    this->node_level = level;
    this->forward = new Node<K, V> *[level + 1];
    memset(this->forward, 0, sizeof(Node<K, V>) * (level + 1));
}

template<typename K, typename V>
Node<K, V>::~Node() {
    delete[] forward;
}

template<typename K, typename V>
void Node<K, V>::set_value(V v) {
    this->value = v;
}

template<typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
}

template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level) {
    this->_max_level = max_level;
    this->_element_count = 0;
    this->_skip_list_level = 0;
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
    if (_file_reader.is_open())
        _file_reader.close();
    if (_file_writer.is_open())
        _file_writer.close();
    delete _header;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int level = 1;
    // TODO 了解这个公式背后的数学原理
    double ZSKIPLIST_P = 0.25;
    int ZSKIPLIST_MAXLEVEL = _max_level;
    while ((random() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}
// template<typename K, typename V>
// int SkipList<K, V>::get_random_level(){

//     int k = 1;
//     while (rand() % 2) {
//         k++;
//     }
//     k = (k < _max_level) ? k : _max_level;
//     return k;
// }
template<typename K, typename V>
int SkipList<K, V>::size() {
    return _element_count;
}

template<typename K, typename V>
Node<K, V> *SkipList<K, V>::create_node(K k, V v, int level) {
    Node<K, V> *node = new Node<K, V>(k, v, level);
    return node;
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(K k, V v) {

    mtx.lock();

    Node<K, V> *current = this->_header;

    Node<K, V> *update[_max_level + 1];
    memset(update, 0, sizeof(Node<K, V> *) * (_max_level + 1));

    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->get_key() < k)
            current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0]; // will replace this node

    if (current != NULL && current->get_key() == k) {
        std::cout << "Key:" << k << " has existed." << std::endl;
        mtx.unlock();
        return 1;
    }

    if (current == NULL || current->get_key() != k) {
        int random_level = get_random_level();
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level + 1; i <= random_level; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }
        Node<K, V> *new_node = create_node(k, v, random_level);

        for (int i = 0; i <= random_level; i++) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }
        std::cout << "Successful insert Key:" << k << " Value: " << v << std::endl;
        _element_count++;
    }
    mtx.unlock();
    return 0;
}

template<typename K, typename V>
void SkipList<K, V>::display_list() {
    std::cout << "******** SkipList Show ********" << std::endl;
    for (int i = 0; i <= _skip_list_level; i++) {
        Node<K, V> *node = this->_header->forward[i];
        std::cout << "Level " << i << " : ";

        while (node != NULL) {
            std::cout << "Key : " << node->get_key() << " , Value : " << node->get_value() << ".  ";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template<typename K, typename V>
void SkipList<K, V>::dump_file() {
    std::cout << "********DumpFile********" << std::endl;
    _file_writer.open(STORE_FILE);

    for (int i = 0; i <= 0; i++) {
        Node<K, V> *node = this->_header->forward[i];

        while (node != NULL) {
            _file_writer << "Key:" << node->get_key() << " , Value : " << node->get_value() << ".  \n";
            std::cout << "Key : " << node->get_key() << " , Value : " << node->get_value() << ".  " << std::endl;
            node = node->forward[i];
        }
    }
    _file_writer.flush();
    _file_writer.close();
}

template<typename K, typename V>
void SkipList<K, V>::load_file() {
    std::cout << "********LoadFile********" << std::endl;
    _file_reader.open(STORE_FILE);
    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();

    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(stoi(*key), *value);
        std::cout << "load Key=" << *key << "  and  Value = " << *value << " ." << std::endl;
    }
    delete key;
    delete value;
    _file_reader.close();
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string &str, std::string *key, std::string *value) {
    if (!is_valid_string(str)) return;
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter) + 1, str.size());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string &str) {
    if (str.empty()) return false;
    if (str.find(delimiter) == std::string::npos) return false;
    return true;
}

template<typename K, typename V>
void SkipList<K, V>::delete_element(K k) {
    mtx.lock();
    Node<K, V> *update[_max_level + 1];
    Node<K, V> *current = this->_header;

    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->get_key() < k)
            current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0];
    if (current == NULL || current->get_key() != k) {
        std::cout << "Key not exist" << std::endl;
        mtx.unlock();
        return ;
    }

    for(int i =0 ;i<=_skip_list_level; i++) {
        if(update[i]->forward[0] != current)
            break;
        update[i]->forward[i] = current->forward[i];
    }
    while(_skip_list_level>0 && _header->forward[_skip_list_level]==0)
        _skip_list_level--;
    delete current;
    _element_count--;

    std::cout << "Delete Successful!!!" << std::endl;
    mtx.unlock();
    return ;
}

template<typename K, typename V>
bool SkipList<K,V>::search_element(K k) {

    Node<K, V> *current = this->_header;

    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->get_key() < k)
            current = current->forward[i];
        if(current->get_key()==k)
           return true;
    }
    current = current->forward[0];
    if(current==NULL) return false;
    if(current->get_key()==k)
    {
        std::cout << "Key : " << current->get_key() << " , Value : " << current->get_value() << ".  " << std::endl;
        return true;
    }
    std::cout << "Not Found!!!" << std::endl;
    return false;
}
