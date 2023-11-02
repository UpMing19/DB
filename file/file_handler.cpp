
/********************************************************************************
 * @author: MingYu Wang
 * @email: M.Wang56@newcastle.ac.uk
 * @date: 2023/11/01 16:03
 * @description:
 ********************************************************************************/


#include "file_handler.h"


FileHandler::FileHandler()
{
}
FileHandler::~FileHandler()
{
    if (file_stream.is_open())
        file_stream.close();
}
int FileHandler::open(const std::string &path, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
{
    file_path = path;
    file_stream.open(path, mode);
    if(!file_stream.is_open()){
        return -1;
    }
    return 0;
}
int FileHandler::write(const std::string &data)
{
   if(!file_stream.is_open()){
        return -1;
    }
    file_stream << data;
    if(file_stream.fail()) 
        return -1;
    return 0;
}
int FileHandler::read(std::string *data)
{
    if (!file_stream.is_open())
        return -1;
    std::string line ;
    while(std::getline(file_stream,line)){
        *data+=line;
    }
    
    return 0;
}
int FileHandler::close()
{
     if (file_stream.is_open())
        file_stream.close();
    if(file_stream.fail()) return -1;
    return 0;
}