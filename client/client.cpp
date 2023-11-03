#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "request.h"

using std::cout;
using std::endl;
using std::string;

// 定义程序中使用的常量
#define SERVER_ADDRESS "127.0.0.1" // 服务器端IP地址
#define MSGSIZE 2048               // 收发缓冲区的大小
#define PORT 8888                  // 默认端口号

int main(int argc, char **argv)
{

    LOG_INIT("./store", LogLevel::INFO);
    short port = PORT;
    if (argc >= 2)
    {
        port = atoi(argv[1]);
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    int ret = connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        LOG(LogLevel::ERROR, "connect error");
        perror("connect error");
        exit(1);
    }
    else
    {
        LOG(LogLevel::INFO, "client by Stack start up ");
        cout << "client by Stack start up..." << endl;
    }

    while (1)
    {
        cout << "please input " << endl;

        string writebuffer;
        getline(std::cin, writebuffer);

        Request r(writebuffer);

        if (!r.is_valid_request())
        {
            continue;
        }
        write(socket_fd, writebuffer.c_str(), writebuffer.length());
        writebuffer.clear();

        char readbuffer[MSGSIZE] = {0};
        int ret = read(socket_fd, readbuffer, MSGSIZE);

        if (ret <= 0)
        {
            LOG(LogLevel::INFO, "server close connection ");
            cout << "server close connection " << endl;
        }
        if (ret == -1)
        {
            LOG(LogLevel::ERROR, "read error");
            perror("read error");
            exit(1);
        }

        cout<<"server reply : " << readbuffer<<endl;

        if (std::string(readbuffer).compare(std::string("exit"))  == 0 )
        {
            std::cout << "exit succeed" << std::endl;
            break;
        }
        memset(readbuffer, 0, MSGSIZE);
 
    }
    close(socket_fd);
}