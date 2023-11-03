#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "../log/log.h"

using std::cout;
using std::endl;
using std::string;

#define orderSize 12

const string all_order[orderSize] = {"set", "get", "del", "load", "dump", "display", "revoke", "clear", "size", "getfile", "sitefile","exit"};
const int order_number = orderSize;
const string delimitero = " ";

class Request{
    private:
    string request;

    public:
    inline string get_request() const {return request;};
    Request(string s){request = s;};
    bool is_valid_request();
    void split_request();
    void print_error(int error_number) const ;
    std::vector<string>arg;
    
};

void Request::split_request(){
    if(request.empty())
        return ;
    string strs = get_request() + delimitero;
    
    size_t pos = strs.find(delimitero);
    size_t size = strs.size();

    while(pos!=std::string::npos){
        string x = strs.substr(0,pos);
        arg.push_back(x);
        strs = strs.substr(pos+1,size);
        pos = strs.find(delimitero);
    }

}


// 判断请求是否有效
bool Request::is_valid_request()
{
    split_request();

    if (arg.size() == 0)
    {
        return false;
    }

    string order = arg[0];
    bool is_valid_order = false;
    for (int i = 0; i < order_number; ++i)
    {
        if (order == all_order[i])
        {
            is_valid_order = true;
            break;
        }
    }

    if (!is_valid_order)
    {
        LOG(LogLevel::ERROR,"error request");
        return false;
    }
    


    return true;
}
