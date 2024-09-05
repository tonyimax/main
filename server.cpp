//
// Created by Hacker X on 2024/9/5.
//
#include <iostream>  //引用头文件
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string> //bzero函数用
#include <unistd.h> //close函数用

using namespace std; //使用命名空间std
int main() {
    cout << "Linux网络编程" << endl;
    cout << "LINUX NETWORK PROGRAMMING" << endl;
    cout << "TCP网络通信服务端(Server):" << endl;

    // socket(int domain, int type, int protocol);

    //创建套接字并返回句柄
    int socket_fd = socket(
            //地址系列
            AF_INET,
            //套接字类型
            SOCK_STREAM,
            //使用的协议
            IPPROTO_TCP
    );
    if(socket_fd<0){
        printf("Error: 创建服务端网络套接字失败!!!\n");
    }else{
        printf("===创建服务端网络套接字成功===,sockfd:%d\n",socket_fd);
    }

    //初始化Socket地址
    struct sockaddr_in tmpAddr;//声明网络地址结构对象
    //初始化所有成员为0,在声明时就初始化,不要在赋值后再初始化!
    bzero(&tmpAddr,//socket地址指针
          sizeof(tmpAddr));//socket地址大小
    tmpAddr.sin_family=AF_INET;//使用IPv4通信
    tmpAddr.sin_port= htons(8888);//指定监听端口,需要将主机字节序转换为网络字节序
    tmpAddr.sin_addr.s_addr= htons(INADDR_ANY);//指定监听地址,INADDR_ANY表示监听本地任何地址,需要将主机字节序转换为网络字节序

    //绑定
    // bind(int socket, const struct sockaddr *address, socklen_t address_len);
    //bind前要加上::不然后会调用到标准库的bind
    int bindRet = ::bind(
            //socket的句柄
            socket_fd,
            //网络地址结构
            (struct sockaddr *)&tmpAddr,//转换sockaddr_in地址为sockaddr指针
            //地址长度
            sizeof(tmpAddr)
    );
    if(0==bindRet){
        printf("===网络绑定成功===\n");
    }else{
        printf("Error:网络绑写失败!!!\n");
    }
    //监听
    //listen(int socket, int backlog);
    int listenRet = listen(
            //要监听的Socket句柄
            socket_fd,
            //挂起的连接队列的最大长度
            10
    );
    if (0==listenRet){
        printf("===监听网络成功===>监听端口:8888\n");
    }else{
        printf("Error:监听网络失败!!!\n");
    }



    struct sockaddr_in cliAddr;
    socklen_t len = sizeof (cliAddr);
    //接受连接,未接受客户端连接前,telnet无法连接
    // accept(int socket, struct sockaddr *restrict address,
    //         socklen_t *restrict address_len);
    int client_socket_fd = accept(
            //监听socket的句柄
            socket_fd,
            //保存客户端连接sockaddr结构指针
            (struct sockaddr*)&cliAddr,//转换sockaddr_in结构地址为sockaddr结构指针
            //保存客户端连接sockaddr结构长度socklen_t类型的指针
            &len //传入socklen_t类型地址
    );
    if(client_socket_fd<0){
        printf("接受客户端连接失败");
    }else{
        char tmpIpStr[32]={0};
        printf("接受客户端连接成功:IP:%s,PORT:%d\n", inet_ntop(AF_INET,&cliAddr.sin_addr,tmpIpStr,32),
               ntohs(cliAddr.sin_port) );
    }

    //循环接收数据
    while (true){
        //接收数据
        char buf[1024]={0};
        // recv(int socket, void *buffer, size_t length, int flags);
        ssize_t recv_len = recv(
                //socket
                client_socket_fd,
                //buffer
                buf,
                //size_t
                sizeof (buf),
                //flags
                0
        );
        printf("接收长度:%d,接收内容:%s,",recv_len,buf);

        //向客户端发送数据
        char sendBuf[1024]="服务器发来的消息:===HelloWorld From Server!===\n";
        //send(int socket, const void *buffer, size_t length, int flags);
        ssize_t send_len= send(
                //要发送数据socket
                client_socket_fd,
                //发送数据缓冲区
                sendBuf,
                //缓冲区大小
                strlen(sendBuf),
                //一组指定调用方式的标志
                0
        );
        printf("成功向客户端发送数据:长度:%d,内容:%s,", send_len,sendBuf);

        //如果客户端发来q,退出服务并关闭网络连接
        if(0==strcmp(buf,"q\r\n")||0== strcmp(buf,"q\n")){ //telnet发消息会在后面加上/r/n  普通终端q\n
            printf("客户端发来退出指令:%s\n",buf);
            break;
        }
    }

    //关闭客户端连接
    close(client_socket_fd);
    //关闭监听服务
    close(socket_fd);

    return  0;
}