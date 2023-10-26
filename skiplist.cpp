//
// Created by c3009023 on 2023/10/26.
//

#include "skiplist.h"

template<typename K,typename V>
Node<K,V>::Node() {

}

template<typename K,typename V>
Node<K,V>::Node(K k, V v, int level) {
    this->value = v;
    this->key = k;
    this->node_level = level;
    this->forward = new Node<K,V>* [level+1];
    memset(this->forward,0,sizeof(Node<K,V>) *(level+1));
}

template<typename K,typename V>
Node<K,V>::~Node() {
    delete[] forward;
}

template<typename K,typename V>
void Node<K, V>::set_value(V v) {
    this->value = v;
}

template<typename K,typename V>
V Node<K,V>::get_value(){
    return value;
}

template<typename K,typename V>
K Node<K, V>::get_key() const {
    return key;
}




template<typename K,typename V>
SkipList<K,V>::SkipList(int max_level) {
    this->_max_level = max_level;
    this->_element_count =0 ;
    this->_skip_list_level =0 ;
    K k;
    V v;
    this->header = new Node<K,V>(k,v,_max_level);
}

template<typename K,typename V>
SkipList<K,V>::~SkipList() {
    if(_file_reader.is_open())
        _file_reader.close();
    if(_file_writer.is_open())
        _file_writer.close();
    delete header;
}
template<typename K,typename V>
int SkipList<K, V>::get_random_level() {
    int level = 1;
    // TODO 了解这个公式背后的数学原理
    double ZSKIPLIST_P = 0.25;
    int ZSKIPLIST_MAXLEVEL = _max_level;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}
template<typename K,typename V>
int SkipList<K, V>::size() {
    return _element_count;
}

template<typename K,typename V>
Node<K, V> *SkipList<K, V>::create_node(K k, V v, int level) {
    Node<K,V> *node = new Node<K,V>(k,v,level);
    return node;
}

template<typename K,typename V>
int SkipList<K, V>::insert_element(K k, V v) {

    mtx.lock();


}