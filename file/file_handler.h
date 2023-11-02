
/********************************************************************************
 * @author: MingYu Wang
 * @email: M.Wang56@newcastle.ac.uk
 * @date: 2023/11/01 16:03
 * @description:
 ********************************************************************************/


#pragma once

#include <string>
#include <fstream>

class FileHandler
{
public:
    explicit FileHandler();
    ~FileHandler();
    int open(const std::string &file_path,std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
    int write(const std::string &data);
    int read(std::string *data);
    int close();

private:
    int _fd;
    std::fstream file_stream;
    std::string file_path;
};